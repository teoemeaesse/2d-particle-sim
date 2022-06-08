#version 430 core

layout(location = 0) out vec4 color;

in vec2 velocity;

void main() {
    float abs_velocity = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float min_v = 0.0f, max_v = 2.5f;
    float norm_velocity = (abs_velocity - min_v) / (max_v - min_v);
    color = vec4(norm_velocity * 0.75, 0.8 - norm_velocity, 0.5, 0.4);
}