#version 430 core

layout (location = 0) in vec3 v_position;  // Позиция вершины
layout (location = 1) in vec3 v_normal;    // Нормаль вершины
layout (location = 2) in vec4 v_color;     // Цвет вершины
layout (location = 3) in vec2 v_texCoords; // Текстурные координаты
layout (location = 4) in ivec4 v_boneIds;  // Идентификаторы костей
layout (location = 5) in vec4 v_weights;   // Веса костей

uniform mat4 u_projMat;
uniform mat4 u_viewMat;
uniform mat4 u_model;

// Skinning
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_finalBonesMats[MAX_BONES]; // Финальные матрицы костей
uniform int u_skeleton;

out VS_OUT {
    vec3 fragPos;  // Позиция фрагмента
    vec3 normal;   // Нормаль фрагмента
    vec2 texCoords;
    vec4 color;
} vs_out;

void main() {
    vs_out.color = v_color;
    vs_out.texCoords = v_texCoords;

    if (u_skeleton > 0) {  // Если используется скиннинг
        vec4 totalPosition = vec4(0.0f);
        vec3 totalNormal = vec3(0.0f); // Для нормалей

        // Применение влияния костей на позицию и нормаль
        for(int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if(v_boneIds[i] == -1) continue;  // Пропускаем если кость не существует

            if(v_boneIds[i] >= MAX_BONES) {
                totalPosition = vec4(v_position, 1.0f);
                break;
            }

            // Матрица костей для позиции
            mat4 boneMat = u_finalBonesMats[v_boneIds[i]];

            // Трансформация позиции с учетом веса
            vec4 localPosition = boneMat * vec4(v_position, 1.0f);
            totalPosition += localPosition * v_weights[i];

            // Трансформация нормали
            mat3 boneMat3 = mat3(boneMat); // Оставляем только матрицу поворота (3x3)
            vec3 localNormal = boneMat3 * v_normal; // Применяем трансформацию к нормали
            totalNormal += localNormal * v_weights[i]; // Скаливаем нормаль с учетом веса
        }

        // Обновление final позиции и нормали
        vs_out.normal = normalize(totalNormal); // Нормализуем нормаль
        vs_out.fragPos = vec3(u_model * totalPosition); // Позиция во всем мире
        gl_Position = u_projMat * u_viewMat * (u_model * totalPosition);

    } else {  // Если без скиннинга
        vs_out.normal = mat3(transpose(inverse(u_model))) * v_normal;  // Трансформация нормали
        vs_out.fragPos = vec3(u_model * vec4(v_position, 1.0f));  // Позиция в мировых координатах
        gl_Position = u_projMat * u_viewMat * (u_model * vec4(v_position, 1.0f));
    }
}
