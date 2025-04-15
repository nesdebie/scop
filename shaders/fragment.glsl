#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;
uniform bool useTexture;
uniform sampler2D texture1;

out vec4 FragColor;

void main() {
    float diff = max(dot(normalize(Normal), -lightDir), 0.0);
    vec3 baseColor = vec3(0.7, 0.7, 0.7) * diff;

    FragColor = vec4(baseColor, 1.0);
}
