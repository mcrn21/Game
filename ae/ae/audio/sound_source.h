#ifndef AE_SOUND_SOURCE_H
#define AE_SOUND_SOURCE_H

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

class SoundSource
{
public:
    enum Status { STOPPED, PAUSED, PLAYING };

    SoundSource();
    virtual ~SoundSource();

    virtual bool isValid() const;
    virtual Status getStatus() const;

    float getPitch() const;
    void setPitch(float pitch);

    float getVolume() const;
    void setVolume(float volume);

    bool isLoop() const;
    void setLoop(bool loop);

    vec3 getPosition() const;
    void setPosition(const vec3 &position);

    bool isRelativeToListener() const;
    void setRelativeToListener(bool relative);

    float getMinDistance() const;
    void setMinDistance(float distance);

    float getAttenuation() const;
    void setAttenuation(float attenuation);

    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

protected:
    uint32_t m_source;
};

} // namespace ae

#endif // AE_SOUND_SOURCE_H
