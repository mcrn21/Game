#ifndef AE_CUBE_H
#define AE_CUBE_H

#include "mesh.h"

namespace ae {

class Shape : public Mesh
{
public:
    Shape();
    ~Shape() = default;

    void createTriangle(float radius = 2.0f);
    void createCube(float half_size = 2.0f);
    void createSphere(float radius = 2.0f, int32_t segments = 16, int32_t rings = 16);
    void createCapsule(float radius = 1.0f,
                       float height = 2.0f,
                       int32_t segments = 16,
                       int32_t rings = 8);
};

} // namespace ae

#endif // AE_CUBE_H
