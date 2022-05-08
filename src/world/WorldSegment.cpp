#include "WorldSegment.h"

WorldSegmnet::WorldSegmnet(std::shared_ptr<Player> player) : m_Player(player) , m_lastPlayerPos(m_Player->GetPosition()) { m_Chunks.reserve(SEGMENT_AREA); }

WorldSegmnet::~WorldSegmnet() {
    for (auto& chunk : m_Chunks) delete chunk.second;
}

BlockType WorldSegmnet::Get(int x, int y, int z) const {
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

bool WorldSegmnet::IsActive(int x, int y, int z) const
{
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;  // Block position inside chunk
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (m_Chunks.contains({ chunkX, chunkZ }))
        return m_Chunks.find({ chunkX, chunkZ })->second->IsActive(x, y, z);
    else return false;
}

void WorldSegmnet::Set(int x, int y, int z, BlockType type) {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;
    
    if (!m_Chunks.contains({chunkX, chunkZ}))
        m_Chunks.emplace(SegmentPos({chunkX, chunkZ}), new Chunk(chunkX, chunkZ, this));

    m_Chunks.find({chunkX, chunkZ})->second->Set(x, y, z, type);
}

void WorldSegmnet::SetActive(int x, int y, int z, bool activeLevel)
{
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({ chunkX, chunkZ }))
        m_Chunks.emplace(SegmentPos({ chunkX, chunkZ }), new Chunk(chunkX, chunkZ, this));

    m_Chunks.find({ chunkX, chunkZ })->second->SetActive(x, y, z, activeLevel);
}

void WorldSegmnet::Render() {
    glm::mat4 model;
    for (auto& chunk : m_Chunks) {
        model = glm::translate(glm::mat4(1),
                               glm::vec3(chunk.first.x * CHUNK_SIZE, 0,
                                         chunk.first.z * CHUNK_SIZE));
        m_Player->SetModelM(model);
        chunk.second->Render(m_Player->GetMVP());
    }
}

//REDO ALL THIS *********************
void WorldSegmnet::CheckCollision()
{
    glm::vec3 pos = m_Player->GetPosition();
    glm::ivec3 blockPos = pos;
    int chunkX = (int)floor((float)pos.x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)pos.z / CHUNK_SIZE);
   
    if (m_Chunks.contains({ chunkX, chunkZ }))
    {
        blockPos.x %= CHUNK_SIZE;
        blockPos.y %= CHUNK_HEIGHT;
        blockPos.z %= CHUNK_SIZE;
        // Rough 
        if (m_Chunks.find({ chunkX, chunkZ })->second->IsActive(blockPos.x, blockPos.y, blockPos.z))
        {
            
        }
    }
    else 
        printf("Outside!!\n");

    m_lastPlayerPos = pos;
}
