#include "Chunk.h"
#include <filesystem>
#include <cstring>

extern std::filesystem::path g_WorkDir;

Chunk::Chunk() : m_Elements(0), m_Changed(true), m_Renderer(), m_WhiteTexture(0xffffffff)
{
    memset(m_Blocks, 0, sizeof(m_Blocks)); // filling m_Blocks with 0s
    m_VBO = std::make_unique<VertexBuffer>();
    m_VAO = std::make_unique<VertexArray>();
    m_ChunkShader = std::make_unique<Shader>((g_WorkDir/"res/shaders/ChunkShader.glsl").string());
    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Vertecies = new glm::vec4byte[CHUNK_VOLUME * 6 * 6];
    m_Layout->PushAttrib<glm::vec4byte>(1);
}

Chunk::~Chunk()
{
    m_VBO.reset(nullptr);
    delete[] m_Vertecies;
}

uint8_t Chunk::Get(int x, int y, int z) const
{
    return m_Blocks[x][y][z];
}

void Chunk::Set(int x, int y, int z, uint8_t type)
{
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    m_Blocks[x][y][z] = type;
    m_Changed = true;
}

void Chunk::Update()
{
    m_Changed = false;
    int i = 0;
    uint8_t type{};
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                type = m_Blocks[x][y][z];

                if (!type) // if empty
                    continue;

                // View from negative x (right face)
                    m_Vertecies[i++] = glm::vec4byte(x, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z + 1, type);
             
                
               // View from positive x (left face)
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z, type);

                // View from negative z (front face)
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z, type);

                // View from positive z (back face)
                    m_Vertecies[i++] = glm::vec4byte(x, y, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z + 1, type);

                // View from negative y (bottom face)
                    m_Vertecies[i++] = glm::vec4byte(x, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y, z + 1, type);
                
                // View from positive y (up face)
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x + 1, y + 1, z + 1, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z, type);
                    m_Vertecies[i++] = glm::vec4byte(x, y + 1, z + 1, type);
            }
        }
    }

    m_Elements = i;
    m_VBO->LoadData(m_Vertecies, m_Elements * 4);
    m_VAO->AddBuffer(*m_VBO, *m_Layout);
}

void Chunk::Render(const glm::mat4& MVP)
{
    if (m_Changed)
        Update();

    m_ChunkShader->Bind();
    m_ChunkShader->SetUniformMat4f("u_MVP", MVP);
    m_Renderer.Draw(*m_VAO, *m_ChunkShader, m_Elements);
}
