#include "sound_buffer.h"
#include "../system/log.h"
#include "utils.h"

#define STB_VORBIS_HEADER_ONLY
#include "../../3rd/stb/stb_vorbis.c"

#include <al.h>

namespace ae {

SoundBuffer::SoundBuffer()
    : m_buffer{0}
    , m_audio_format{AudioFormat::UNKNOW}
{}

SoundBuffer::SoundBuffer(const std::filesystem::path &path)
    : m_buffer{0}
    , m_audio_format{AudioFormat::UNKNOW}
{
    loadFromFile(path);
}

SoundBuffer::SoundBuffer(const uint8_t *data, int32_t size)
    : m_buffer{0}
    , m_audio_format{AudioFormat::UNKNOW}
{
    loadFromMemory(data, size);
}

SoundBuffer::SoundBuffer(const std::vector<int16_t> samples,
                         AudioFormat audio_format,
                         int32_t sample_rate)
{
    create(samples, audio_format, sample_rate);
}

SoundBuffer::~SoundBuffer()
{
    destroy();
}

uint32_t SoundBuffer::getId() const
{
    return m_buffer;
}

AudioFormat SoundBuffer::getAudioFormat() const
{
    return m_audio_format;
}

const Time &SoundBuffer::getDuration() const
{
    return m_duration;
}

bool SoundBuffer::loadFromFile(const std::filesystem::path &path)
{
    int32_t channels = 0;
    int32_t sample_rate = 0;
    int16_t *output = nullptr;

    int32_t samples = stb_vorbis_decode_filename(path.string().c_str(),
                                                 &channels,
                                                 &sample_rate,
                                                 &output);

    if (samples < 0) {
        l_error("Error decoding Ogg Vorbis file: {}", path.string());
        return false;
    }

    std::vector<int16_t> pcm_data(output, output + samples * channels);

    std::free(output);

    AudioFormat audio_format = audio_utils::channelsToAudioFormat(channels);
    create(pcm_data, audio_format, sample_rate);

    return true;
}

bool SoundBuffer::loadFromMemory(const uint8_t *data, int32_t size)
{
    int32_t channels = 0;
    int32_t sample_rate = 0;
    int16_t *output = nullptr;

    int32_t samples = stb_vorbis_decode_memory(data, size, &channels, &sample_rate, &output);

    if (samples < 0) {
        l_error("Error decoding Ogg Vorbis file");
        return false;
    }

    std::vector<int16_t> pcm_data(output, output + samples * channels);

    std::free(output);

    AudioFormat audio_format = audio_utils::channelsToAudioFormat(channels);
    create(pcm_data, audio_format, sample_rate);

    return true;
}

void SoundBuffer::create(const std::vector<int16_t> samples,
                         AudioFormat audio_format,
                         int32_t sample_rate)
{
    destroy();

    if (samples.empty() || audio_format == AudioFormat::UNKNOW || sample_rate == 0)
        return;

    alGenBuffers(1, &m_buffer);
    alBufferData(m_buffer,
                 audio_utils::audioFormatToAl(audio_format),
                 samples.data(),
                 samples.size() * sizeof(int16_t),
                 sample_rate);

    int32_t frame_count = samples.size() / static_cast<int32_t>(audio_format);

    m_audio_format = audio_format;
    m_duration = seconds(static_cast<float>(frame_count) / sample_rate);
}

bool SoundBuffer::isValid() const
{
    return m_buffer != 0;
}

void SoundBuffer::destroy()
{
    if (m_buffer != 0) {
        alDeleteBuffers(1, &m_buffer);
        m_buffer = 0;
    }
}

void SoundBuffer::testPlay()
{
    uint32_t source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, m_buffer);
    alSourcePlay(source);
}

} // namespace ae
