#version 460 core

uniform sampler2D uTexture;
in vec2 vUv;
out vec4 outColor;

void main() {
  outColor = texture(uTexture, vUv);
}
