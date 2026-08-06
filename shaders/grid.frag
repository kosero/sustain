#version 460 core

uniform mat4 uInvViewProj;
uniform vec3 uCamPos;
in vec2 vUv;
out vec4 outColor;

void main() {
  vec4 p = uInvViewProj * vec4(vUv, 1.0, 1.0);
  vec3 farWorld = p.xyz / p.w;
  vec3 rayDir = normalize(farWorld - uCamPos);

  float t = -uCamPos.y / rayDir.y;
  if (t <= 0.0) {
    discard;
  }
  vec3 worldPos = uCamPos + rayDir * t;

  float dist = length(worldPos - uCamPos);
  float fade = exp(-dist * 0.02);

  vec2 pos = worldPos.xz;
  vec2 grid = abs(fract(pos - 0.5) - 0.5) / fwidth(pos);
  float line = min(grid.x, grid.y);
  float gridIntensity = 1.0 - min(line, 1.0);

  float axisX = 1.0 - min(abs(pos.x) / fwidth(pos.x), 1.0);
  float axisZ = 1.0 - min(abs(pos.y) / fwidth(pos.y), 1.0);

  vec3 col = vec3(0.28, 0.30, 0.36) * gridIntensity;
  col += vec3(0.75, 0.22, 0.22) * axisX * 0.7;
  col += vec3(0.20, 0.42, 0.85) * axisZ * 0.7;
  col *= fade;

  outColor = vec4(col, 1.0);
}
