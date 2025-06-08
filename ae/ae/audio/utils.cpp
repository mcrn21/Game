#include "utils.h"

#include <al.h>

namespace ae::audio_utils {

AudioFormat channelsToAudioFormat(int32_t channels_num)
{
    if (channels_num == 1)
        return AudioFormat::FORMAT_MONO16;
    else if (channels_num == 2)
        return AudioFormat::FORMAT_STEREO16;
    else
        return AudioFormat::UNKNOW;
}

int32_t audioFormatToAl(AudioFormat audio_format)
{
    switch (audio_format) {
    case AudioFormat::FORMAT_MONO16:
        return AL_FORMAT_MONO16;
    case AudioFormat::FORMAT_STEREO16:
        return AL_FORMAT_STEREO16;
    case AudioFormat::UNKNOW:
        return -1;
    }
}

} // namespace ae::audio_utils
