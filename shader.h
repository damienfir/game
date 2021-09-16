#pragma once

#include <filesystem>

struct Shader {
    unsigned int program;
};

Shader compile(const std::filesystem::path &vertex, const std::filesystem::path &fragment);

void use(const Shader &shader);