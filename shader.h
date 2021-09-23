#pragma once

#include <string>

class Mat4;
class Vec3;

struct Shader {
    unsigned int program;
};

Shader compile(const std::string &vertex, const std::string &fragment);

void set_matrix4(const Shader &shader, const std::string &name, const Mat4 &matrix);
void set_vec3(const Shader &shader, const std::string &name, const Vec3 &vec);
void set_int(const Shader &shader, const std::string &name, int value);
void set_float(const Shader &shader, const std::string &name, float value);

// RAII to disable shader at end of scope
class UseShader {
  public:
    explicit UseShader(unsigned int program);
    ~UseShader();
};