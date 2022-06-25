#include "Renderer.h"

#include <iostream>
#include "../includes/VertexBufferLayout.h"
#include <filesystem>

extern std::filesystem::path g_WorkDir;
extern float aspectRatio;

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
    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LEQUAL));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    m_VBO = std::make_unique<VertexBuffer>();
    m_VAO = std::make_unique<VertexArray>();
    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->PushAttrib<glm::vec2>(1);
    m_CrossShader = std::make_unique<Shader>(
        (g_WorkDir / "res/shaders/CrossShader.glsl").string());

    m_Cross[0][0]= -0.02f;
    m_Cross[0][1]= 0.f;
    m_Cross[1][0]= 0.02f;
    m_Cross[1][1]= 0.f;
    m_Cross[2][0]= 0.f;
    m_Cross[2][1]= -0.02f;
    m_Cross[3][0]= 0.f;
    m_Cross[3][1]= 0.02f;
}

void Renderer::Clear() const {
    GLCall(glClearColor(0.49f, 0.62f, 0.95f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const Shader& shader, int elements) const {
    shader.Bind();
    va.Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0, elements));
}

void Renderer::DrawCross() {
    GLCall(glDisable(GL_DEPTH_TEST));
    m_VBO->LoadData(m_Cross, sizeof(m_Cross));
    m_VAO->AddBufferf(*m_VBO, *m_Layout);
    m_CrossShader->Bind();
    m_CrossShader->SetUniform1f("u_aspect", aspectRatio);
    GLCall(glDrawArrays(GL_LINES, 0, 4));
    GLCall(glEnable(GL_DEPTH_TEST));
}
