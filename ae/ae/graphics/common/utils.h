#ifndef AE_GRAPHICS_UTILS_UTILS_H
#define AE_GRAPHICS_UTILS_UTILS_H

#include "enums.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ae::graphics_utils {

int32_t textureFormatToGl(TextureFormat format);
int32_t textureTypeToGl(TextureType type);
int32_t shaderTypeToGl(ShaderType type);
int32_t bufferTypeToGl(BufferType type);
int32_t usageTypeToGl(UsageType type);
int32_t primitiveTypeToGl(PrimitiveType type);
int32_t dataTypeToGl(DataType type);
int32_t boolToGl(bool value);
int32_t renderTextureTypeToGl(RenderTextureType type);
TextureFormat renderTextureTypeToTextureFormat(RenderTextureType type);

std::vector<uint32_t> utf8ToCodepoints(const std::string &str);

} // namespace ae::graphics_utils

#endif // AE_GRAPHICS_UTILS_UTILS_H
