#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform Material {
    vec3 color;
    int useTexture;
} material;

void main() {
    if (material.useTexture == 1)
        outColor = texture(texSampler, fragTexCoord);
    else
        outColor = vec4(material.color, 1.0);
}
