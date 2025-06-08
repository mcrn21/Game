#ifndef AE_FRAPHICS_ENUMS_H
#define AE_FRAPHICS_ENUMS_H

namespace ae {

enum class TextureFormat { RED = 1, RGB = 3, RGBA = 4, DEPTH = 5 };
enum class TextureType { DEFAULT, CUBE_MAP };
enum class ShaderType { VERTEX, GEOMETRY, FRAGMENT };
enum class BufferType { ARRAY_BUFFER, ELEMENT_ARRAY_BUFFER, SHADER_STORAGE_BUFFER, UNIFORM_BUFFER };
enum class UsageType { STATIC, DYNAMIC, STREAM };
enum class PrimitiveType { TRIANGLES, LINES };
enum class DataType { INT, FLOAT };
enum class RenderTextureType { COLOR, DEPTH };

} // namespace ae

#endif // AE_FRAPHICS_ENUMS_H
