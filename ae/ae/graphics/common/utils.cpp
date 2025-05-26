#include "utils.h"

#include <GL/glew.h>

namespace ae::graphics_utils {

int32_t textureFormatToGl(TextureFormat format)
{
    switch (format) {
    case TextureFormat::RED:
        return GL_RED;
    case TextureFormat::RGB:
        return GL_RGB;
    case TextureFormat::RGBA:
        return GL_RGBA;
    case TextureFormat::DEPTH:
        return GL_DEPTH_COMPONENT;
    }
}

int32_t textureTypeToGl(TextureType type)
{
    switch (type) {
    case TextureType::DEFAULT:
        return GL_TEXTURE_2D;
    case TextureType::CUBE_MAP:
        return GL_TEXTURE_CUBE_MAP;
    }
}

int32_t shaderTypeToGl(ShaderType type)
{
    switch (type) {
    case ShaderType::VERTEX:
        return GL_VERTEX_SHADER;
    case ShaderType::GEOMETRY:
        return GL_GEOMETRY_SHADER;
    case ShaderType::FRAGMENT:
        return GL_FRAGMENT_SHADER;
    }
}

int32_t bufferTypeToGl(BufferType type)
{
    switch (type) {
    case BufferType::ARRAY_BUFFER:
        return GL_ARRAY_BUFFER;
    case BufferType::ELEMENT_ARRAY_BUFFER:
        return GL_ELEMENT_ARRAY_BUFFER;
    case BufferType::SHADER_STORAGE_BUFFER:
        return GL_SHADER_STORAGE_BUFFER;
    case BufferType::UNIFORM_BUFFER:
        return GL_UNIFORM_BUFFER;
    }
}

int32_t usageTypeToGl(UsageType type)
{
    switch (type) {
    case UsageType::STATIC:
        return GL_STATIC_DRAW;
    case UsageType::DYNAMIC:
        return GL_DYNAMIC_DRAW;
    case UsageType::STREAM:
        return GL_STREAM_DRAW;
    }
}

int32_t primitiveTypeToGl(PrimitiveType type)
{
    switch (type) {
    case PrimitiveType::TRIANGLES:
        return GL_TRIANGLES;
    case PrimitiveType::LINES:
        return GL_LINES;
    }
}

int32_t dataTypeToGl(DataType type)
{
    switch (type) {
    case DataType::INT:
        return GL_INT;
    case DataType::FLOAT:
        return GL_FLOAT;
    }
}

int32_t boolToGl(bool value)
{
    return value ? GL_TRUE : GL_FALSE;
}

int32_t renderTextureTypeToGl(RenderTextureType type)
{
    switch (type) {
    case RenderTextureType::COLOR:
        return GL_COLOR_ATTACHMENT0;
    case RenderTextureType::DEPTH:
        return GL_DEPTH_ATTACHMENT;
    }
}

TextureFormat renderTextureTypeToTextureFormat(RenderTextureType type)
{
    switch (type) {
    case RenderTextureType::COLOR:
        return TextureFormat::RGB;
    case RenderTextureType::DEPTH:
        return TextureFormat::DEPTH;
    }
}

std::vector<uint32_t> utf8ToCodepoints(const std::string &str)
{
    std::vector<uint32_t> codepoints;
    int32_t i = 0;

    while (i < str.size()) {
        uint8_t c = str[i];
        uint32_t cp = 0;

        if ((c & 0x80) == 0) {
            cp = c;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            cp = ((str[i] & 0x1F) << 6) | (str[i + 1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            cp = ((str[i] & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            cp = ((str[i] & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) | ((str[i + 2] & 0x3F) << 6)
                 | (str[i + 3] & 0x3F);
            i += 4;
        } else {
            // invalid byte, skip
            i += 1;
        }

        codepoints.push_back(cp);
    }
    return codepoints;
}

} // namespace ae::graphics_utils
