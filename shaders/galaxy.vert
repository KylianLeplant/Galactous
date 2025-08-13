#version 430
precision mediump float;

in vec3 position;
in vec3 velocity;

out float v_brightness;

uniform mat4 u_mvp;

void main() {
    gl_Position = u_mvp * vec4(position.xyz, 1.)
    v_brightness = length(vec3(velocity));
}