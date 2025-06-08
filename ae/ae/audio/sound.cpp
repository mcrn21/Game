#include "sound.h"

#include <al.h>

namespace ae {

const s_ptr<SoundBuffer> &Sound::getSoundBuffer() const
{
    return m_sound_buffer;
}

void Sound::setSoundBuffer(const s_ptr<SoundBuffer> &sound_buffer)
{
    m_sound_buffer = sound_buffer;

    if (m_source != 0)
        alSourcei(m_source, AL_BUFFER, sound_buffer ? sound_buffer->getId() : 0);
}

Time Sound::getRemaining() const
{
    if (m_source == 0 || !m_sound_buffer)
        return {};

    ALfloat seconds_played = 0.0f;
    alGetSourcef(m_source, AL_SEC_OFFSET, &seconds_played);

    return seconds(m_sound_buffer->getDuration().asSeconds() - seconds_played);
}

void Sound::play()
{
    if (m_source != 0 && m_sound_buffer) {
        alSourcePlay(m_source);
    }
}

void Sound::pause()
{
    if (m_source != 0 && m_sound_buffer)
        alSourcePause(m_source);
}

void Sound::stop()
{
    if (m_source != 0 && m_sound_buffer)
        alSourceStop(m_source);
}

} // namespace ae
