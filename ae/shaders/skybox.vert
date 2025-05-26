#version 430 core

layout (location = 0) in vec3 v_position;  // Позиция вершины

uniform mat4 u_projMat;
uniform mat4 u_viewMat;

out VS_OUT {
    vec3 texCoords;
} vs_out;

void main(void)
{
    vs_out.texCoords = v_position;
    vec4 pos = u_projMat * u_viewMat * vec4(v_position, 1.0);
    gl_Position = pos.xyww;
}
