#pragma once

#include <string>

class Mat4;
class Vec3;

struct Shader {
    unsigned int program;
};

Shader compile(const std::string &vertex, const std::string &fragment);

void use(const Shader &shader);

void set_matrix4(const Shader &shader, const std::string &name, const Mat4 &matrix);

void set_vec3(const Shader &shader, const std::string &name, const Vec3 &vec);