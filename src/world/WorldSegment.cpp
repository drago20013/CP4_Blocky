#include "WorldSegment.h"

#include <cmath>
#include <filesystem>
#include <future>
#include <thread>

extern std::filesystem::path g_WorkDir;

WorldSegment::WorldSegment(std::shared_ptr<Player> player) : m_Player(player) {
    m_Chunks.reserve(SEGMENT_AREA);
    m_ChunkShader = std::make_shared<Shader>(
        (g_WorkDir / "res/shaders/ChunkShader.glsl").string());
    m_WhiteTexture = std::make_shared<Texture>(0xffffffff);
    m_TextureAtlas = std::make_shared<Texture>((g_WorkDir / "res/textures/TextureAtlas.png").string());
    m_WhiteTexture->Bind(0);
    m_TextureAtlas->Bind(1); 

    m_ChunkShader->Bind();
    int samplers[2] = { 0, 1 };
    m_ChunkShader->SetUniform1iv("u_Textures",2, samplers);
    m_ChunkShader->SetUniform1i("u_TexIndex", 1);
}

WorldSegment::~WorldSegment() {
    for (auto& chunk : m_Chunks) delete chunk.second;
}

BlockType WorldSegment::Get(int x, int y, int z) const {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;  // Block position inside chunk
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        return BlockType::Default;
    else
        return m_Chunks.find({chunkX, chunkZ})->second->Get(x, y, z);
}

bool WorldSegment::IsActive(int x, int y, int z) const {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;  // Block position inside chunk
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (m_Chunks.contains({chunkX, chunkZ}))
        return m_Chunks.find({chunkX, chunkZ})->second->IsActive(x, y, z);
    else
        return false;
}

bool WorldSegment::IsTransparent(int x, int y, int z) const {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;  // Block position inside chunk
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (m_Chunks.contains({ chunkX, chunkZ }))
        return m_Chunks.find({ chunkX, chunkZ })->second->IsTransparent(x, y, z);
    else
        return false;
}

void WorldSegment::Set(int x, int y, int z, BlockType type) {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        m_Chunks.emplace(SegmentPos{ chunkX, chunkZ }, new Chunk(chunkX, chunkZ, this, m_ChunkShader));

    m_Chunks.find({chunkX, chunkZ})->second->Set(x, y, z, type);
}

void WorldSegment::SetActive(int x, int y, int z, bool activeLevel) {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        m_Chunks.emplace(SegmentPos{ chunkX, chunkZ }, new Chunk(chunkX, chunkZ, this, m_ChunkShader));

    m_Chunks.find({chunkX, chunkZ})->second->SetActive(x, y, z, activeLevel);
}

void WorldSegment::DestroyBlock()
{
    glm::vec3 testpos = m_Player->GetCamPosition();
    glm::vec3 prevpos = testpos;

    for (int i = 0; i < 1000; i++) {

        prevpos = testpos;
        testpos += m_Player->GetLookAt()*0.005f;

        int mx = floorf(testpos.x);
        int my = floorf(testpos.y);
        int mz = floorf(testpos.z);

        if ((Get(mx, my, mz) != BlockType::Default) && (Get(mx, my, mz) != BlockType::Water)) {
            Set(mx, my, mz, BlockType::Default);
            break;
        }
    }
}

void WorldSegment::PlaceBlock()
{
}

void WorldSegment::Render() {
    for (auto& [d , chunk] : m_ToRender) {
        auto a = std::async(std::launch::async, [&]() { chunk->ReMesh(); });
    }

    for (auto i = m_ToRender.rbegin(); i != m_ToRender.rend(); i++){
        i->second->SetModel(glm::translate(
            glm::mat4(1), glm::vec3(i->second->GetPosX() * CHUNK_SIZE, 0,
                i->second->GetPosZ() * CHUNK_SIZE)));
        m_Player->SetModelM(i->second->GetModel());
        i->second->Render(m_Player->GetMVP());
    }
}

void WorldSegment::Load() {
    int X{}, Z{};
    for (auto& chunk : m_ToLoad) {
        auto a = std::async(std::launch::async, [&]() {
            X = chunk->GetPosX();
            Z = chunk->GetPosZ();
            chunk->Load();

            if (m_Chunks.contains({X - 1, Z}) &&
                m_Chunks[{X - 1, Z}]->IsLoaded())
                m_Chunks[{X - 1, Z}]->SetChanged(true);
            if (m_Chunks.contains({X + 1, Z}) &&
                m_Chunks[{X + 1, Z}]->IsLoaded())
                m_Chunks[{X + 1, Z}]->SetChanged(true);
            if (m_Chunks.contains({X, Z - 1}) &&
                m_Chunks[{X, Z - 1}]->IsLoaded())
                m_Chunks[{X, Z - 1}]->SetChanged(true);
            if (m_Chunks.contains({X, Z + 1}) &&
                m_Chunks[{X, Z + 1}]->IsLoaded())
                m_Chunks[{X, Z + 1}]->SetChanged(true);
        });
    }
}

void WorldSegment::Unload() {
    for (auto& chunk : m_ToUnload) {
        auto a = std::async(std::launch::async, [&]() {
            chunk->Unload();
            });
    }

    for (auto& chunk : m_ToUnload) {
        m_Chunks.erase({ chunk->GetPosX(),chunk->GetPosZ() });
        delete chunk;
    }
}

void WorldSegment::Generate() {
    for (auto& chunk : m_ToGenerate) {
        auto a = std::async(std::launch::async, [&]() { chunk->Generate(); });
    }
}

void WorldSegment::Update() {
    int playerChunkX =
        (int)floor((float)m_Player->GetLastPosition().x / CHUNK_SIZE);
    int playerChunkZ =
        (int)floor((float)m_Player->GetLastPosition().z / CHUNK_SIZE);

    m_ToRender.clear();

    m_ToLoad.clear();

    m_ToUnload.clear();

    m_ToGenerate.clear();

    float distance{};
    int i{2}, k{10};
    for (auto& [chunkPos, chunk] : m_Chunks) {
        distance = sqrt((playerChunkX - chunkPos.x) *
                            (playerChunkX - chunkPos.x) +
                        (playerChunkZ - chunkPos.z) *
                            (playerChunkZ - chunkPos.z));

        if (i && distance > 40 && chunk->IsLoaded()) {
            i--;
            m_ToUnload.push_back(chunk);
        }
        if (distance < 15 && chunk->IsLoaded())
            m_ToRender.emplace(distance, chunk);
        else if (k && distance < 20 && !chunk->IsLoaded()) {
            k--;
            m_ToLoad.push_back(chunk);
            m_ToGenerate.push_back(chunk);
        }
    }

    Load();
    Unload();
    Generate();
}

//TODO (drago): Repair position of the collider
void WorldSegment::CheckCollision() {
    glm::vec3 newPos = m_Player->GetPosition();
    glm::vec3 lastPos = m_Player->GetLastPosition();

    glm::ivec3 currentBlock(floor(lastPos));
    glm::ivec3 targetBlock(newPos);

    for (int x = (currentBlock.x - 1); x <= (targetBlock.x + 1); x++) {
        for (int y = (currentBlock.y - 1); y <= (targetBlock.y + 1); y++) {
            for (int z = (currentBlock.z - 1); z <= (targetBlock.z + 1); z++) {
                if (IsActive(x, y, z) && Get(x, y, z) != BlockType::Water) {
                    glm::vec3 nearestPoint{
                        std::max(float(x), std::min(float(x+1), newPos.x)),
                        std::max(float(y), std::min(float(y+1), newPos.y)),
                        std::max(float(z), std::min(float(z+1), newPos.z)),
                    };

                    
                    glm::vec3 rayToNearest = nearestPoint - newPos;

                    float rayMag = glm::length(rayToNearest);
                    float overlap = m_Player->GetDimensions().x - rayMag;

                    if (std::isnan(overlap)) overlap = 0;

                    if (overlap > 0 && rayMag > 0) {
                        newPos =
                            newPos - glm::normalize(rayToNearest) * overlap;
                        
                        if (rayToNearest.y < 0) {
                            m_Player->SetVelocityY(0);
                            m_Player->SetOnGround(true);
                        }
                        if (rayToNearest.y > 0) {
                            m_Player->SetVelocityY(0);
                        }
                    }
                }
            }
        }
    }
    m_Player->SetPosition(newPos);
}

void WorldSegment::GenereteSegment() {
    int playerChunkX =
        (int)floor((float)m_Player->GetLastPosition().x / CHUNK_SIZE);
    int playerChunkZ =
        (int)floor((float)m_Player->GetLastPosition().z / CHUNK_SIZE);
    for (int posX = playerChunkX-20; posX <= playerChunkX+20; posX++) {
        for (int posZ = playerChunkZ-20; posZ <= playerChunkZ+20; posZ++) {
            if (!m_Chunks.contains({posX, posZ}))
                m_Chunks.emplace(SegmentPos{ posX, posZ }, new Chunk(posX, posZ, this, m_ChunkShader));
        }
    }
}

void WorldSegment::Initialize() {
    GenereteSegment();

    int playerChunkX =
        (int)floor((float)m_Player->GetLastPosition().x / CHUNK_SIZE);
    int playerChunkZ =
        (int)floor((float)m_Player->GetLastPosition().z / CHUNK_SIZE);
    float distance{};

    for (auto& [chunkPos, chunk] : m_Chunks) {
        distance = sqrt((playerChunkX - chunkPos.x) *
            (playerChunkX - chunkPos.x) +
            (playerChunkZ - chunkPos.z) *
            (playerChunkZ - chunkPos.z));

        if (distance < 15)
            m_ToRender.emplace(distance, chunk);

            m_ToLoad.push_back(chunk);
            m_ToGenerate.push_back(chunk);
    }

    Load();
    Generate();
    Render();
}