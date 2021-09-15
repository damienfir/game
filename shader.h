#pragma once

struct Shader {
    unsigned int program;
};

Shader compile(const char *vertex_source, const char *fragment_source);

void use(const Shader &shader);