#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

// Push Constants para enviar a matriz MVP (Model-View-Projection)
layout(push_constant) uniform PushConstants {
    mat4 renderMatrix;
} push;

void main() {
    gl_Position = push.renderMatrix * vec4(inPosition, 1.0);
    fragColor = inColor;
}