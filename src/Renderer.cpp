#include "Renderer.h"

#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer() {
    int offset{};
    for (int i = 0; i < CHUNK_VOLUME*36; i += 6) {
   
        indecies.push_back(0+offset);
        indecies.push_back(1+offset);
        indecies.push_back(2+offset);
        indecies.push_back(2+offset);
        indecies.push_back(3+offset);
        indecies.push_back(0+offset);
        offset += 4;
    }
}

void Renderer::Clear() const {
    GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, unsigned int count, const Shader& shader) {
    m_IndexBuffer = std::make_unique<IndexBuffer>(indecies.data(), count);
    shader.Bind();
    va.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
