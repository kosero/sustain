#version 460 core

in vec3 vNormal;
in vec3 vViewPos;
uniform vec4 uColor;
uniform vec3 uLightDir;
out vec4 outColor;

void main() {
  vec3 base = uColor.rgb;
  vec3 n = normalize(vNormal);
  vec3 l = normalize(uLightDir);
  vec3 v = normalize(-vViewPos);

  float ndotl = clamp(dot(n, l), 0.0, 1.0);
  float wrap = ndotl * 0.5 + 0.5;
  float diffuse = wrap * wrap;

  float mid = smoothstep(0.5, 0.66, diffuse);
  float sunlit = smoothstep(0.78, 0.92, diffuse);

  float fill = clamp(-ndotl * 0.5 + 0.5, 0.0, 1.0);
  fill *= fill;

  vec3 key = vec3(1.0, 0.92, 0.78);
  vec3 ambient = vec3(0.42, 0.45, 0.55);
  vec3 rim = vec3(1.0, 0.78, 0.5);

  vec3 col = base * (ambient * fill + key * (0.35 + 0.65 * mid));
  col += base * key * sunlit * 0.35;

  float edgeFactor = length(dFdx(vNormal)) + length(dFdy(vNormal));
  float fresnel = pow(1.0 - clamp(dot(n, v), 0.0, 1.0), 3.0);
  float curved = smoothstep(0.0, 0.06, edgeFactor);
  float softRim = curved * smoothstep(0.25, 0.6, fresnel);
  float hardRim = smoothstep(0.08, 0.3, edgeFactor);
  float rimStrength = max(softRim, hardRim) * (0.4 + 0.6 * sunlit);
  col += rim * rimStrength * 0.75;

  vec3 grey = vec3(dot(col, vec3(0.299, 0.587, 0.114)));
  col = mix(col, grey, sunlit * 0.12);

  outColor = vec4(col, uColor.a);
}
