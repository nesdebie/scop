#version 450

layout(location = 0) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
    // Just visualize normal direction as color for now
    outColor = vec4(normalize(fragNormal) * 0.5 + 0.5, 1.0);
}
