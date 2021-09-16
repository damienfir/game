#pragma once

#include <filesystem>
#include "math.h"

struct Shader {
    unsigned int program;
};

Shader compile(const std::filesystem::path &vertex, const std::filesystem::path &fragment);

void use(const Shader &shader);

void set_matrix4(const Shader &shader, const std::string &name, const Matrix &matrix);

void set_vec3(const Shader &shader, const std::string &name, const Vec3 &vec);