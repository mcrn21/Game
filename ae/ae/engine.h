#ifndef AE_ENGINE_H
#define AE_ENGINE_H

#include "assets/asset_loaders.h"
#include "engine_context.h"

#include "system/clock.h"

#include "../3rd/sigslot/signal.hpp"

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

public:
    sigslot::signal<> created;

private:
    Config m_config;
    bool m_running;
};

} // namespace ae

#endif // AE_ENGINE_H
