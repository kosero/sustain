#version 460 core

uniform mat4 uInvViewProj;
uniform mat4 uViewProj;
uniform vec3 uCamPos;
in vec2 vUv;
out vec4 outColor;

const float LINE_WIDTH = 2.0;

float ray_line_distance(vec3 ro, vec3 rd, vec3 lp, vec3 ld, out float sc,
                        out float t)
{
  vec3 w0 = ro - lp;
  float b = dot(rd, ld);
  float d = dot(rd, w0);
  float e = dot(ld, w0);
  float denom = 1.0 - b * b;

  if (denom > 1e-6) {
    sc = (b * e - d) / denom;
    t = (e - b * d) / denom;
  } else {
    sc = 0.0;
    t = e;
  }
  return length((lp + t * ld) - (ro + sc * rd));
}

void main() {
  vec4 p = uInvViewProj * vec4(vUv, 1.0, 1.0);
  vec3 farWorld = p.xyz / p.w;
  vec3 rd = normalize(farWorld - uCamPos);

  vec3 col = vec3(0.0);
  float intensity = 0.0;
  float sc, t;
  vec3 hitPoint = farWorld;

  float dx = ray_line_distance(uCamPos, rd, vec3(0.0), vec3(1.0, 0.0, 0.0),
                               sc, t);
  float wx = 1.0 - min(abs(dx) / (fwidth(dx) * LINE_WIDTH), 1.0);
  vec3 cx = mix(vec3(0.55, 0.34, 0.32), vec3(0.95, 0.22, 0.18),
                step(0.0, t));
  col += cx * wx;
  if (wx > intensity) {
    intensity = wx;
    hitPoint = uCamPos + sc * rd;
  }

  float dy = ray_line_distance(uCamPos, rd, vec3(0.0), vec3(0.0, 1.0, 0.0),
                               sc, t);
  float wy = 1.0 - min(abs(dy) / (fwidth(dy) * LINE_WIDTH), 1.0);
  vec3 cy = mix(vec3(0.34, 0.55, 0.36), vec3(0.22, 0.88, 0.34),
                step(0.0, t));
  col += cy * wy;
  if (wy > intensity) {
    intensity = wy;
    hitPoint = uCamPos + sc * rd;
  }

  float dz = ray_line_distance(uCamPos, rd, vec3(0.0), vec3(0.0, 0.0, 1.0),
                               sc, t);
  float wz = 1.0 - min(abs(dz) / (fwidth(dz) * LINE_WIDTH), 1.0);
  vec3 cz = mix(vec3(0.32, 0.44, 0.60), vec3(0.22, 0.48, 0.98),
                step(0.0, t));
  col += cz * wz;
  if (wz > intensity) {
    intensity = wz;
    hitPoint = uCamPos + sc * rd;
  }

  if (intensity > 0.0) {
    vec4 clip = uViewProj * vec4(hitPoint, 1.0);
    gl_FragDepth = (clip.z / clip.w) * 0.5 + 0.5;
  }

  outColor = vec4(col, intensity);
}
