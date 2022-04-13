#include "WorldSegment.h"

WorldSegmnet::WorldSegmnet() { m_Chunks.reserve(SEGMENT_AREA); }

WorldSegmnet::~WorldSegmnet() {
    for (auto& chunk : m_Chunks) delete chunk.second;
}

uint8_t WorldSegmnet::Get(int x, int y, int z) const {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;  // Block position inside chunk
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        return 0;
    else
        return m_Chunks.find({chunkX, chunkZ})->second->Get(x, y, z);
}

void WorldSegmnet::Set(int x, int y, int z, BlockType type) {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        m_Chunks.emplace(SegmentPos({chunkX, chunkZ}), new Chunk);

    m_Chunks.find({chunkX, chunkZ})->second->Set(x, y, z, type);
}

void WorldSegmnet::Render(Player& player) {
    glm::mat4 model;
    for (auto& chunk : m_Chunks) {
        model = glm::translate(glm::mat4(1),
                               glm::vec3(chunk.first.x * CHUNK_SIZE, 0,
                                         chunk.first.z * CHUNK_SIZE));
        player.SetModelM(model);
        // if ((int)player.GetPosition().x == chunk.first.x)
        chunk.second->Render(player.GetMVP());
    }
}
