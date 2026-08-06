#version 460 core

layout(location = 0) in vec2 aPosition;
out vec2 vUv;

void main() {
  gl_Position = vec4(aPosition, 1.0, 1.0);
  vUv = aPosition;
}
