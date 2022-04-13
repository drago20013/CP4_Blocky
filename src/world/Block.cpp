#include "Block.h"

Block::Block() {
	m_Active = false;
	m_BlockType = BlockType::BlockType_Default;
}

bool Block::IsActive() const {
	return m_Active;
}

void Block::SetActive(bool active) {
	m_Active = active;
}

void Block::SetType(BlockType type){
    m_BlockType = type;
}
