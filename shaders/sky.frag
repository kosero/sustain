#version 460 core

uniform mat4 uInvViewProj;
uniform vec3 uCamPos;
uniform vec3 uSunDir;
in vec2 vUv;
out vec4 outColor;

void main() {
  vec4 p = uInvViewProj * vec4(vUv, 1.0, 1.0);
  vec3 farWorld = p.xyz / p.w;
  vec3 dir = normalize(farWorld - uCamPos);

  vec3 topColor = vec3(0.18, 0.34, 0.72);
  vec3 horizonColor = vec3(0.60, 0.68, 0.76);
  vec3 groundColor = vec3(0.28, 0.26, 0.28);
  vec3 sunColor = vec3(1.0, 0.92, 0.74);

  vec3 sunDir = normalize(uSunDir);
  float sunDot = clamp(dot(dir, sunDir), -1.0, 1.0);
  float sunAngle = acos(sunDot);

  float horizon = pow(1.0 - clamp(dir.y, 0.0, 1.0), 3.0);
  vec3 sky = mix(topColor, horizonColor, horizon);

  if (dir.y < 0.0) {
    float g = pow(clamp(-dir.y, 0.0, 1.0), 0.45);
    sky = mix(sky, groundColor, g);
  }

  float disk = 1.0 - smoothstep(0.012, 0.02, sunAngle);
  float glow = pow(1.0 - clamp(sunAngle / 0.6, 0.0, 1.0), 3.0);
  sky += sunColor * (disk * 1.6 + glow * 0.35);

  outColor = vec4(sky, 1.0);
}
