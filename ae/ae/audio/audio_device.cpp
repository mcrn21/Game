#include "audio_device.h"
#include "../system/log.h"

#include <al.h>
#include <alc.h>

namespace ae {

namespace priv {

struct OpenAlAudioDeviceImpl : public AudioDeviceImpl
{
    ~OpenAlAudioDeviceImpl()
    {
        // Destroy the context
        alcMakeContextCurrent(nullptr);
        if (audio_context)
            alcDestroyContext(audio_context);

        // Destroy the device
        if (audio_device)
            alcCloseDevice(audio_device);
    }

    bool create()
    {
        audio_device = alcOpenDevice(nullptr);
        if (!audio_device) {
            l_error("The OpenAL device has not been created");
            return false;
        }

        audio_context = alcCreateContext(audio_device, nullptr);
        if (!audio_context) {
            l_error("The OpenAL context has not been created");
            alcCloseDevice(audio_device);
            return false;
        }

        alcMakeContextCurrent(audio_context);

        alListenerf(AL_GAIN, listener_volume);

        float orientation[] = {listener_direction.x,
                               listener_direction.y,
                               listener_direction.z,
                               listener_up_vector.x,
                               listener_up_vector.y,
                               listener_up_vector.z};
        alListener3f(AL_POSITION, listener_position.x, listener_position.y, listener_position.z);

        alListenerfv(AL_ORIENTATION, orientation);

        l_info("Init OpenAL");

        return true;
    }

    float getGlobalVolume() const { return listener_volume; }
    void setGlobalVolume(float volume)
    {
        listener_volume = volume;
        if (audio_context)
            alListenerf(AL_GAIN, volume);
    }

    vec3 getListenerPosition() const { return listener_position; }
    void setListenerPosition(const vec3 &position)
    {
        listener_position = position;
        if (audio_context)
            alListener3f(AL_POSITION, position.x, position.y, position.z);
    }

    vec3 getListenerDirection() const { return listener_direction; }
    void setListenerDirection(const vec3 &direction)
    {
        listener_direction = direction;
        if (audio_context) {
            float orientation[] = {direction.x,
                                   direction.y,
                                   direction.z,
                                   listener_up_vector.x,
                                   listener_up_vector.y,
                                   listener_up_vector.z};
            alListenerfv(AL_ORIENTATION, orientation);
        }
    }

    vec3 getListenerUpVector() const { return listener_up_vector; }
    void setListenerUpVector(const vec3 &up_vector)
    {
        listener_up_vector = up_vector;
        if (audio_context) {
            float orientation[] = {listener_direction.x,
                                   listener_direction.y,
                                   listener_direction.z,
                                   up_vector.x,
                                   up_vector.y,
                                   up_vector.z};
            alListenerfv(AL_ORIENTATION, orientation);
        }
    }

    ALCdevice *audio_device = nullptr;
    ALCcontext *audio_context = nullptr;

    float listener_volume = 1.f;
    vec3 listener_position{0.0f};
    vec3 listener_direction{0.0f, 0.0f, -1.0f};
    vec3 listener_up_vector{0.0f, 1.f, 0.0f};
};

} // namespace priv

AudioDevice::AudioDevice()
{
    auto audio_device = createUnique<priv::OpenAlAudioDeviceImpl>();
    if (audio_device->create())
        m_audio_device_impl = std::move(audio_device);
}

AudioDevice::~AudioDevice() {}

bool AudioDevice::isValid() const
{
    return m_audio_device_impl != nullptr;
}

float AudioDevice::getGlobalVolume() const
{
    if (!m_audio_device_impl)
        return 0.0f;
    return m_audio_device_impl->getGlobalVolume();
}

void AudioDevice::setGlobalVolume(float volume)
{
    if (!m_audio_device_impl)
        return;
    m_audio_device_impl->setGlobalVolume(volume);
}

vec3 AudioDevice::getListenerPosition() const
{
    if (!m_audio_device_impl)
        return vec3{0.0f};
    return m_audio_device_impl->getListenerPosition();
}

void AudioDevice::setListenerPosition(const vec3 &position)
{
    if (!m_audio_device_impl)
        return;
    m_audio_device_impl->setListenerPosition(position);
}

vec3 AudioDevice::getListenerDirection() const
{
    if (!m_audio_device_impl)
        return vec3{0.0f};
    return m_audio_device_impl->getListenerDirection();
}

void AudioDevice::setListenerDirection(const vec3 &direction)
{
    if (!m_audio_device_impl)
        return;
    m_audio_device_impl->setListenerDirection(direction);
}

vec3 AudioDevice::getListenerUpVector() const
{
    if (!m_audio_device_impl)
        return vec3{0.0f};
    return m_audio_device_impl->getListenerUpVector();
}

void AudioDevice::setListenerUpVector(const vec3 &up_vector)
{
    if (!m_audio_device_impl)
        return;
    m_audio_device_impl->setListenerUpVector(up_vector);
}

} // namespace ae
