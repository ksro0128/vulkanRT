#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) rayPayloadInEXT vec4 payload;

struct Material {
    vec4 baseColor;
    vec3 emissiveFactor;
    float roughness;
    float metallic;
    float ao;
    int albedoTexIndex;
    int normalTexIndex;
    int metallicTexIndex;
    int roughnessTexIndex;
    int aoTexIndex;
    int emissiveTexIndex;
};

layout(std430, set = 2, binding = 1) readonly buffer MaterialBuffer {
    Material materials[];
};

layout(set = 2, binding = 2) uniform sampler2D textures[];

void main() {
    int materialIndex = gl_InstanceCustomIndexEXT;
    int albedoTexIndex = materials[materialIndex].albedoTexIndex;
    if (albedoTexIndex == -1) {
        payload = materials[materialIndex].baseColor;
        return;
    }
    else {
        vec4 albedoColor = texture(textures[albedoTexIndex], vec2(0.5, 0.5));
        payload = albedoColor;
        return;
    }
}
