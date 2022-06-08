#version 330 core

uniform vec2 window;
uniform vec2 camera;
uniform float zoom;

layout(location = 0) in vec4 position;

out vec2 velocity;

void main() {
    gl_Position = vec4(((position.x - camera.x) / window.x - 0.5f) * zoom , ((position.y - camera.y) / window.y - 0.5f) * zoom , 1, 1);
    velocity = position.zw;
}