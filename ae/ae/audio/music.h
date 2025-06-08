#ifndef AE_MUSIC_H
#define AE_MUSIC_H

#include "audio_stream.h"
#include "sound_source.h"

#include <thread>

namespace ae {

class Music : public SoundSource
{
public:
    Music();
    ~Music();

    bool loadFromFile(const std::filesystem::path &path);

    bool isValid() const;

    void play();
    void pause();
    void stop();

private:
    bool streamBuffer(int32_t buffer);
    void streamThread();

private:
    static constexpr int32_t BUFFER_SIZE_FRAMES = 4096;
    static constexpr int32_t BUFFERS_COUNT = 3;

    AudioStream m_audio_stream;
    uint32_t m_buffers[BUFFERS_COUNT];

    std::thread m_thread;
    std::atomic<bool> m_running;
};

} // namespace ae

#endif // AE_MUSIC_H
