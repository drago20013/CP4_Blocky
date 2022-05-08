#include "Chunk.h"

#include <cstring>
#include <filesystem>

#include "../../includes/world/WorldSegment.h"
#include "Block.h"

extern std::filesystem::path g_WorkDir;

Chunk::Chunk(int x, int z, WorldSegmnet* segment)
    : m_Segment(segment), m_PosX(x),m_PosZ(z), m_Elements(0), m_Changed(true), m_Renderer(), m_WhiteTexture(0xffffffff) {
    m_VBO = std::make_unique<VertexBuffer>();
    m_VAO = std::make_unique<VertexArray>();
    m_ChunkShader = std::make_unique<Shader>(
        (g_WorkDir / "res/shaders/ChunkShader.glsl").string());
    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Vertecies = new Vertex[CHUNK_VOLUME * 6 * 6];
    m_Layout->PushAttrib<glm::vec3b>(1);
    m_Layout->PushAttrib<GLubyte>(1);
}

Chunk::~Chunk() {
    m_VBO.reset(nullptr);
    delete[] m_Vertecies;
}

BlockType Chunk::Get(int x, int y, int z) const {
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    return m_Blocks[x][y][z].GetType();
}

void Chunk::Set(int x, int y, int z, BlockType type) {
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    m_Blocks[x][y][z].SetType(type);
    m_Changed = true;
}

void Chunk::SetActive(int x, int y, int z, bool activeLevel)
{
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    m_Blocks[x][y][z].SetActive(activeLevel);
}

bool Chunk::IsActive(int x, int y, int z)
{
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    return m_Blocks[x][y][z].IsActive();
}

void Chunk::Update() {
    m_Changed = false;
    int i = 0;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {

                if (!m_Blocks[x][y][z].IsActive()) {  // if empty
                    continue;
                }

                if (x == 0 && !m_Segment->IsActive((m_PosX*CHUNK_SIZE) + x -1,y, (m_PosZ * CHUNK_SIZE) +z) || x > 0 && !m_Blocks[x - 1][y][z].IsActive()) {
                    // View from negative x (right face)
                    m_Vertecies[i++] = { glm::vec3b(x, y, z)        , 8};
                    m_Vertecies[i++] = { glm::vec3b(x, y, z + 1)    , 8};
                    m_Vertecies[i++] = { glm::vec3b(x, y + 1, z)    , 8};
                    m_Vertecies[i++] = { glm::vec3b(x, y + 1, z)    , 8};
                    m_Vertecies[i++] = { glm::vec3b(x, y, z + 1)    , 8};
                    m_Vertecies[i++] = { glm::vec3b(x, y + 1, z + 1), 8};
                }

                if (x == CHUNK_SIZE - 1 && !m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x + 1, y, (m_PosZ * CHUNK_SIZE) + z) || x < CHUNK_SIZE-1 && !m_Blocks[x + 1][y][z].IsActive()) {
                    // View from positive x (left face)
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z + 1)    , 8};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z)        , 8};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z + 1), 8};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z + 1), 8};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z)        , 8};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z)    , 8};
                }

                if (z == 0 && !m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x, y, (m_PosZ * CHUNK_SIZE) + z - 1) || z > 0 && !m_Blocks[x][y][z - 1].IsActive()) {
                    // View from negative z (front face)
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z)    , 6};
                    m_Vertecies[i++] = {glm::vec3b(x, y, z)        , 6};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z), 6};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z), 6};
                    m_Vertecies[i++] = {glm::vec3b(x, y, z)        , 6};
                    m_Vertecies[i++] = {glm::vec3b(x, y + 1, z)    , 6};
                }

                if (z == CHUNK_SIZE - 1 && !m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x, y, (m_PosZ * CHUNK_SIZE) + z + 1) || z < CHUNK_SIZE-1 && !m_Blocks[x][y][z + 1].IsActive()) {
                    // View from positive z (back face)
                    m_Vertecies[i++] = {glm::vec3b(x, y, z + 1)        , 6};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z + 1)    , 6};
                    m_Vertecies[i++] = {glm::vec3b(x, y + 1, z + 1)    , 6};
                    m_Vertecies[i++] = {glm::vec3b(x, y + 1, z + 1)    , 6};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z + 1)    , 6};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z + 1), 6};
                }

                if (y == 0 || y > 0 && !m_Blocks[x][y - 1][z].IsActive()) {
                    // View from negative y (bottom face)
                    m_Vertecies[i++] = {glm::vec3b(x, y, z)        , 4};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z)    , 4};
                    m_Vertecies[i++] = {glm::vec3b(x, y, z + 1)    , 4};
                    m_Vertecies[i++] = {glm::vec3b(x, y, z + 1)    , 4};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z)    , 4};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y, z + 1), 4};
                }

                if (y == CHUNK_HEIGHT - 1 || y < CHUNK_HEIGHT-1 && !m_Blocks[x][y + 1][z].IsActive()) {
                    // View from positive y (up face)
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z)    , 10};
                    m_Vertecies[i++] = {glm::vec3b(x, y + 1, z)        , 10};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z + 1), 10};
                    m_Vertecies[i++] = {glm::vec3b(x + 1, y + 1, z + 1), 10};
                    m_Vertecies[i++] = {glm::vec3b(x, y + 1, z)        , 10};
                    m_Vertecies[i++] = {glm::vec3b(x, y + 1, z + 1)    , 10};
                }
            }
        }
    }

    m_Elements = i;
    m_VBO->LoadData(m_Vertecies, m_Elements * 4);
    m_VAO->AddBuffer(*m_VBO, *m_Layout);
}

void Chunk::Render(const glm::mat4& MVP) {
    if (m_Changed) Update();

    m_ChunkShader->Bind();
    m_ChunkShader->SetUniformMat4f("u_MVP", MVP);
    m_Renderer.Draw(*m_VAO, *m_ChunkShader, m_Elements);
}
