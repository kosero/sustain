#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
out vec3 vNormal;

void main() {
  gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
  vNormal = mat3(uView * uModel) * aNormal;
}
