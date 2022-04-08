#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Shader.h"
#include "VertexArray.h"

#ifdef _MSC_VER
#define ASSERT(x) \
    if (!(x)) __debugbreak();
#else
#define ASSERT(x) \
    if (!(x)) __builtin_trap();
#endif
#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

struct Vertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoords;
    float TexID;
    float DirectLight;
};

class Renderer {    
public:
    Renderer();
    void Clear() const;
    void Draw(const VertexArray& va, const Shader& shader, int elements) const;
};
