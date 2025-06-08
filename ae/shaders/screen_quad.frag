#version 330 core

in VS_OUT {
    vec2 texCoords;
    vec4 color;
} fs_in;

uniform float u_time;
uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{
    fragColor = fs_in.color * texture(u_texture, fs_in.texCoords);
}


