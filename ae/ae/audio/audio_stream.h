#ifndef AE_AUDIO_STREAM_H
#define AE_AUDIO_STREAM_H

#include "../system/memory.h"
#include "enums.h"

#include <filesystem>
#include <vector>

namespace ae {

namespace priv {

struct AudioStreamImpl
{
    virtual ~AudioStreamImpl() = default;

    virtual bool loadFromFile(const std::filesystem::path &path) = 0;
    virtual void close() = 0;

    virtual bool isValid() const = 0;

    virtual AudioFormat getAudioFormat() const = 0;
    virtual int32_t getSampleRate() const = 0;
    virtual bool isEOF() const = 0;

    virtual bool read(std::vector<int16_t> &output, int32_t frames_to_read) = 0;
    virtual bool seek(int32_t frame_offset) = 0;
};

} // namespace priv

class AudioStream
{
public:
    AudioStream();
    ~AudioStream() = default;

    bool loadFromFile(const std::filesystem::path &path);
    void close();

    bool isValid() const;

    AudioFormat getAudioFormat() const;
    int32_t getSampleRate() const;
    bool isEOF() const;

    bool read(std::vector<int16_t> &output, int32_t frames_to_read);
    bool seek(int32_t frame_offset);

private:
    u_ptr<priv::AudioStreamImpl> m_audio_stream_impl;
};

} // namespace ae

#endif // AE_AUDIO_STREAM_H
