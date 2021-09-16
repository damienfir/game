#version 330 core

layout (location = 0) in vec3 coord3d;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void) {
  gl_Position = projection * view * model * vec4(coord3d, 1.0);
}