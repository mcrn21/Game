#include "engine_context_object.h"

namespace ae {

EngineContextObject::EngineContextObject(EngineContext &engine_ctx)
    : m_engine_ctx{engine_ctx}
{}

EngineContext &EngineContextObject::getEngineContext()
{
    return m_engine_ctx;
}

const EngineContext &EngineContextObject::getEngineContext() const
{
    return m_engine_ctx;
}

} // namespace ae
