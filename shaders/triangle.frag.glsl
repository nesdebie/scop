#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
    float lightIntensity;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform Material {
    vec3 diffuse;
    float specularExponent;
    vec3 ambient;
    float dissolve;
    vec3 specular;
    float refractionIndex;
    vec3 emissive;
    int illumModel;
    int useTexture;
} material;

void main() {
    vec3 baseColor = material.useTexture == 1
        ? texture(texSampler, fragTexCoord).rgb
        : material.diffuse;

    vec3 normal = normalize(fragNormal);

    vec3 viewDir = normalize(ubo.cameraPos - fragWorldPos);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 reflectDir = reflect(-lightDir, normal);

    float diffFactor = max(dot(normal, lightDir), 0.0); // 1
    float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.specularExponent); // 0

    vec3 ambient = material.ambient;
    vec3 diffuse = diffFactor * baseColor;
    vec3 specular = specFactor * material.specular;
    vec3 emissive = material.emissive;

    vec3 finalColor = (ambient + diffuse + specular + emissive) * ubo.lightIntensity;
    finalColor = clamp(finalColor, 0.0, 1.0);

    outColor = vec4(finalColor, material.dissolve);
}
