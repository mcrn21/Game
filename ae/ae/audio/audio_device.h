#ifndef AE_AUDIO_DEVICE_H
#define AE_AUDIO_DEVICE_H

#include "../system/memory.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

namespace priv {

struct AudioDeviceImpl
{
    virtual ~AudioDeviceImpl() = default;
    virtual bool create() = 0;

    virtual float getGlobalVolume() const = 0;
    virtual void setGlobalVolume(float volume) = 0;

    virtual vec3 getListenerPosition() const = 0;
    virtual void setListenerPosition(const vec3 &position) = 0;

    virtual vec3 getListenerDirection() const = 0;
    virtual void setListenerDirection(const vec3 &direction) = 0;

    virtual vec3 getListenerUpVector() const = 0;
    virtual void setListenerUpVector(const vec3 &up_vector) = 0;
};

} // namespace priv

class AudioDevice
{
public:
    AudioDevice();
    ~AudioDevice();

    bool isValid() const;

    float getGlobalVolume() const;
    void setGlobalVolume(float volume);

    vec3 getListenerPosition() const;
    void setListenerPosition(const vec3 &position);

    vec3 getListenerDirection() const;
    void setListenerDirection(const vec3 &direction);

    vec3 getListenerUpVector() const;
    void setListenerUpVector(const vec3 &up_vector);

private:
    u_ptr<priv::AudioDeviceImpl> m_audio_device_impl;
};

} // namespace ae

#endif // AE_AUDIO_DEVICE_H
