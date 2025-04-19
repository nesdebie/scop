#version 450
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform Blend {
    float blend;
} ubo2;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = mix(vec4(fragColor,1.0), texColor, ubo2.blend);
}