#version 330 core
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec2 v_texCoords;

uniform mat4 u_projMat;
uniform mat4 u_model;

out VS_OUT {
    vec2 texCoords;
    vec4 color;
} vs_out;

void main()
{
    vs_out.texCoords = v_texCoords;
    vs_out.color = v_color;
    gl_Position = u_projMat * u_model * vec4(v_position, 1.0); // z учитывается!
}
