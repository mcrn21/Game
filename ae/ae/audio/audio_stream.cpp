#include "audio_stream.h"
#include "utils.h"

#define STB_VORBIS_HEADER_ONLY
#include "../../3rd/stb/stb_vorbis.c"

namespace ae {

namespace priv {

struct StbVorbisAudioStreamImpl : public AudioStreamImpl
{
    ~StbVorbisAudioStreamImpl() { close(); }

    bool loadFromFile(const std::filesystem::path &path)
    {
        int32_t error;
        vorbis = stb_vorbis_open_filename(path.c_str(), &error, nullptr);
        if (!vorbis)
            return false;

        stb_vorbis_info info = stb_vorbis_get_info(vorbis);

        audio_format = audio_utils::channelsToAudioFormat(info.channels);
        sample_rate = info.sample_rate;

        return true;
    }

    void close()
    {
        if (vorbis) {
            stb_vorbis_close(vorbis);
            vorbis = nullptr;
            audio_format = AudioFormat::UNKNOW;
            sample_rate = 0;
        }
    }

    bool isValid() const { return vorbis != nullptr; }

    AudioFormat getAudioFormat() const { return audio_format; }
    int32_t getSampleRate() const { return sample_rate; }
    bool isEOF() const { return m_eof; }

    bool read(std::vector<int16_t> &output, int32_t frames_to_read)
    {
        if (!vorbis)
            return false;

        output.resize(frames_to_read * static_cast<int32_t>(audio_format));

        int32_t samples_read
            = stb_vorbis_get_samples_short_interleaved(vorbis,
                                                       static_cast<int32_t>(audio_format),
                                                       output.data(),
                                                       frames_to_read
                                                           * static_cast<int32_t>(audio_format));

        m_eof = samples_read <= 0;

        return samples_read > 0;
    }

    bool seek(int32_t frame_offset)
    {
        if (!vorbis)
            return false;

        // frame_offset — номер кадра, а stb_vorbis_seek принимает номер сэмпла (одного канала)
        // Но stb_vorbis_seek считает sample_number как индекс сэмпла с учётом каналов (т.е. уже interleaved)
        // Нет! На самом деле stb_vorbis_seek ожидает позицию в сэмплах (моно-канальных), т.е. учитывая каналы

        // В документации stb_vorbis: sample_number — sample index (в моно-канальном пространстве),
        // то есть если у вас 2 канала, 1 кадр = 2 сэмпла, значит надо умножить на каналы

        int32_t samples_to_seek = frame_offset * static_cast<int32_t>(audio_format);
        bool result = stb_vorbis_seek(vorbis, samples_to_seek) == 1;

        m_eof = false;

        return result;
    }

    stb_vorbis *vorbis = nullptr;
    AudioFormat audio_format = AudioFormat::UNKNOW;
    int32_t sample_rate = 0;
    bool m_eof;
};

} // namespace priv

AudioStream::AudioStream() {}

bool AudioStream::loadFromFile(const std::filesystem::path &path)
{
    auto audio_stream = createUnique<priv::StbVorbisAudioStreamImpl>();
    if (!audio_stream->loadFromFile(path))
        return false;

    m_audio_stream_impl = std::move(audio_stream);
    return true;
}

void AudioStream::close()
{
    if (m_audio_stream_impl)
        m_audio_stream_impl->close();
}

bool AudioStream::isValid() const
{
    if (!m_audio_stream_impl)
        return false;
    return m_audio_stream_impl->isValid();
}

AudioFormat AudioStream::getAudioFormat() const
{
    if (!m_audio_stream_impl)
        return AudioFormat::UNKNOW;
    return m_audio_stream_impl->getAudioFormat();
}

int32_t AudioStream::getSampleRate() const
{
    if (!m_audio_stream_impl)
        return 0;
    return m_audio_stream_impl->getSampleRate();
}

bool AudioStream::isEOF() const
{
    if (!m_audio_stream_impl)
        return true;
    return m_audio_stream_impl->isEOF();
}

bool AudioStream::read(std::vector<int16_t> &output, int32_t frames_to_read)
{
    if (!m_audio_stream_impl)
        return false;
    return m_audio_stream_impl->read(output, frames_to_read);
}

bool AudioStream::seek(int32_t frame_offset)
{
    if (!m_audio_stream_impl)
        return false;
    return m_audio_stream_impl->seek(frame_offset);
}

} // namespace ae
