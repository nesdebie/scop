#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(abs(fragNormal), 1.0); // simple color based on normal
}
