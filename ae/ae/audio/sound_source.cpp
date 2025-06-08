#include "sound_source.h"

#include <al.h>

namespace ae {

SoundSource::SoundSource()
    : m_source{0}
{
    alGenSources(1, &m_source);
    alSourcei(m_source, AL_BUFFER, 0);
}

SoundSource::~SoundSource()
{
    if (m_source != 0) {
        alSourcei(m_source, AL_BUFFER, 0);
        alDeleteSources(1, &m_source);
    }
}

bool SoundSource::isValid() const
{
    return m_source != 0;
}

SoundSource::Status SoundSource::getStatus() const
{
    if (m_source == 0)
        return STOPPED;

    ALint status;
    alGetSourcei(m_source, AL_SOURCE_STATE, &status);

    switch (status) {
    case AL_INITIAL:
    case AL_STOPPED:
        return STOPPED;
    case AL_PAUSED:
        return PAUSED;
    case AL_PLAYING:
        return PLAYING;
    }

    return STOPPED;
}

float SoundSource::getPitch() const
{
    if (m_source == 0)
        return 0.0f;

    ALfloat pitch;
    alGetSourcef(m_source, AL_PITCH, &pitch);
    return pitch;
}

void SoundSource::setPitch(float pitch)
{
    if (m_source != 0)
        alSourcef(m_source, AL_PITCH, pitch);
}

float SoundSource::getVolume() const
{
    if (m_source == 0)
        return 0.0f;

    ALfloat gain;
    alGetSourcef(m_source, AL_GAIN, &gain);
    return gain;
}

void SoundSource::setVolume(float volume)
{
    if (m_source != 0)
        alSourcef(m_source, AL_GAIN, volume);
}

bool SoundSource::isLoop() const
{
    if (m_source == 0)
        return false;
    ALint loop;
    alGetSourcei(m_source, AL_LOOPING, &loop);
    return loop != 0;
}

void SoundSource::setLoop(bool loop)
{
    if (m_source != 0)
        alSourcei(m_source, AL_LOOPING, loop);
}

vec3 SoundSource::getPosition() const
{
    if (m_source == 0)
        return vec3{0.0f};

    vec3 position;
    alGetSource3f(m_source, AL_POSITION, &position.x, &position.y, &position.z);
    return position;
}

void SoundSource::setPosition(const vec3 &position)
{
    if (m_source != 0)
        alSource3f(m_source, AL_POSITION, position.x, position.y, position.z);
}

bool SoundSource::isRelativeToListener() const
{
    if (m_source == 0)
        return false;

    ALint relative;
    alGetSourcei(m_source, AL_SOURCE_RELATIVE, &relative);
    return relative != 0;
}

void SoundSource::setRelativeToListener(bool relative)
{
    if (m_source != 0)
        alSourcei(m_source, AL_SOURCE_RELATIVE, relative);
}

float SoundSource::getMinDistance() const
{
    if (m_source == 0)
        return 0.0f;

    ALfloat distance;
    alGetSourcef(m_source, AL_REFERENCE_DISTANCE, &distance);
    return distance;
}

void SoundSource::setMinDistance(float distance)
{
    if (m_source != 0)
        alSourcef(m_source, AL_REFERENCE_DISTANCE, distance);
}

float SoundSource::getAttenuation() const
{
    if (m_source == 0)
        return 0.0f;

    ALfloat attenuation;
    alGetSourcef(m_source, AL_ROLLOFF_FACTOR, &attenuation);
    return attenuation;
}

void SoundSource::setAttenuation(float attenuation)
{
    if (m_source != 0)
        alSourcef(m_source, AL_ROLLOFF_FACTOR, attenuation);
}

} // namespace ae
