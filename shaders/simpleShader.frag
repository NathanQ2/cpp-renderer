#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 o_Color;

void main() {
    o_Color = vec4(fragColor, 1.0);
}
