#version 330 core

layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal_;
layout (location = 2) in int face_index_;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 pos;
flat out int face_index;

void main(void) {
    vec4 coord_model = model * vec4(coord, 1.0);
    gl_Position = projection * view * coord_model;
    pos = vec3(coord_model);
    normal = mat3(transpose(inverse(model))) * normal_;
    face_index = face_index_;
}