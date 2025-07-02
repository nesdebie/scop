#version 450

layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 cameraPos;
    int lightMode;

    vec3 objectCenter;
    float spotCosCutoff;

    vec4 lightPositions[8];
    vec4 lightIntensities[8];

    int  numLights;
    int  isLightOff;
    int  tex;
    int  _pad1;
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

// Hash a uint to a grayscale color (same value for R, G, B)
vec3 hashColor(uint id) {
    // Xorshift-style hash
    id = (id ^ 61u) ^ (id >> 16);
    id *= 9u;
    id = id ^ (id >> 4);
    id *= 0x27d4eb2du;
    id = id ^ (id >> 15);

    // Map to grayscale: extract 8 bits and normalize
    float gray = float(id & 0xFFu) / 255.0;
    return vec3(gray);
}


void main() {
    vec3 baseColor = (material.useTexture == 1)
        ? texture(texSampler, fragTexCoord).rgb
        : material.diffuse;

    // Texture disabled
    if (ubo.tex == 0) {
        outColor = vec4(hashColor(gl_PrimitiveID), material.dissolve);
        return;
    }

    // Light effects disabled
    if (ubo.isLightOff == 0) {
            outColor = vec4(baseColor, material.dissolve);
        return;
    }

    vec3 N = normalize(fragNormal);
    vec3 V = normalize(ubo.cameraPos - fragWorldPos);

    vec3 color = material.ambient + material.emissive;
    vec3 lightestSpot = vec3(0.0);

    for (int i = 0; i < ubo.numLights; ++i) {
        if (ubo.lightMode > 0 && ubo.lightMode != i + 1)
            continue;

        vec3  Lpos = ubo.lightPositions[i].xyz;
        float I    = ubo.lightIntensities[i].x;

        vec3 spotAxis = normalize(ubo.objectCenter - Lpos);
        vec3 fragDir  = normalize(fragWorldPos - Lpos);
        float cosA    = dot(spotAxis, fragDir);
        if (cosA < ubo.spotCosCutoff) continue;

        float d   = length(Lpos - fragWorldPos);
        float att = 1.0 / (1.0 + 0.1 * d);

        float diff = max(dot(N, normalize(Lpos - fragWorldPos)), 0.0);
        vec3  R    = reflect(-normalize(Lpos - fragWorldPos), N);
        float spec = pow(max(dot(V, R), 0.0), material.specularExponent);

        float edge = smoothstep(ubo.spotCosCutoff, 1.0, cosA);

        vec3 tempcol = (diff * baseColor + spec * material.specular)
                     * att * I * edge;

        lightestSpot = max(lightestSpot, tempcol);
    }

    color += lightestSpot;
    color = clamp(color, 0.0, 1.0);
    outColor = vec4(color, material.dissolve);
}
