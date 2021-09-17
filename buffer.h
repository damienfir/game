#pragma once

#include <GL/glew.h>
#include "shader.h"
#include "math.h"
#include "camera.h"


//struct Mesh {
//    std::vector<float> vertices;
//    std::vector<unsigned int> indices;
//    GLenum mode = GL_TRIANGLES;
//    Vec3 color;
//    bool has_normals;
//};

using Vertex = Vec3;

struct VertexNormal {
    Vec3 vertex;
    Vec3 normal;
};

void log(const VertexNormal& vn);

template<typename T>
struct Buffer {
    enum ObjectType {
        Axis,
        Cube
    };

    unsigned int VAO;
    unsigned int VBO;
    Shader shader;
    std::vector<T> elements;
    Vec3 color;
    Matrix transform;
    ObjectType type;
};

template<typename T>
void draw(const Buffer<T> &object, const Camera &camera);

Buffer<VertexNormal> make_surface(int rows, int cols);

Buffer<Vertex> make_axis(int ax);

Buffer<VertexNormal> make_rectangle(float width, float height, float depth);

Buffer<VertexNormal> make_cube(float size);