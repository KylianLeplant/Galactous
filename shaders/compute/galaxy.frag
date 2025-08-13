#version 430
precision mediump float;

in float v_brightness;

out vec4 color;

void main() {
    color = vec4(v_brightness / 5., 0., 0., 1.);
}