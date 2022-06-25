#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer() {
    GLCall(glGenBuffers(1, &m_RenderID));
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
    GLCall(glGenBuffers(1, &m_RenderID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RenderID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
    GLCall(glDeleteBuffers(1, &m_RenderID));
}

void VertexBuffer::LoadData(const void* data, unsigned int size) {
    Bind();
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::UnloadData(){
    Bind();
    GLCall(glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW));
}

void VertexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RenderID));
}

void VertexBuffer::Unbind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
