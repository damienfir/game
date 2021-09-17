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


struct Buffer {
    enum ObjectType {
        Axis,
        Cube
    };

    unsigned int VAO;
    unsigned int VBO;
    Shader shader;
    std::vector<float> vertices;
    Vec3 color;
    Matrix transform;
    ObjectType type;
};

void draw(const Buffer &object, const Camera &camera);

//Buffer make_object(const Mesh &mesh, const std::string& vertex_shader, const std::string& fragment_shader);

//Mesh grid_mesh(int rows, int cols);

Buffer make_axis(int ax);

Buffer make_rectangle(float width, float height, float depth);

Buffer make_cube(float size);