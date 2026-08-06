#version 460 core

uniform mat4 uInvViewProj;
uniform vec3 uCamPos;
uniform vec3 uSunDir;
in vec2 vUv;
out vec4 outColor;

void main()
{
  vec4 p = uInvViewProj * vec4(vUv, 1.0, 1.0);
  vec3 farWorld = p.xyz / p.w;
  vec3 dir = normalize(farWorld - uCamPos);
  vec3 sunDir = normalize(uSunDir);

  float h = clamp(dir.y, -1.0, 1.0);

  vec3 zenithColor = vec3(0.18, 0.34, 0.72);
  vec3 horizonColor = vec3(0.98, 0.76, 0.55);
  vec3 groundColor = vec3(0.14, 0.14, 0.18);

  float up = pow(1.0 - clamp(h, 0.0, 1.0), 2.4);
  vec3 sky = mix(zenithColor, horizonColor, up);

  if (h < 0.0) {
    float g = pow(clamp(-h, 0.0, 1.0), 0.55);
    sky = mix(sky, groundColor, g);
  }

  // warm haze hugging the horizon
  float haze = pow(1.0 - clamp(abs(h) / 0.22, 0.0, 1.0), 2.0);
  sky += vec3(0.30, 0.18, 0.10) * haze * 0.35;

  // big stylized sun with layered glow
  float sunAngle = acos(clamp(dot(dir, sunDir), -1.0, 1.0));
  float disk = smoothstep(0.030, 0.020, sunAngle);
  float halo = pow(1.0 - clamp(sunAngle / 0.16, 0.0, 1.0), 2.5);
  float bloom = pow(1.0 - clamp(sunAngle / 0.5, 0.0, 1.0), 3.0);
  vec3 sunColor = vec3(1.0, 0.84, 0.60);
  sky += sunColor * (disk * 2.0 + halo * 0.65 + bloom * 0.16);

  outColor = vec4(sky, 1.0);
}
