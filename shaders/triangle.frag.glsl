#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform FallbackFlags {
    int useTexture;
};

void main() {
    if (useTexture == 1) {
        vec3 color = texture(texSampler, fragTexCoord).rgb; // ignore alpha
        outColor = vec4(color, 1.0);                         // force alpha = 1
    } else {
        float dx = length(dFdx(fragWorldPos));
        float dy = length(dFdy(fragWorldPos));
        float area = dx * dy;
        float grayscale = clamp(area * 20.0 + 0.5, 0.0, 1.0);
        outColor = vec4(vec3(grayscale), 1.0);
    }
}
