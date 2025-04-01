#version 450

layout(location = 0) in vec2 inPosition;

void main() {
    // Transforma a posição 2D para uma posição 4D em espaço clip
    gl_Position = vec4(inPosition, 0.0, 1.0);
}
