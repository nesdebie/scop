#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraPos;

    vec3 lightPos;
    float lightIntensity;

    vec3 lightPos2;
    float lightIntensity2;
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
float blurFactor = 0.9;
// existing normal/view/light1
vec3 normal    = normalize(fragNormal);
vec3 viewDir   = normalize(ubo.cameraPos - fragWorldPos);
vec3 lightDir1 = normalize(ubo.lightPos   - fragWorldPos);
float d1       = length(ubo.lightPos       - fragWorldPos);
float att1     = 1.0/(d1 + 0.032*d1*d1);
float diff1    = max(dot(normal, lightDir1), 0.0);
vec3 reflect1 = reflect(-lightDir1, normal);
float spec1    = pow(max(dot(viewDir, reflect1), 0.0),
                     material.specularExponent * (1 - blurFactor));
vec3 col1      = (diff1 * baseColor + spec1 * material.specular)
                 * att1 * ubo.lightIntensity;

// new light2
vec3 lightDir2 = normalize(ubo.lightPos2  - fragWorldPos);
float d2       = length(ubo.lightPos2      - fragWorldPos);
float att2     = 1.0/(d2 + 0.032*d2*d2);
float diff2    = max(dot(normal, lightDir2), 0.0);
vec3 reflect2 = reflect(-lightDir2, normal);
float spec2    = pow(max(dot(viewDir, reflect2), 0.0),
                     material.specularExponent * (1 - blurFactor));
vec3 col2      = (diff2 * baseColor + spec2 * material.specular)
                 * att2 * ubo.lightIntensity2;

// ambient + both + emissive
vec3 ambient = material.ambient;
vec3 emissive = material.emissive;
vec3 finalColor = ambient + col1 + col2 + emissive;
finalColor = clamp(finalColor, 0.0, 1.0);
outColor = vec4(finalColor, material.dissolve);

}
