#ifndef AE_ENGINE_H
#define AE_ENGINE_H

#include "assets/asset_loaders.h"
#include "engine_context.h"

#include "system/clock.h"

namespace ae {

class Engine : public EngineContext
{
public:
    Engine();
    ~Engine() = default;

    bool create(const Config &config = {});

    Config &getConfig();

    int32_t exec();
    void exit();

private:
    Config m_config;
    bool m_running;
};

} // namespace ae

#endif // AE_ENGINE_H
