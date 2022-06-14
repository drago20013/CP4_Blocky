#include "Chunk.h"

#include <cstring>

#include "../../includes/world/WorldSegment.h"
#include "Block.h"

FastNoise::SmartNode<> Chunk::m_rootNoiseNode = FastNoise::NewFromEncodedNodeTree("EwAK16M8DQAEAAAAzczMPwkAAHsUbj8A9ihcPw==");

Chunk::Chunk(int x, int z, WorldSegment* segment, std::shared_ptr<Shader>& ChunkShader)
    : m_Segment(segment), m_PosX(x), m_PosZ(z), m_Elements(0), m_Changed(true), m_Renderer(), m_WhiteTexture(0xffffffff) {
    m_ChunkShader = ChunkShader;

    m_VBO = std::make_unique<VertexBuffer>();
    m_VAO = std::make_unique<VertexArray>();
    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->PushAttrib<glm::vec3b>(1);
    m_Layout->PushAttrib<GLubyte>(1);
    m_Loaded = false;
    m_noiseOutput.resize(CHUNK_AREA);

    // Generate a 16 x 16 area of noise
    m_rootNoiseNode->GenUniformGrid2D(m_noiseOutput.data(), m_PosZ * CHUNK_SIZE, m_PosX * CHUNK_SIZE, CHUNK_SIZE , CHUNK_SIZE, 0.2f, 1338);
    for (auto& n : m_noiseOutput) {
        n *= CHUNK_HEIGHT * 0.5;
        if (n < 1) n = 1;
        n += CHUNK_SIZE * 2;
    }

    int index = 0;
}

Chunk::~Chunk() {
    m_VBO.reset(nullptr);
    m_Vertecies.clear();
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

void Chunk::SetChanged(bool changedLevel)
{
    m_Changed = changedLevel;
}

bool Chunk::IsActive(int x, int y, int z)
{
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    return m_Blocks[x][y][z].IsActive();
}

void Chunk::ReMesh()
{
    if (m_Changed) {
        m_Vertecies.clear();
        Generate();
    }
}

void Chunk::Unload(){
    //TODO(drago): Save chunk on disc(maybe)
    m_Vertecies.clear();
    m_Loaded = false;
}

void Chunk::Load(){
    //TODO(drago): Load chunk from disc(maybe)
    if (!m_Loaded) {
        int height;
        
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    if (y < m_noiseOutput[x * CHUNK_SIZE + z]) {
                        Set(x, y, z, BlockType::BlockType_Grass);
                        SetActive(x, y, z, true);
                    }
                }
            }
    } 
    m_Loaded = true;
}

void Chunk::Generate() {
    std::lock_guard<std::mutex> guard(m_VerteciesMutex);
    int i = 0;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {

                if (!m_Blocks[x][y][z].IsActive()) {  // if empty
                    continue;
                }
                if (x == 0 && !m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x - 1, y, (m_PosZ * CHUNK_SIZE) + z) || x > 0 && !m_Blocks[x - 1][y][z].IsActive()) {
                    // View from negative x (right face)
                    m_Vertecies.emplace_back( glm::vec3b(x, y, z), 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x, y, z + 1)    , 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x, y + 1, z)    , 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x, y + 1, z)    , 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x, y, z + 1)    , 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x, y + 1, z + 1), 8 );
                }

                if (x == CHUNK_SIZE - 1 && !m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x + 1, y, (m_PosZ * CHUNK_SIZE) + z) || x < CHUNK_SIZE - 1 && !m_Blocks[x + 1][y][z].IsActive()) {
                    // View from positive x (left face)
                    m_Vertecies.emplace_back( glm::vec3b(x + 1, y, z + 1)    , 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x + 1, y, z)        , 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x + 1, y + 1, z + 1), 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x + 1, y + 1, z + 1), 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x + 1, y, z)        , 8 );
                    m_Vertecies.emplace_back( glm::vec3b(x + 1, y + 1, z)    , 8 );
                }

                if (z == 0 && !m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x, y, (m_PosZ * CHUNK_SIZE) + z - 1) || z > 0 && !m_Blocks[x][y][z - 1].IsActive()) {
                    // View from negative z (front face)
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y, z), 6 );
                    m_Vertecies.emplace_back(glm::vec3b(x, y, z), 6);
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y + 1, z), 6);
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y + 1, z), 6);
                    m_Vertecies.emplace_back(glm::vec3b(x, y, z), 6);
                    m_Vertecies.emplace_back(glm::vec3b(x, y + 1, z)    , 6 );
                }

                if (z == CHUNK_SIZE - 1 && !m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x, y, (m_PosZ * CHUNK_SIZE) + z + 1) || z < CHUNK_SIZE - 1 && !m_Blocks[x][y][z + 1].IsActive()) {
                    // View from positive z (back face)
                    m_Vertecies.emplace_back(glm::vec3b(x, y, z + 1), 6 );
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y, z + 1), 6);
                    m_Vertecies.emplace_back(glm::vec3b(x, y + 1, z + 1), 6);
                    m_Vertecies.emplace_back(glm::vec3b(x, y + 1, z + 1), 6);
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y, z + 1), 6);
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y + 1, z + 1), 6 );
                }

                if (y == 0 || y > 0 && !m_Blocks[x][y - 1][z].IsActive()) {
                    // View from negative y (bottom face)
                    m_Vertecies.emplace_back(glm::vec3b(x, y, z), 4 );
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y, z), 4);
                    m_Vertecies.emplace_back(glm::vec3b(x, y, z + 1), 4);
                    m_Vertecies.emplace_back(glm::vec3b(x, y, z + 1), 4);
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y, z), 4);
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y, z + 1), 4 );
                }

                if (y == CHUNK_HEIGHT - 1 || y < CHUNK_HEIGHT - 1 && !m_Blocks[x][y + 1][z].IsActive()) {
                    // View from positive y (up face)
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y + 1, z), 10 );
                    m_Vertecies.emplace_back(glm::vec3b(x, y + 1, z), 10);
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y + 1, z + 1), 10);
                    m_Vertecies.emplace_back(glm::vec3b(x + 1, y + 1, z + 1), 10);
                    m_Vertecies.emplace_back(glm::vec3b(x, y + 1, z), 10);
                    m_Vertecies.emplace_back(glm::vec3b(x, y + 1, z + 1)    , 10 );
                }
            }
        }
    }

    m_Elements = m_Vertecies.size();
    m_Changed = false;
}

void Chunk::Update() {
    m_VBO->LoadData(m_Vertecies.data(), m_Elements * 4);
    m_VAO->AddBuffer(*m_VBO, *m_Layout);
}

void Chunk::Render(const glm::mat4& MVP) {
    if (m_Loaded && !m_Changed) {
        Update();
    }

    m_ChunkShader->Bind();
    m_ChunkShader->SetUniformMat4f("u_MVP", MVP);

    m_Renderer.Draw(*m_VAO, *m_ChunkShader, m_Elements);
}
