#version 430 core

#include "shaders/material.inc"
#include "shaders/light.inc"

// From vert shader
in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 color;
} fs_in;

uniform vec3 u_viewPos;
uniform Material u_material;
uniform DirectLight m_directLight;
uniform int u_enableLight;
uniform int u_lightsCount;
layout(std430, binding = 1) buffer LightsBuffer {
    Light u_lights[];
};

out vec4 fragColor;

void main(){
    if (texture(u_material.diffuse_texture, fs_in.texCoords).a < 0.005) {
        fragColor = vec4(0.0);
        return;
    }

    if (u_enableLight > 0) {
        vec3 normal = normalize(fs_in.normal);
        vec3 viewDir = normalize(u_viewPos - fs_in.fragPos);

        vec4 light = calcDirectLight(m_directLight, normal, fs_in.fragPos, viewDir, u_material, fs_in.texCoords);
        for (int i = 0; i < u_lightsCount; ++i)
            light += calcLight(u_lights[i], normal, fs_in.fragPos, viewDir, u_material, fs_in.texCoords);

        fragColor = light * fs_in.color;
    } else {
        fragColor = texture(u_material.diffuse_texture, fs_in.texCoords) * fs_in.color;
    }
}
