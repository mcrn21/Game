#ifndef AE_AUDIO_UTILS_UTILS_H
#define AE_AUDIO_UTILS_UTILS_H

#include "enums.h"

#include <stdint.h>

namespace ae::audio_utils {

AudioFormat channelsToAudioFormat(int32_t channels_num);
int32_t audioFormatToAl(AudioFormat audio_format);

} // namespace ae::audio_utils

#endif // AE_AUDIO_UTILS_UTILS_H
