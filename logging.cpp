#include "logging.h"

#include "maths.h"

std::string string(const Vec3 &v) {
    return std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z);
}

std::string string(const Mat4 &m) {
    std::string s;
    for (int i = 0; i < m.rows(); ++i) {
        for (int j = 0; j < m.cols(); ++j) {
            s += std::to_string(m.val(i, j)) + " ";
        }
        s += "\n";
    }
    return s;
}