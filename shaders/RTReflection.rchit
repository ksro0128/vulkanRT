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

    // vec3 baseColor = (mat.albedoTexIndex == -1)
    //     ? mat.baseColor.rgb
    //     : texture(textures[nonuniformEXT(mat.albedoTexIndex)], uv).rgb;

    // payload.color = baseColor;




	vec3 normal;
	if (mat.normalTexIndex != -1) {
		vec3 tangent = normalize(v0.tangent);
		vec3 bitangent = normalize(cross(tangent, v0.normal));
		mat3 TBN = mat3(tangent, bitangent, v0.normal);

		vec3 sampledNormal = texture(textures[nonuniformEXT(mat.normalTexIndex)], uv).rgb;
		sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
		normal = normalize(TBN * sampledNormal);
	} else {
		normal = normalize(cross(v1.pos - v0.pos, v2.pos - v0.pos));
	}
	vec3 albedo = (mat.albedoTexIndex == -1) ? mat.baseColor.rgb : texture(textures[nonuniformEXT(mat.albedoTexIndex)], uv).rgb;
	float ao = (mat.aoTexIndex == -1) ? mat.ao : texture(textures[nonuniformEXT(mat.aoTexIndex)], uv).r;
	float roughness = (mat.roughnessTexIndex == -1) ? mat.roughness : texture(textures[nonuniformEXT(mat.roughnessTexIndex)], uv).g;
	float metallic = (mat.metallicTexIndex == -1) ? mat.metallic : texture(textures[nonuniformEXT(mat.metallicTexIndex)], uv).b;
	vec3 N = normal;
	vec3 worldPos = gl_WorldRayOriginEXT + gl_RayTmaxEXT * gl_WorldRayDirectionEXT;
	vec3 V = normalize(-gl_WorldRayDirectionEXT);
	float attenuation = 1.0;

	Light light = lightInfo.lights[0];
	vec3 L = normalize(-light.direction);

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

	vec3 ambient = lightInfo.ambientColor * albedo * ao;

	payload.color = ambient + (diffuse + specular) * radiance;
}
