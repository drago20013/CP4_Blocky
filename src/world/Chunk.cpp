#include "Chunk.h"

#include <cstring>
#include <random>
#include <ranges>

#include "../../includes/world/WorldSegment.h"
#include "Block.h"

FastNoise::SmartNode<> Chunk::m_rootNoiseNode =
    FastNoise::NewFromEncodedNodeTree(
        "EwAK16M8DQAGAAAAAAAAQAkAAFK4Hj8AAAAAPw==");

Chunk::Chunk(int x, int z, WorldSegment* segment,
             std::shared_ptr<Shader>& ChunkShader,
             std::shared_ptr<Renderer>& renderer)
    : m_Segment(segment), m_PosX(x), m_PosZ(z), m_Changed(true) {
    m_Elements[0] = 0;
    m_Elements[1] = 0;
    m_Elements[2] = 0;
    m_Modified = false;
    m_ChunkShader = ChunkShader;
    m_Renderer = renderer;
    m_VBO = std::make_unique<VertexBuffer>();
    m_TransparentVBO = std::make_unique<VertexBuffer>();
    m_WaterVBO = std::make_unique<VertexBuffer>();
    m_VAO = std::make_unique<VertexArray>();
    m_TransparentVAO = std::make_unique<VertexArray>();
    m_WaterVAO = std::make_unique<VertexArray>();
    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->PushAttrib<GLubyte>(1);  // VertexData1
    m_Layout->PushAttrib<GLubyte>(1);  // VertexData2
    m_Layout->PushAttrib<GLubyte>(1);  // VertexData3
    m_Layout->PushAttrib<GLubyte>(1);  // VertexData4
    m_Loaded = false;
    m_noiseOutput.resize(CHUNK_AREA);
    m_treesNoise.resize(CHUNK_AREA);
    std::hash<SegmentPos> hash;

    std::default_random_engine generator;
    generator.seed(hash({m_PosX, m_PosZ}));
    std::uniform_int_distribution<int> distribution(0, 1000);

    srand(hash({m_PosX, m_PosZ}));
    std::ranges::for_each(m_treesNoise,
                          [&](float& n) { n = distribution(generator); });

    // Generate a 16 x 16 area of noise for hight map
    m_rootNoiseNode->GenUniformGrid2D(m_noiseOutput.data(), m_PosZ * CHUNK_SIZE,
                                      m_PosX * CHUNK_SIZE, CHUNK_SIZE,
                                      CHUNK_SIZE, 0.2f, SEED);
    std::ranges::for_each(m_noiseOutput, [](float& n) {
        n = (n + 1) * .5f;
        n *= CHUNK_SIZE * 6;
    });
}

Chunk::~Chunk() {
    m_Vertecies[0].clear();
    m_Vertecies[1].clear();
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

void Chunk::SetActive(int x, int y, int z, bool activeLevel) {
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    m_Blocks[x][y][z].SetActive(activeLevel);
}

void Chunk::SetModified() { m_Modified = true; }

void Chunk::SetChanged(bool changedLevel) { m_Changed = changedLevel; }

bool Chunk::IsActive(int x, int y, int z) const {
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    return m_Blocks[x][y][z].IsActive();
}

bool Chunk::IsTransparent(int x, int y, int z) const {
    if (x < 0) x += CHUNK_SIZE;
    if (z < 0) z += CHUNK_SIZE;
    return m_Blocks[x][y][z].IsTransparent();
}

void Chunk::ReMesh() {
    if (m_Changed) {
        m_Vertecies[0].clear();
        m_Vertecies[1].clear();
        Generate();
    }
}

void Chunk::Unload() {
    m_Vertecies[0].resize(0);
    m_Vertecies[1].resize(0);
    m_Loaded = false;
}

void Chunk::PlaceTree(int x, int y, int z) {
    int height{};

    if (m_treesNoise[x * CHUNK_SIZE + z] < 10)
        height = 4;
    else if (m_treesNoise[x * CHUNK_SIZE + z] < 20)
        height = 5;
    else
        height = 6;

    BlockType woodType{BlockType::OakWood};
    BlockType leafType{BlockType::OakLeaf};

    if (m_treesNoise[x * CHUNK_SIZE + z] < 5) {
        woodType = BlockType::BirchWood;
        leafType = BlockType::BirchLeaf;
    }

    // leaves
    for (int _x = x - 2; _x <= x + 2; _x++) {
        for (int _y = y + height - 2; _y < y + height; _y++) {
            for (int _z = z - 2; _z <= z + 2; _z++) {
                if (!IsActive(_x, _y, _z)) {
                    Set(_x, _y, _z, leafType);
                    SetActive(_x, _y, _z, true);
                }
            }
        }
    }
    for (int _x = x - 1; _x <= x + 1; _x++) {
        for (int _z = z - 1; _z <= z + 1; _z++) {
            if (!IsActive(_x, y + height, _z)) {
                Set(_x, y + height, _z, leafType);
                SetActive(_x, y + height, _z, true);
            }
        }
    }

    Set(x - 1, y + height + 1, z, leafType);
    SetActive(x - 1, y + height + 1, z, true);
    Set(x + 1, y + height + 1, z, leafType);
    SetActive(x + 1, y + height + 1, z, true);
    Set(x, y + height + 1, z - 1, leafType);
    SetActive(x, y + height + 1, z - 1, true);
    Set(x, y + height + 1, z + 1, leafType);
    SetActive(x, y + height + 1, z + 1, true);
    Set(x, y + height + 1, z, leafType);
    SetActive(x, y + height + 1, z, true);

    // trunk
    for (int i = 0; i < height; i++) {
        Set(x, y + i, z, woodType);
        SetActive(x, y + i, z, true);
    }
}

void Chunk::Load() {
    if (!m_Loaded) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    int height = m_noiseOutput[x * CHUNK_SIZE + z];
                    if (y > height && y < WATER_LEVEL) {
                        Set(x, y, z, BlockType::Water);
                        SetActive(x, y, z, true);
                    } else if (y == height) {
                        if (y < WATER_LEVEL + 1) {
                            Set(x, y, z, BlockType::Sand);
                        } else {
                            Set(x, y, z, BlockType::Grass);
                            if (x > 1 && x < CHUNK_SIZE - 2 && z > 1 &&
                                z < CHUNK_SIZE - 2) {
                                if (m_treesNoise[x * CHUNK_SIZE + z] <= 20)
                                    PlaceTree(x, y + 1, z);
                                else if (m_treesNoise[x * CHUNK_SIZE + z] >=
                                         999) {
                                    Set(x, y + 1, z, BlockType::Pumpkin);
                                    SetActive(x, y + 1, z, true);
                                }
                            }
                        }
                        SetActive(x, y, z, true);
                    } else if (y < height && y >= height - 2) {
                        Set(x, y, z, BlockType::Dirt);
                        SetActive(x, y, z, true);
                    } else if (y < height - 2) {
                        Set(x, y, z, BlockType::Stone);
                        SetActive(x, y, z, true);
                    }
                }
            }
        }
    }
    m_Loaded = true;
}

void Chunk::Generate() {
    int i{};
    uint8_t blockType{};

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (!IsActive(x, y, z)) {  // if empty
                    continue;
                }

                blockType = (int)Get(x, y, z);
                // bl 00 - 0
                // br 01 - 1
                // tl 10 - 2
                // tr 11 - 3

                // 0-left-right face
                // 1-front-back face
                // 2-top face
                // 3-bottom face

                if (Get(x, y, z) == BlockType::Water) {
                    i = 1;
                } else if (IsTransparent(x, y, z)) {
                    i = 2;
                } else
                    i = 0;

                if ((x == 0 &&
                         !m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x - 1, y,
                                              (m_PosZ * CHUNK_SIZE) + z) ||
                     (m_Segment->IsTransparent((m_PosX * CHUNK_SIZE) + x - 1, y,
                                               (m_PosZ * CHUNK_SIZE) + z) &&
                      !IsTransparent(x, y, z))) ||
                    x > 0 && (!IsActive(x - 1, y, z) ||
                              (IsTransparent(x - 1, y, z) &&
                               !IsTransparent(x, y, z)))) {
                    // View from negative x (right face)
                    //========================x========y=====vpos=====light======z======blocktype
                    m_Vertecies[i].emplace_back(x | (y << 5),
                                                (y >> 3) | 0 << 4 | 0 << 6, (z),
                                                blockType);  // bl
                    m_Vertecies[i].emplace_back(x | (y << 5),
                                                (y >> 3) | 1 << 4 | 0 << 6,
                                                (z + 1), blockType);  // br
                    m_Vertecies[i].emplace_back(
                        x | ((y + 1) << 5), ((y + 1) >> 3) | 2 << 4 | 0 << 6,
                        (z), blockType);  // tl
                    m_Vertecies[i].emplace_back(
                        x | ((y + 1) << 5), ((y + 1) >> 3) | 2 << 4 | 0 << 6,
                        (z), blockType);  // tl
                    m_Vertecies[i].emplace_back(x | (y << 5),
                                                (y >> 3) | 1 << 4 | 0 << 6,
                                                (z + 1), blockType);  // br
                    m_Vertecies[i].emplace_back(x | ((y + 1) << 5),
                                                (y + 1) >> 3 | 3 << 4 | 0 << 6,
                                                (z + 1), blockType);  // tr
                }

                if (x == CHUNK_SIZE - 1 &&
                        (!m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x + 1, y,
                                              (m_PosZ * CHUNK_SIZE) + z) ||
                         (m_Segment->IsTransparent(
                              (m_PosX * CHUNK_SIZE) + x + 1, y,
                              (m_PosZ * CHUNK_SIZE) + z) &&
                          !IsTransparent(x, y, z))) ||
                    x < CHUNK_SIZE - 1 && (!IsActive(x + 1, y, z) ||
                                           (IsTransparent(x + 1, y, z) &&
                                            !IsTransparent(x, y, z)))) {
                    // View from positive x (left face)
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 0 << 4 | 0 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 1 << 4 | 0 << 6, z,
                                                blockType);
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 2 << 4 | 0 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 2 << 4 | 0 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 1 << 4 | 0 << 6, z,
                                                blockType);
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 3 << 4 | 0 << 6,
                                                z, blockType);
                }

                if (z == 0 &&
                        (!m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x, y,
                                              (m_PosZ * CHUNK_SIZE) + z - 1) ||
                         (m_Segment->IsTransparent(
                              (m_PosX * CHUNK_SIZE) + x, y,
                              (m_PosZ * CHUNK_SIZE) + z - 1) &&
                          !IsTransparent(x, y, z))) ||
                    z > 0 && (!IsActive(x, y, z - 1) ||
                              (IsTransparent(x, y, z - 1) &&
                               !IsTransparent(x, y, z)))) {
                    // View from negative z (front face)
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 0 << 4 | 1 << 6, z,
                                                blockType);
                    m_Vertecies[i].emplace_back(
                        x | y << 5, y >> 3 | 1 << 4 | 1 << 6, z, blockType);
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 2 << 4 | 1 << 6,
                                                z, blockType);
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 2 << 4 | 1 << 6,
                                                z, blockType);
                    m_Vertecies[i].emplace_back(
                        x | y << 5, y >> 3 | 1 << 4 | 1 << 6, z, blockType);
                    m_Vertecies[i].emplace_back(x | (y + 1) << 5,
                                                (y + 1) >> 3 | 3 << 4 | 1 << 6,
                                                z, blockType);
                }

                if (z == CHUNK_SIZE - 1 &&
                        (!m_Segment->IsActive((m_PosX * CHUNK_SIZE) + x, y,
                                              (m_PosZ * CHUNK_SIZE) + z + 1) ||
                         (m_Segment->IsTransparent(
                              (m_PosX * CHUNK_SIZE) + x, y,
                              (m_PosZ * CHUNK_SIZE) + z + 1) &&
                          !IsTransparent(x, y, z))) ||
                    z < CHUNK_SIZE - 1 && (!IsActive(x, y, z + 1) ||
                                           (IsTransparent(x, y, z + 1) &&
                                            !IsTransparent(x, y, z)))) {
                    // View from positive z (back face)
                    m_Vertecies[i].emplace_back(x | y << 5,
                                                y >> 3 | 0 << 4 | 1 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 1 << 4 | 1 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back(x | (y + 1) << 5,
                                                (y + 1) >> 3 | 2 << 4 | 1 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back(x | (y + 1) << 5,
                                                (y + 1) >> 3 | 2 << 4 | 1 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 1 << 4 | 1 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 3 << 4 | 1 << 6,
                                                (z + 1), blockType);
                }

                if (y == 0 || y > 0 && !IsActive(x, y - 1, z) ||
                    (IsTransparent(x, y - 1, z) && !IsTransparent(x, y, z))) {
                    // View from negative y (bottom face)
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 0 << 4 | 3 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back(x | y << 5,
                                                y >> 3 | 1 << 4 | 3 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 2 << 4 | 3 << 6, z,
                                                blockType);
                    m_Vertecies[i].emplace_back((x + 1) | y << 5,
                                                y >> 3 | 2 << 4 | 3 << 6, z,
                                                blockType);
                    m_Vertecies[i].emplace_back(x | y << 5,
                                                y >> 3 | 1 << 4 | 3 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back(
                        x | y << 5, y >> 3 | 3 << 4 | 3 << 6, z, blockType);
                }

                if (y == CHUNK_HEIGHT - 1 ||
                    y < CHUNK_HEIGHT - 1 && !IsActive(x, y + 1, z) ||
                    (IsTransparent(x, y + 1, z) && !IsTransparent(x, y, z))) {
                    // View from positive y (up face)
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 0 << 4 | 2 << 6,
                                                z, blockType);
                    m_Vertecies[i].emplace_back(x | (y + 1) << 5,
                                                (y + 1) >> 3 | 1 << 4 | 2 << 6,
                                                z, blockType);
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 2 << 4 | 2 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back((x + 1) | (y + 1) << 5,
                                                (y + 1) >> 3 | 2 << 4 | 2 << 6,
                                                (z + 1), blockType);
                    m_Vertecies[i].emplace_back(x | (y + 1) << 5,
                                                (y + 1) >> 3 | 1 << 4 | 2 << 6,
                                                z, blockType);
                    m_Vertecies[i].emplace_back(x | (y + 1) << 5,
                                                (y + 1) >> 3 | 3 << 4 | 2 << 6,
                                                (z + 1), blockType);
                }
            }
        }
    }

    m_Elements[0] = m_Vertecies[0].size();
    m_Elements[1] = m_Vertecies[1].size();
    m_Elements[2] = m_Vertecies[2].size();
    m_Changed = false;
}

void Chunk::Update() {
    m_VBO->LoadData(m_Vertecies[0].data(), m_Elements[0] * 4);
    m_TransparentVBO->LoadData(m_Vertecies[2].data(), m_Elements[2] * 4);
    m_WaterVBO->LoadData(m_Vertecies[1].data(), m_Elements[1] * 4);
    m_VAO->AddBuffer(*m_VBO, *m_Layout);
    m_TransparentVAO->AddBuffer(*m_TransparentVBO, *m_Layout);
    m_WaterVAO->AddBuffer(*m_WaterVBO, *m_Layout);
    m_Vertecies[0].resize(0);
    m_Vertecies[1].resize(0);
    m_Vertecies[2].resize(0);
}

void Chunk::Render(const glm::mat4& MVP) {
    glm::vec4 center =
        MVP * glm::vec4(CHUNK_SIZE_HALF, CHUNK_HEIGHT_HALF, CHUNK_SIZE_HALF, 1);

    float d = glm::length(center);
    center.x /= center.w;
    center.y /= center.w;

    // If it is behind the camera
    if (center.z < -CHUNK_HEIGHT_HALF) return;

    // If it is outside the screen
    if (fabsf(center.x) > 1 + fabsf(CHUNK_HEIGHT * 2 / center.w) ||
        fabsf(center.y) > 1 + fabsf(CHUNK_HEIGHT * 2 / center.w))
        return;

    if (m_Loaded && !m_Changed) {
        Update();
    }

    m_ChunkShader->Bind();
    m_ChunkShader->SetUniformMat4f("u_MVP", MVP);

    m_Renderer->Draw(*m_VAO, *m_ChunkShader, m_Elements[0]);
    m_Renderer->Draw(*m_WaterVAO, *m_ChunkShader, m_Elements[1]);
    m_Renderer->Draw(*m_TransparentVAO, *m_ChunkShader, m_Elements[2]);
}
