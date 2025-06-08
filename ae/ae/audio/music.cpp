#include "music.h"
#include "utils.h"

#include <al.h>

namespace ae {

Music::Music()
{
    for (int32_t i = 0; i < BUFFERS_COUNT; ++i)
        m_buffers[i] = 0;
    alGenBuffers(BUFFERS_COUNT, m_buffers);
}

Music::~Music()
{
    stop();
    alDeleteBuffers(BUFFERS_COUNT, m_buffers);
}

bool Music::loadFromFile(const std::filesystem::path &path)
{
    return m_audio_stream.loadFromFile(path);
}

bool Music::isValid() const
{
    return m_audio_stream.isValid() && m_source != 0;
}

void Music::play()
{
    if (!isValid())
        return;

    if (getStatus() == STOPPED) {
        for (auto buffer : m_buffers) {
            if (!streamBuffer(buffer))
                return;
        }
        alSourceQueueBuffers(m_source, 3, m_buffers);
    }

    alSourcePlay(m_source);

    if (!m_running) {
        m_running = true;
        m_thread = std::thread{&Music::streamThread, this};
    }
}

void Music::pause()
{
    if (!isValid())
        return;

    m_running = false;
    if (m_thread.joinable())
        m_thread.join();

    alSourcePause(m_source);
}

void Music::stop()
{
    m_running = false;
    if (m_thread.joinable())
        m_thread.join();

    alSourceStop(m_source);

    m_audio_stream.seek(0);

    ALint queued = 0;
    alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
    while (queued-- > 0) {
        uint32_t buffer;
        alSourceUnqueueBuffers(m_source, 1, &buffer);
    }
}

bool Music::streamBuffer(int32_t buffer)
{
    std::vector<int16_t> pcm;
    if (!m_audio_stream.read(pcm, BUFFER_SIZE_FRAMES))
        return false;

    alBufferData(buffer,
                 audio_utils::audioFormatToAl(m_audio_stream.getAudioFormat()),
                 pcm.data(),
                 static_cast<ALsizei>(pcm.size() * sizeof(int16_t)),
                 m_audio_stream.getSampleRate());

    return true;
}

void Music::streamThread()
{
    while (m_running) {
        ALint processed = 0;
        alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

        while (processed-- > 0) {
            uint32_t buffer;
            alSourceUnqueueBuffers(m_source, 1, &buffer);

            if (streamBuffer(buffer))
                alSourceQueueBuffers(m_source, 1, &buffer);
            else {
                m_running = false; // EOF
                break;
            }
        }

        ALint state;
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && m_running)
            alSourcePlay(m_source);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

} // namespace ae
