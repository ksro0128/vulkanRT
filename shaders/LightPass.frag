#version 460

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    vec3 camPos;
} camera;

struct Light {
    int type;
    int shadowMapIndex;
    int castsShadow;
    float intensity;

    vec3 color;
    float range;

    vec3 position;
    float spotInnerAngle;

    vec3 direction;
    float spotOuterAngle;
};

layout(set = 0, binding = 1) readonly buffer LightBuffer {
    Light lights[64];
    vec3 ambientColor;
    int lightCount;
}lightInfo;

layout(set = 1, binding = 0) uniform sampler2D gPosition;
layout(set = 1, binding = 1) uniform sampler2D gNormal;
layout(set = 1, binding = 2) uniform sampler2D gAlbedo;
layout(set = 1, binding = 3) uniform sampler2D gPBR;


// Fresnel-Schlick Approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Normal Distribution Function (NDF)
float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (3.14159265359 * denominator * denominator);
}

// Geometry Function
float geometrySchlickGGX(float NdotV, float roughness) {
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return geometrySchlickGGX(NdotV, roughness) * geometrySchlickGGX(NdotL, roughness);
}

void main() {
    vec3 fragPos = texture(gPosition, fragTexCoord).xyz;
    vec3 normal = normalize(texture(gNormal, fragTexCoord).xyz);
    vec3 albedo = texture(gAlbedo, fragTexCoord).rgb;
    vec3 pbrParams = texture(gPBR, fragTexCoord).rgb;

    float ao = pbrParams.r;
    float roughness = pbrParams.g;
    float metallic = pbrParams.b;

    vec3 N = normal;
    vec3 V = normalize(camera.camPos - fragPos);

    vec3 finalColor = vec3(0.0);

    vec3 ambient =  lightInfo.ambientColor * albedo * ao;
    finalColor += ambient;

    for (uint i = 0; i < lightInfo.lightCount; i++) {
        vec3 L;
        float attenuation = 1.0;
        Light light = lightInfo.lights[i];

        if (light.type == 0) { // directional
            L = normalize(-light.direction);
            attenuation = 1.0;
        }
        else if (light.type == 1) { // point
            L = normalize(light.position - fragPos);
            float distance = length(light.position - fragPos);
            float constant = 1.0;
            float linear = 0.09;
            float quadratic = 0.032;
            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
            float rangeFactor = clamp(1.0 - distance / light.range, 0.0, 1.0);
            attenuation *= rangeFactor;
        }
        else { // spot
            L = normalize(light.position - fragPos);
            float distance = length(light.position - fragPos);
            float constant = 1.0;
            float linear = 0.09;
            float quadratic = 0.032;
            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
            float rangeFactor = clamp(1.0 - distance / light.range, 0.0, 1.0);
            attenuation *= rangeFactor;


            float theta = dot(L, normalize(-light.direction));
            float cosInner = cos(radians(light.spotInnerAngle));
            float cosOuter = cos(radians(light.spotOuterAngle));
            float epsilon = max(cosInner - cosOuter, 0.001);
            float intensity = clamp((theta - cosOuter) / epsilon, 0.0, 1.0);
            attenuation *= intensity;
        }

        vec3 H = normalize(V + L);
        vec3 F0 = mix(vec3(0.04), albedo, metallic);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float NDF = distributionGGX(N, H, roughness);
        float G = geometrySmith(N, V, L, roughness);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.01);
        vec3 diffuse = kD * albedo / 3.14159265359;
        vec3 radiance = light.intensity * light.color * attenuation * NdotL;

        finalColor += (diffuse + specular) * radiance;
    }
    outColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}
