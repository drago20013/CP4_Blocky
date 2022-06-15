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
        return BlockType::BlockType_Default;
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

void WorldSegment::Set(int x, int y, int z, BlockType type) {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        m_Chunks.emplace(SegmentPos({chunkX, chunkZ}),
                         new Chunk(chunkX, chunkZ, this, m_ChunkShader));

    m_Chunks.find({chunkX, chunkZ})->second->Set(x, y, z, type);
}

void WorldSegment::SetActive(int x, int y, int z, bool activeLevel) {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        m_Chunks.emplace(SegmentPos({chunkX, chunkZ}),
                         new Chunk(chunkX, chunkZ, this, m_ChunkShader));

    m_Chunks.find({chunkX, chunkZ})->second->SetActive(x, y, z, activeLevel);
}

void WorldSegment::Render() {
    for (auto& chunk : m_ToRender) {
        auto a = std::async(std::launch::async, [&]() { chunk->ReMesh(); });
    }

    for (auto& chunk : m_ToRender) {
        chunk->SetModel(glm::translate(
            glm::mat4(1), glm::vec3(chunk->GetPosX() * CHUNK_SIZE, 0,
                                    chunk->GetPosZ() * CHUNK_SIZE)));
        /*chunk->SetModel(
            glm::translate(chunk->GetModel(), glm::vec3(-0.5f, 0, -0.5f)));*/
        m_Player->SetModelM(chunk->GetModel());
        chunk->Render(m_Player->GetMVP());
    }
}

void WorldSegment::Load() {
    int X{}, Z{};
    // std::vector<std::jthread> threads;
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
    // for (auto& t : threads) {
    // t.detach();
    //}
}

void WorldSegment::Unload() {
    std::vector<std::jthread> threads;
    for (auto& chunk : m_ToUnload) {
        auto a = std::async(std::launch::async, [&]() { chunk->Unload(); });
    }
    // for (auto& t : threads) {
    // t.detach();
    //}
}

void WorldSegment::Generate() {
    // std::vector<std::jthread> threads;
    for (auto& chunk : m_ToGenerate) {
        auto a = std::async(std::launch::async, [&]() { chunk->Generate(); });
    }
    // for (auto& t : threads) {
    // t.detach();
    //}
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
    int i{2}, k{2};
    for (auto& chunk : m_Chunks) {
        distance = sqrt((playerChunkX - chunk.second->GetPosX()) *
                            (playerChunkX - chunk.second->GetPosX()) +
                        (playerChunkZ - chunk.second->GetPosZ()) *
                            (playerChunkZ - chunk.second->GetPosZ()));

        if (i && distance > 40 && chunk.second->IsLoaded()) {
            i--;
            m_ToUnload.push_back(chunk.second);
        }
        if (distance < 10 && chunk.second->IsLoaded())
            m_ToRender.push_back(chunk.second);
        else if (k && distance < 20 && !chunk.second->IsLoaded()) {
            k--;
            m_ToLoad.push_back(chunk.second);
            m_ToGenerate.push_back(chunk.second);
        }
    }

    Load();
    Unload();
    Generate();
}

void WorldSegment::CheckCollision() {
    glm::vec3 newPos = m_Player->GetPosition();
    glm::vec3 lastPos = m_Player->GetLastPosition();

    glm::ivec3 currentBlock(floor(lastPos));
    glm::ivec3 targetBlock(newPos);

    for (int x = (currentBlock.x - 1); x <= (targetBlock.x + 1); x++) {
        for (int y = (currentBlock.y - 1); y <= (targetBlock.y + 1); y++) {
            for (int z = (currentBlock.z - 1); z <= (targetBlock.z + 1); z++) {
                if (IsActive(x, y, z)) {
                    glm::vec3 nearestPoint;
                    nearestPoint.x =
                        std::max(float(x), std::min(newPos.x, float(x + 1)));
                    nearestPoint.y =
                        std::max(float(y), std::min(newPos.y, float(y + 1)));
                    nearestPoint.z =
                        std::max(float(z), std::min(newPos.z, float(z + 1)));

                    glm::vec3 rayToNearest = nearestPoint - newPos;

                    float rayMag = glm::length(rayToNearest);
                    float overlap = m_Player->GetDimensions().x - rayMag;

                    if (std::isnan(overlap)) overlap = 0;

                    if (overlap > 0 && rayMag > 0) {
                        newPos =
                            newPos - glm::normalize(rayToNearest) * overlap;
                    }
                }
            }
        }
    }

    // glm::vec3 direction(0);
    // if (glm::length(newPos - lastPos) > 0)
    // direction = glm::normalize(newPos - lastPos);

    // if (direction.y < 0)  // check below
    //{
    // m_Player->SetOnGround(false);

    // if (IsActive(newPos.x, newPos.y,
    // newPos.z > 0 ? newPos.z + .5f : newPos.z)) {
    // m_Player->SetOnGround(true);
    // m_Player->SetVelocityY(0.0f);
    // newPos.y = glm::floor(lastPos.y);
    //// printf("Collision going y-\n");
    //}
    //}

    // if (direction.y > 0)  // check above
    //{
    // if (IsActive(newPos.x, newPos.y + 2,
    // newPos.z > 0 ? newPos.z + .5f : newPos.z)) {
    // m_Player->SetVelocityY(0.0f);
    // newPos.y = lastPos.y;
    //}
    //}

    // if (direction.x < 0) {
    // if (IsActive(newPos.x, newPos.y,
    // newPos.z > 0 ? newPos.z + .5f : newPos.z) ||
    // IsActive(newPos.x, newPos.y + 1.0f,
    // newPos.z > 0 ? newPos.z + .5f : newPos.z)) {
    // m_Player->SetVelocityX(0.0f);
    // newPos.x = lastPos.x;
    // printf("Collision going x-\n");
    //}
    //}

    // if (direction.x > 0) {
    // if (IsActive(newPos.x, newPos.y,
    // newPos.z > 0 ? newPos.z + .5f : newPos.z) ||
    // IsActive(newPos.x, newPos.y + 1.0f,
    // newPos.z > 0 ? newPos.z + .5f : newPos.z)) {
    // m_Player->SetVelocityX(0.0f);
    // newPos.x = lastPos.x;
    // printf("Collision going x+\n");
    //}
    //}

    // if (direction.z < 0) {
    // if (IsActive(newPos.x, newPos.y,
    // newPos.z > 0 ? newPos.z - .5f : newPos.z) ||
    // IsActive(newPos.x, newPos.y + 1.0f,
    // newPos.z > 0 ? newPos.z - .5f : newPos.z)) {
    // m_Player->SetVelocityZ(0.0f);
    // newPos.z = lastPos.z;
    // printf("Collision going z-\n");
    //}
    //}

    // if (direction.z > 0) {
    // if (IsActive(newPos.x, newPos.y,
    // newPos.z > 0 ? newPos.z - .5f : newPos.z) ||
    // IsActive(newPos.x, newPos.y + 1.0f,
    // newPos.z > 0 ? newPos.z - .5f : newPos.z)) {
    // m_Player->SetVelocityZ(0.0f);
    // newPos.z = lastPos.z;
    // printf("Collision going z+\n");
    //}
    //}

    m_Player->SetPosition(newPos);
}

void WorldSegment::GenereteSegment() {
    int playerChunkX =
        (int)floor((float)m_Player->GetLastPosition().x / CHUNK_SIZE);
    int playerChunkZ =
        (int)floor((float)m_Player->GetLastPosition().z / CHUNK_SIZE);
    for (int posX = playerChunkX - 20; posX < playerChunkX + 20; posX++) {
        for (int posZ = playerChunkZ - 20; posZ < playerChunkZ + 20; posZ++) {
            if (!m_Chunks.contains({posX, posZ}))
                m_Chunks.emplace(SegmentPos({posX, posZ}),
                                 new Chunk(posX, posZ, this, m_ChunkShader));
        }
    }
}
