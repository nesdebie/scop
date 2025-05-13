#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragWorldPos;

layout(location = 4) out float vSizeApprox;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    fragTexCoord = inTexCoord;
    fragWorldPos = worldPos.xyz;

    // crude estimate of size: distance from origin
    vSizeApprox = length(worldPos.xyz);
}
