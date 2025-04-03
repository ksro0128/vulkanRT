#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;
layout(location = 3) out vec3 outTangent;

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 camPos;
} camera;

// Set 1 - Object Matrix
layout(set = 1, binding = 0) readonly buffer ModelBuffer {
    mat4 modelMatrices[];
};

// 여기서부터 고쳐봅시다
layout(push_constant) uniform PushConstants {
    uint objectIndex;
} pc;

void main() {
    mat4 model = modelMatrices[pc.objectIndex];

    vec4 worldPosition = model * vec4(inPosition, 1.0);
    outWorldPos = worldPosition.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    outNormal = normalize(normalMatrix * inNormal);
    outTangent = normalize(normalMatrix * inTangent);

    outTexCoord = inTexCoord;

    gl_Position = camera.proj * camera.view * worldPosition;
}
