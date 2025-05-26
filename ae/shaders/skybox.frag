#version 430 core

in VS_OUT {
    vec3 texCoords;
} fs_in;

uniform samplerCube skybox;

out vec4 fragColor;

void main() {
    fragColor = texture(skybox, fs_in.texCoords);
}
