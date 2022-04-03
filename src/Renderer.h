#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>

#include "IndexBuffer.h"
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

static constexpr int CHUNK_SIZE = 16;
static constexpr int CHUNK_VOLUME = CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE;

struct Vertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoords;
    float TexID;
};

class Renderer {
private:
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::vector<unsigned int> indecies;
public:
    Renderer();
    void Clear() const;
    void Draw(const VertexArray& va, unsigned int count, const Shader& shader);
    void Draw(const VertexArray& va, const IndexBuffer& ib,
              const Shader& shader) const;
};
