#version 330 core

in VS_OUT {
    vec2 texCoords;
    vec4 color;
} fs_in;

uniform sampler2D u_texture;
uniform float u_time; // время в секундах

out vec4 fragColor;

// Простая генерация шума по координатам
float random(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float offset(float blocks, vec2 uv) {
        return random(vec2(u_time, floor(uv.y * blocks)));
}

void main()
{
    vec2 uv = fs_in.texCoords;

    // 📺 Сканлайны — затенение строк
    float scanline = 0.95 + 0.02 * sin(uv.y * 800.0);

    // Основной цвет с шумом
    vec4 texColor = texture(u_texture, uv);

    texColor.r = texture(u_texture, uv + vec2(offset(10.0, uv) * 0.0005, 0.0)).r;
    texColor.g = texture(u_texture, uv + vec2(offset(10.0, uv) * 0.0005, 0.0)).g;
    texColor.b = texture(u_texture, uv + vec2(offset(10.0, uv) * 0.0005, 0.0)).b;

    // 🌫 Добавим шум поверх
    float noise = (random(uv * u_time) - 0.5) * 0.05;
    texColor.rgb += noise;

    // Комбинируем всё: цвет * сканлайн * цвет вершины
    fragColor = fs_in.color * texColor * scanline;
}
