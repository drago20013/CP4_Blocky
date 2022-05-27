#include "WorldSegment.h"

WorldSegmnet::WorldSegmnet(std::shared_ptr<Player> player)
    : m_Player(player) {
    m_Chunks.reserve(SEGMENT_AREA);
}

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

bool WorldSegmnet::IsActive(int x, int y, int z) const {
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

void WorldSegmnet::Set(int x, int y, int z, BlockType type) {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        m_Chunks.emplace(SegmentPos({chunkX, chunkZ}),
                         new Chunk(chunkX, chunkZ, this));

    m_Chunks.find({chunkX, chunkZ})->second->Set(x, y, z, type);
}

void WorldSegmnet::SetActive(int x, int y, int z, bool activeLevel) {
    int chunkX = (int)floor((float)x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)z / CHUNK_SIZE);

    x %= CHUNK_SIZE;
    y %= CHUNK_HEIGHT;
    z %= CHUNK_SIZE;

    if (!m_Chunks.contains({chunkX, chunkZ}))
        m_Chunks.emplace(SegmentPos({chunkX, chunkZ}),
                         new Chunk(chunkX, chunkZ, this));

    m_Chunks.find({chunkX, chunkZ})->second->SetActive(x, y, z, activeLevel);
}

void WorldSegmnet::Render() {
    glm::mat4 model;
    for (auto& chunk : m_Chunks) {
        model = glm::translate(glm::mat4(1),
                               glm::vec3(chunk.first.x * CHUNK_SIZE, 0,
                                         chunk.first.z * CHUNK_SIZE));
        model = glm::translate(model, glm::vec3(-0.5f, 0, -0.5f));
        m_Player->SetModelM(model);
        chunk.second->Render(m_Player->GetMVP());
    }
}

// REDO ALL THIS *********************
void WorldSegmnet::CheckCollision() {
    glm::vec3 newPos = m_Player->GetPosition();
    glm::vec3 lastPos = m_Player->GetLastPosition();
    glm::ivec3 blockPos = newPos;
    glm::vec3 direction = glm::normalize(newPos - lastPos);

    int chunkX = (int)floor((float)newPos.x / CHUNK_SIZE);
    int chunkZ = (int)floor((float)newPos.z / CHUNK_SIZE);

    if (m_Chunks.contains({chunkX, chunkZ})) {
        if (direction.y < 0)  // check below
        {
            blockPos.x %= CHUNK_SIZE;
            blockPos.y %= CHUNK_HEIGHT;
            blockPos.z %= CHUNK_SIZE;
            m_Player->SetOnGround(false);

            if (m_Chunks.find({chunkX, chunkZ})
                    ->second->IsActive(blockPos.x, blockPos.y, blockPos.z)) {

                m_Player->SetOnGround(true);
                m_Player->SetCollision(Collision::GROUND, true);
                m_Player->SetVelocityX(0.0f);
                newPos.y = glm::floor(lastPos.y);
            }
        }

        if (direction.y > 0)  // check above
        {
        }

        if (direction.x > 0)  // check *player height* of blocks at x + 1
        {
        }

        if (direction.x < 0)  // check *player height* of blocks at x - 1
        {
        }

        if (direction.z > 0)  // check *player height* of blocks at z + 1
        {
            //blockPos.x %= CHUNK_SIZE;
            //blockPos.y %= CHUNK_HEIGHT;
            //blockPos.z++;
            //blockPos.z %= CHUNK_SIZE;

            ///*printf("pos: x=%f, y=%f, z=%f;\ndeltaPos: x=%f, y=%f, z=%f\n",
            //       pos.x, pos.y, pos.z, m_Player->GetDeltaPosition().x,
            //       m_Player->GetDeltaPosition().y,
            //       m_Player->GetDeltaPosition().z);*/

            //if (m_Chunks.find({ chunkX, chunkZ })
            //    ->second->IsActive(blockPos.x, blockPos.y, blockPos.z) && m_Chunks.find({ chunkX, chunkZ })
            //    ->second->IsActive(blockPos.x, blockPos.y+1, blockPos.z)) {
            //    // printf("block under me is y=%d, my y=%f\n", blockPos.y,
            //    // pos.y);
            //    m_Player->SetCollision(Collision::FRONT, true);
            //    pos.z = glm::floor(pos.z);
            //    m_Player->SetPosition(glm::vec3(pos.x, pos.y, pos.z));
            //}
        }

        if (direction.z < 0)  // check *player height* of blocks at z - 1
        {
        }

        // if (deltaPos.y < 0) {
        //// block under
        // blockPos.x %= CHUNK_SIZE;
        // blockPos.y--;
        // blockPos.y %= CHUNK_HEIGHT;
        // blockPos.z %= CHUNK_SIZE;

        // m_Player->SetCollision(Collision::GROUND, false);

        // if (m_Chunks.find({chunkX, chunkZ})
        //->second->IsActive(blockPos.x, blockPos.y, blockPos.z)) {
        //// printf("collision on y axis\n");
        // m_Player->SetCollision(Collision::GROUND, true);
        // m_Player->SetPosition(
        // glm::vec3(pos.x, (int)(pos.y + 0.5f), pos.z));
        // m_Player->SetDeltaPosition(
        // glm::vec3(deltaPos.x, 0, deltaPos.z));
        //}
        //} else if (deltaPos.y > 0) {
        //// block above
        // blockPos.x %= CHUNK_SIZE;
        // blockPos.y += m_Player->GetDimensions().y + 0.5f;
        // blockPos.y %= CHUNK_HEIGHT;
        // blockPos.z %= CHUNK_SIZE;

        // if (m_Chunks.find({chunkX, chunkZ})
        //->second->IsActive(blockPos.x, blockPos.y, blockPos.z)) {
        // m_Player->SetCollision(Collision::UP, true);
        // m_Player->SetPosition(glm::vec3(pos.x, (int)pos.y, pos.z));
        // m_Player->SetDeltaPosition(
        // glm::vec3(deltaPos.x, 0, deltaPos.z));
        //}
        //}

        m_Player->SetPosition(newPos);
    } else
        printf("Outside!!\n");
}
