#include "WorldSegment.h"

#include <cmath>
#include <filesystem>
#include <future>
#include <ranges>

extern std::filesystem::path g_WorkDir;

WorldSegment::WorldSegment(std::shared_ptr<Player> player) : m_Player(player) {
    m_Chunks.reserve(SEGMENT_AREA);
    m_Renderer = std::make_shared<Renderer>();
    m_ChunkShader = std::make_shared<Shader>("ChunkShader");
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

BlockType WorldSegment::Get(int x, int y, int z) {
    glm::ivec3 pos(x, y, z);
    glm::ivec2 chunk = GetInChunkPos(pos);

    if (!m_Chunks.contains({chunk.x, chunk.y}))
        return BlockType::Default;
    else
        return m_Chunks.find({chunk.x, chunk.y})->second->Get(pos.x, pos.y, pos.z);
}

bool WorldSegment::IsActive(int x, int y, int z) {
    glm::ivec3 pos(x, y, z);
    glm::ivec2 chunk = GetInChunkPos(pos);

    if (m_Chunks.contains({chunk.x, chunk.y}))
        return m_Chunks.find({ chunk.x, chunk.y })->second->IsActive(pos.x, pos.y, pos.z);
    else
        return false;
}

bool WorldSegment::IsTransparent(int x, int y, int z) {
    glm::ivec3 pos(x, y, z);
    glm::ivec2 chunk = GetInChunkPos(pos);

    if (m_Chunks.contains({ chunk.x, chunk.y }))
        return m_Chunks.find({ chunk.x, chunk.y })->second->IsTransparent(pos.x, pos.y, pos.z);
    else
        return false;
}

void WorldSegment::Set(int x, int y, int z, BlockType type) {
    glm::ivec3 pos(x, y, z);
    glm::ivec2 chunk = GetInChunkPos(pos);

    if (!m_Chunks.contains({ chunk.x, chunk.y }))
        m_Chunks.emplace(SegmentPos{ chunk.x, chunk.y }, new Chunk(chunk.x, chunk.y, this, m_ChunkShader, m_Renderer));

    m_Chunks.find({ chunk.x, chunk.y })->second->Set(pos.x, pos.y, pos.z, type);
}

void WorldSegment::SetActive(int x, int y, int z, bool activeLevel) {
    glm::ivec3 pos(x, y, z);
    glm::ivec2 chunk = GetInChunkPos(pos);

    if (!m_Chunks.contains({ chunk.x, chunk.y }))
        m_Chunks.emplace(SegmentPos{ chunk.x, chunk.y }, new Chunk(chunk.x, chunk.y, this, m_ChunkShader, m_Renderer));

    m_Chunks.find({ chunk.x, chunk.y })->second->SetActive(pos.x, pos.y, pos.z, activeLevel);
}

void WorldSegment::SetModified(int x, int y, int z)
{
    glm::ivec3 pos(x, y, z);
    glm::ivec2 chunk = GetInChunkPos(pos);

    if (!m_Chunks.contains({ chunk.x, chunk.y }))
        m_Chunks.emplace(SegmentPos{ chunk.x, chunk.y }, new Chunk(chunk.x, chunk.y, this, m_ChunkShader, m_Renderer));

    m_Chunks.find({ chunk.x, chunk.y })->second->SetModified();
}

glm::ivec2 WorldSegment::GetInChunkPos(glm::ivec3& pos)
{
    glm::ivec2 chunk((int)floor((float)pos.x / CHUNK_SIZE), (int)floor((float)pos.z / CHUNK_SIZE));

    pos.x %= CHUNK_SIZE;
    pos.y %= CHUNK_HEIGHT;
    pos.z %= CHUNK_SIZE;

    return chunk;
}

bool WorldSegment::ShootRay(glm::ivec3 &blockPos, glm::ivec3& blockToSetPos) {
    glm::vec3 testpos = m_Player->GetCamPosition();
    glm::vec3 prevpos = testpos;
    int x;
    int y;
    int z;
    for (int i = 0; i < 1000; i++) {

        prevpos = testpos;
        testpos += m_Player->GetLookAt() * 0.005f;

        x = floorf(testpos.x);
        y = floorf(testpos.y);
        z = floorf(testpos.z);
       
        if ((Get(x, y, z) != BlockType::Default) && (Get(x, y, z) != BlockType::Water)) {
            int px = floorf(prevpos.x);
            int py = floorf(prevpos.y);
            int pz = floorf(prevpos.z);

            blockPos = glm::ivec3(x, y, z);
            blockToSetPos = glm::ivec3(px, py, pz);
            return true;
        }
    }
    return false;
}

void WorldSegment::DestroyBlock()
{
    glm::ivec3 blockPos;
    glm::ivec3 blockToSetPos;
    if (ShootRay(blockPos, blockToSetPos)) {
        Set(blockPos.x, blockPos.y, blockPos.z, BlockType::Default);
        SetModified(blockPos.x, blockPos.y, blockPos.z);
        int chunkX = (int)floor((float)blockPos.x / CHUNK_SIZE);
        int chunkZ = (int)floor((float)blockPos.z / CHUNK_SIZE);
        blockPos.x %= CHUNK_SIZE;
        blockPos.y %= CHUNK_HEIGHT;
        blockPos.z %= CHUNK_SIZE;
        if (blockPos.x < 0) blockPos.x += CHUNK_SIZE;
        if (blockPos.z < 0) blockPos.z += CHUNK_SIZE;

        if (blockPos.x == 0) {
            m_Chunks.at(SegmentPos(chunkX - 1, chunkZ))->SetChanged(true);
            m_Chunks.at(SegmentPos(chunkX - 1, chunkZ))->ReMesh();
        }else if (blockPos.x == CHUNK_SIZE - 1) {
            m_Chunks.at(SegmentPos(chunkX + 1, chunkZ))->SetChanged(true);
            m_Chunks.at(SegmentPos(chunkX + 1, chunkZ))->ReMesh();
        }
        if (blockPos.z == 0) {
            m_Chunks.at(SegmentPos(chunkX, chunkZ - 1))->SetChanged(true);
            m_Chunks.at(SegmentPos(chunkX, chunkZ - 1))->ReMesh();
        }else if (blockPos.z == CHUNK_SIZE-1) {
            m_Chunks.at(SegmentPos(chunkX, chunkZ + 1))->SetChanged(true);
            m_Chunks.at(SegmentPos(chunkX, chunkZ + 1))->ReMesh();
        }
    }
}

void WorldSegment::PlaceBlock()
{
    glm::ivec3 blockPos;
    glm::ivec3 blockToSetPos;
    int face;
    if (ShootRay(blockPos, blockToSetPos)) {
            Set(blockToSetPos.x, blockToSetPos.y, blockToSetPos.z, m_Player->GetEquipped());
            SetActive(blockToSetPos.x, blockToSetPos.y, blockToSetPos.z, true);
            SetModified(blockToSetPos.x, blockToSetPos.y, blockToSetPos.z);
    }
}

void WorldSegment::Render() {
    m_WhiteTexture->Bind(0);
    m_TextureAtlas->Bind(1);
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
    std::ranges::for_each(m_ToLoad, [&](Chunk* chunk) {
            auto as = std::async(std::launch::async, [&]() {
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
        });
}

void WorldSegment::Unload() {
        
    std::ranges::for_each(m_ToUnload, [&](Chunk* chunk) {
        auto as = std::async(std::launch::async, [&]() {
            chunk->Unload();
            });
        });

    std::ranges::for_each(m_ToUnload, [&](Chunk* chunk) {
            m_Chunks.erase({ chunk->GetPosX(),chunk->GetPosZ() });
            delete chunk;
        });
}

void WorldSegment::Generate() {
    std::ranges::for_each(m_ToGenerate, [&](Chunk* chunk) {
            auto a = std::async(std::launch::async, [&]() { chunk->Generate(); });
        });
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

        if (i && distance > 40 && chunk->IsLoaded() && !chunk->GetModified()) {
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

void WorldSegment::CheckCollision() {
    glm::vec3 newPos = m_Player->GetPosition();
    glm::vec3 lastPos = m_Player->GetLastPosition();

    glm::ivec3 currentBlock(floor(lastPos));
    glm::ivec3 targetBlock(newPos);

    for (int x = (currentBlock.x - 1); x <= (targetBlock.x + 1); x++) {
        for (int y = (currentBlock.y - 1); y <= (targetBlock.y + 1); y++) {
            for (int z = (currentBlock.z - 1); z <= (targetBlock.z + 1); z++) {
                if (IsActive(x, y, z) && Get(x, y, z) != BlockType::Water || IsActive(x, y+1, z) && Get(x, y+1, z) != BlockType::Water) {
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
                m_Chunks.emplace(SegmentPos{ posX, posZ }, new Chunk(posX, posZ, this, m_ChunkShader, m_Renderer));
        }
    }
}

void WorldSegment::Initialize() {
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
}