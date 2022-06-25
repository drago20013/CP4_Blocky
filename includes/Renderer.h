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

namespace glm {
    typedef glm::tvec3<GLubyte> vec3b;
    typedef glm::tvec2<GLubyte> vec2b;
}

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

class Renderer {    
public:
    Renderer();
    void Clear() const;
    void Draw(const VertexArray& va, const Shader& shader, int elements) const;
    void DrawCross();

private:
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBufferLayout> m_Layout;
    std::unique_ptr<Shader> m_CrossShader;
    float m_Cross[4][2];
};
