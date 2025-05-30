#version 450

layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 cameraPos;
    float _pad0;

    vec4 lightPositions[8];
    vec4 lightIntensities[8];

    int  numLights;
    int  isLightOff;
    int  _pad1;
    int  _pad2;
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
    int   illumModel;
    int   useTexture;
} material;

layout(location = 0) in vec3  fragNormal;
layout(location = 1) in vec2  fragTexCoord;
layout(location = 2) in vec3  fragWorldPos;
layout(location = 0) out vec4 outColor;

void main() {
    vec3 baseColor = (material.useTexture == 1)
        ? texture(texSampler, fragTexCoord).rgb
        : material.diffuse;

    // toggle unlit
    if (ubo.isLightOff == -1) {
        outColor = vec4(baseColor, material.dissolve);
        return;
    }

    vec3 N = normalize(fragNormal);
    vec3 V = normalize(ubo.cameraPos - fragWorldPos);

    // start with ambient + emissive
    vec3 color = material.ambient + material.emissive;

    float blurFactor = 0.01;
    // loop over all lights
    for (int i = 0; i < ubo.numLights; ++i) {
        vec3 Lpos = ubo.lightPositions[i].xyz;
        float I  = ubo.lightIntensities[i].x;

        vec3 L    = normalize(Lpos - fragWorldPos);
        float d   = length(Lpos - fragWorldPos);
        float att = 1.0 / (d + 0.032 * d*d);

        float diff = max(dot(N, L), 0.0);
        vec3  R    = reflect(-L, N);
        float spec = pow(max(dot(V, R), 0.0),
                         material.specularExponent * (1 - blurFactor));

        color += (diff * baseColor + spec * material.specular)
               * att * I;
    }

    color = clamp(color, 0.0, 1.0);
    outColor = vec4(color, material.dissolve);
}
