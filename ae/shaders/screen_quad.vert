#version 330 core
layout (location = 0) in vec2 v_position;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec2 v_texCoords;

out VS_OUT {
    vec2 texCoords;
    vec4 color;
} vs_out;

void main()
{
    vs_out.texCoords = v_texCoords;
    vs_out.color = v_color;
    gl_Position = vec4(v_position.x, v_position.y, 0.0, 1.0);
}
