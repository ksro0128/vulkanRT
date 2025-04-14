#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : require

struct Vertex {
    vec3 pos; float pad0;
    vec3 normal; float pad1;
    vec2 texCoord; vec2 pad2;
    vec3 tangent; float pad3;
};

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices  { uvec3 i[]; };

struct RayPayload {
    vec3 color;
    int bounce;
};

layout(location = 0) rayPayloadInEXT RayPayload payload;


layout(set = 1, binding = 1) uniform accelerationStructureEXT topLevelAS;

struct ObjectInstance {
    uint64_t vertexIndex;
    uint64_t indexIndex;
    int modelMatrixIndex;
    int materialIndex;
    int meshIndex;
    int pad;
};

layout(std430, set = 2, binding = 0) readonly buffer ObjectInstances {
    ObjectInstance instances[];
};

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

layout(std430, set = 3, binding = 0) readonly buffer ModelBuffer {
    mat4 modelMatrices[];
};

layout(std430, set = 3, binding = 1) readonly buffer MaterialBuffer {
    Material materials[];
};

layout(set = 3, binding = 2) uniform sampler2D textures[];


hitAttributeEXT vec2 attribs;

void main() {
    ObjectInstance inst = instances[gl_InstanceCustomIndexEXT];
    Vertices vertices = Vertices(inst.vertexIndex);
    Indices indices = Indices(inst.indexIndex);

    uvec3 idx = indices.i[gl_PrimitiveID];
    Vertex v0 = vertices.v[idx.x];
    Vertex v1 = vertices.v[idx.y];
    Vertex v2 = vertices.v[idx.z];

    float u = attribs.x;
    float v = attribs.y;
    float w = 1.0 - u - v;
    vec2 uv = v0.texCoord * w + v1.texCoord * u + v2.texCoord * v;

    Material mat = materials[inst.materialIndex];


    if (payload.bounce == 0) {

        mat4 model = modelMatrices[inst.modelMatrixIndex];
        vec3 localHitPos = v0.pos * w + v1.pos * u + v2.pos * v;
        vec3 hitPos = (model * vec4(localHitPos, 1.0)).xyz;

        vec3 localNormal = normalize(v0.normal * w + v1.normal * u + v2.normal * v);
        mat3 normalMatrix = transpose(inverse(mat3(model)));
        vec3 worldNormal = normalize(normalMatrix * localNormal);

        vec3 viewDir = normalize(hitPos - gl_WorldRayOriginEXT);
        vec3 reflectDir = normalize(reflect(viewDir, worldNormal));
        
        payload.bounce = 1;
        payload.color = vec3(0.0);

        traceRayEXT(topLevelAS,
            gl_RayFlagsOpaqueEXT,
            0xFF,
            0, 0, 0,
            hitPos + reflectDir * 0.05,
            0.001,
            reflectDir,
            10000.0,
            0);

        vec3 base = (mat.albedoTexIndex == -1)
            ? mat.baseColor.rgb
            : texture(textures[nonuniformEXT(mat.albedoTexIndex)], uv).rgb;

        payload.color = mix(base, payload.color, 0.2);
    }
    else {
        payload.color = (mat.albedoTexIndex == -1)
            ? mat.baseColor.rgb
            : texture(textures[nonuniformEXT(mat.albedoTexIndex)], uv).rgb;
    }
}
