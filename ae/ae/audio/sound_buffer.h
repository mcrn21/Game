#ifndef AE_SOUND_BUFFER_H
#define AE_SOUND_BUFFER_H

#include "../system/time.h"
#include "enums.h"

#include <filesystem>
#include <vector>

namespace ae {

class SoundBuffer
{
public:
    SoundBuffer();
    SoundBuffer(const std::filesystem::path &path);
    SoundBuffer(const uint8_t *data, int32_t size);
    SoundBuffer(const std::vector<int16_t> samples, AudioFormat audio_format, int32_t sample_rate);
    ~SoundBuffer();

    uint32_t getId() const;
    AudioFormat getAudioFormat() const;
    const Time &getDuration() const;

    bool loadFromFile(const std::filesystem::path &path);
    bool loadFromMemory(const uint8_t *data, int32_t size);

    void create(const std::vector<int16_t> samples, AudioFormat audio_format, int32_t sample_rate);
    bool isValid() const;
    void destroy();

    void testPlay();

private:
    uint32_t m_buffer;
    // std::vector<int16_t> m_samples;
    AudioFormat m_audio_format;
    Time m_duration;
};

} // namespace ae

#endif // AE_SOUND_BUFFER_H
