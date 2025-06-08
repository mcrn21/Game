#ifndef AE_SOUND_H
#define AE_SOUND_H

#include "../system/memory.h"
#include "sound_buffer.h"
#include "sound_source.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

class Sound : public SoundSource
{
public:
    Sound() = default;
    ~Sound() = default;

    const s_ptr<SoundBuffer> &getSoundBuffer() const;
    void setSoundBuffer(const s_ptr<SoundBuffer> &sound_buffer);

    Time getRemaining() const;

    void play();
    void pause();
    void stop();

private:
    s_ptr<SoundBuffer> m_sound_buffer;
};

} // namespace ae

#endif // AE_SOUND_H
