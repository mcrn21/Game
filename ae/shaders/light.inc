struct DirectLight {
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Light {
    vec4 position;      // .w = type (0 = point, 1 = spot)
    vec4 direction;     // .xyz = direction, .w = cos(innerCutOff)
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 attenuation;   // x = constant, y = linear, z = quadratic, w = cos(outerCutOff)
};


vec4 calcDirectLight(DirectLight light,
                     vec3 normal,
                     vec3 fragPos,
                     vec3 viewDir,
                     Material material,
                     vec2 texCoords) {
    vec3 lightDir = normalize(-light.direction);

    // Ambient
    vec4 ambient = light.ambient * texture(material.diffuse_texture, texCoords);
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = light.diffuse * diff * texture(material.diffuse_texture, texCoords);
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = light.specular * spec * texture(material.specular_texture, texCoords);

    return (ambient + diffuse + specular) * material.color;
}

vec4 calcLight(Light light,
               vec3 normal,
               vec3 fragPos,
               vec3 viewDir,
               Material material,
               vec2 texCoords) {
    vec3 lightDir =  normalize(light.position.xyz - fragPos);
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.attenuation.x +
                               light.attenuation.y * distance +
                               light.attenuation.z * (distance * distance));
    float intensity = 1.0;

    if (int(light.position.w) == 1) {
        float theta = dot(lightDir, normalize(-light.direction.xyz));
        float epsilon = light.direction.w - light.attenuation.w;
        intensity = clamp((theta - light.attenuation.w) / epsilon, 0.0, 1.0);
    }

    // Ambient
    vec4 ambient = light.ambient * texture(material.diffuse_texture, texCoords);
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = light.diffuse * diff * texture(material.diffuse_texture, texCoords);
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = light.specular * spec * texture(material.specular_texture, texCoords);

    // Итоговый свет
    return (ambient + diffuse + specular) * material.color * attenuation * intensity;
}
