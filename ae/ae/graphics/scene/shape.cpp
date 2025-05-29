#include "shape.h"

#include <glm/gtc/constants.hpp>

namespace ae {

Shape::Shape() {}

void Shape::createTriangle(float radius)
{
    std::vector<Vertex> vertices;

    float angles[3] = {
        glm::radians(90.0f),
        glm::radians(210.0f),
        glm::radians(330.0f),
    };

    for (int32_t i = 0; i < 3; ++i) {
        float x = radius * cos(angles[i]);
        float y = radius * sin(angles[i]);
        glm::vec3 position = glm::vec3(x, y, 0.0f);
        glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec2 tex_coord = glm::vec2((x / (2.0f * radius)) + 0.5f, (y / (2.0f * radius)) + 0.5f);

        vertices.push_back({position, normal, Color::white.getColor(), tex_coord});
    }

    std::vector<uint32_t> indices = {0, 1, 2};

    Mesh::create(vertices, indices, createShared<Material>());
}

void Shape::createCube(float half_size)
{
    struct Face
    {
        vec3 normal;
        vec3 verts[4];
        vec2 uvs[4];
    };

    Face faces[] = {// Front (+Z)
                    {{0.0f, 0.0f, 1.0f},
                     {{-half_size, -half_size, half_size},
                      {half_size, -half_size, half_size},
                      {half_size, half_size, half_size},
                      {-half_size, half_size, half_size}},
                     {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
                    // Back (-Z)
                    {{0.0f, 0.0f, -1.0f},
                     {{half_size, -half_size, -half_size},
                      {-half_size, -half_size, -half_size},
                      {-half_size, half_size, -half_size},
                      {half_size, half_size, -half_size}},
                     {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
                    // Left (-X)
                    {{-1.0f, 0.0f, 0.0f},
                     {{-half_size, -half_size, -half_size},
                      {-half_size, -half_size, half_size},
                      {-half_size, half_size, half_size},
                      {-half_size, half_size, -half_size}},
                     {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
                    // Right (+X)
                    {{1.0f, 0.0f, 0.0f},
                     {{half_size, -half_size, half_size},
                      {half_size, -half_size, -half_size},
                      {half_size, half_size, -half_size},
                      {half_size, half_size, half_size}},
                     {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
                    // Top (+Y)
                    {{0.0f, 1.0f, 0.0f},
                     {{-half_size, half_size, half_size},
                      {half_size, half_size, half_size},
                      {half_size, half_size, -half_size},
                      {-half_size, half_size, -half_size}},
                     {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
                    // Bottom (-Y)
                    {{0.0f, -1.0f, 0.0f},
                     {{-half_size, -half_size, -half_size},
                      {half_size, -half_size, -half_size},
                      {half_size, -half_size, half_size},
                      {-half_size, -half_size, half_size}},
                     {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}}};

    std::vector<Vertex> vertices;
    for (const auto &face : faces) {
        for (int32_t i = 0; i < 4; ++i) {
            Vertex vertex;
            vertex.position = face.verts[i];
            vertex.normal = face.normal;
            vertex.color = Color::white.getColor();
            vertex.tex_coords = face.uvs[i];
            vertices.push_back(vertex);
        }
    }

    std::vector<uint32_t> indices;
    for (int32_t i = 0; i < 6; ++i) {
        int32_t base_index = i * 4;
        indices.push_back(base_index + 0);
        indices.push_back(base_index + 1);
        indices.push_back(base_index + 2);
        indices.push_back(base_index + 2);
        indices.push_back(base_index + 3);
        indices.push_back(base_index + 0);
    }

    Mesh::create(vertices, indices, createShared<Material>());
}

void Shape::createSphere(float radius, int32_t segments, int32_t rings)
{
    createCapsule(radius, 0.0f, segments, rings);
}

void Shape::createCapsule(float radius, float height, int32_t segments, int32_t rings)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    int32_t ring_count = rings * 2 + 1; // верхняя полусфера + цилиндр + нижняя полусфера
    float cylinder_half_height = height / 2.0f;

    for (int32_t y = 0; y <= ring_count; ++y) {
        float v = (float) y / ring_count;
        float phi = (0.5f - v) * glm::pi<float>();
        float sin_phi = sin(phi);
        float cos_phi = cos(phi);

        float ring_radius = radius * cos_phi;
        float ring_y = radius * sin_phi;

        if (y < rings)
            ring_y += cylinder_half_height;
        else if (y > rings)
            ring_y -= cylinder_half_height;

        for (int32_t x = 0; x <= segments; ++x) {
            // Повтор последней вершины с x = 0 на конце для замыкания (шов)
            float u = (float) x / segments;
            float theta = u * glm::two_pi<float>();
            float sin_theta = sin(theta);
            float cos_theta = cos(theta);

            glm::vec3 normal = {cos_theta * cos_phi, sin_phi, sin_theta * cos_phi};
            glm::vec3 position = {ring_radius * cos_theta, ring_y, ring_radius * sin_theta};

            // Перемещаем U немного внутрь от 0.0 и 1.0 чтобы избежать артефактов
            float safe_u = glm::mix(0.001f, 0.999f, u);
            glm::vec2 tex_coord = {safe_u, v};

            vertices.push_back(
                {position, glm::normalize(normal), Color::white.getColor(), tex_coord});
        }
    }

    // Индексы
    for (int32_t y = 0; y < ring_count; ++y) {
        for (int32_t x = 0; x < segments; ++x) {
            int32_t i0 = y * (segments + 1) + x;
            int32_t i1 = i0 + 1;
            int32_t i2 = i0 + (segments + 1);
            int32_t i3 = i2 + 1;

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i2);
        }
    }

    Mesh::create(vertices, indices, createShared<Material>());
}

} // namespace ae
