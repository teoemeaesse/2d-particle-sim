#version 330 core

uniform vec2 window;

layout(location = 0) in vec4 position;

out vec2 velocity;

void main() {
    gl_Position = vec4(position.x / (window.x / 2) - 0.5f, position.y / (window.y / 2) - 0.5f, 1, 1);
    velocity = position.zw;
}