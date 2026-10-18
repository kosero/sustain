#version 460 core

layout (location = 0) in vec2 a_pos;

uniform vec2 u_offset;
uniform vec2 u_size;

void main()
{
    gl_Position = vec4(u_offset + a_pos * u_size, 0.0, 1.0);
}
