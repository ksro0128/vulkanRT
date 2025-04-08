#version 460

layout(location = 0) in vec3 inPosition;

struct ObjectInstance {
    int modelMatrixIndex;
    int materialIndex;
};

layout(std430, set = 0, binding = 0) readonly buffer ObjectInstances {
    ObjectInstance instances[];
};

layout(set = 1, binding = 0) readonly buffer ModelBuffer {
    mat4 modelMatrices[];
};

layout(push_constant) uniform PushConstants {
    mat4 lightViewProj;
} pc;

void main() {
    uint instanceIndex = gl_InstanceIndex;
    int modelIdx = instances[instanceIndex].modelMatrixIndex;

    gl_Position = pc.lightViewProj * modelMatrices[modelIdx] * vec4(inPosition, 1.0);
}
