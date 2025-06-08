#ifndef AE_ENGINE_CONTEXT_OBJECT_H
#define AE_ENGINE_CONTEXT_OBJECT_H

namespace ae {

class EngineContext;

class EngineContextObject
{
public:
    EngineContextObject(EngineContext &engine_ctx);
    virtual ~EngineContextObject() = default;

    EngineContext &getEngineContext();
    const EngineContext &getEngineContext() const;

private:
    EngineContext &m_engine_ctx;
};

} // namespace ae

#endif // AE_ENGINE_CONTEXT_OBJECT_H
