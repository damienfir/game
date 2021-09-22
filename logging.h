#pragma once

#include <iostream>
#include <vector>

template <typename T>
void log(T msg) {
    std::cout << msg << std::endl;
}

template <typename T>
void log(const std::vector<T>& v) {
    for (int i = 0; i < v.size(); ++i) {
        log(v[i]);
    }
}

class Vec3;

std::string string(const Vec3 &v);