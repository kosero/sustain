#version 460 core

in vec3 vNormal;
uniform vec4 uColor;
uniform vec3 uLightDir;
out vec4 outColor;

void main() {
  float ndotl = max(dot(normalize(vNormal), uLightDir), 0.0);
  vec3 lit = uColor.rgb * (0.55 + 0.45 * ndotl);
  outColor = vec4(lit, uColor.a);
}
