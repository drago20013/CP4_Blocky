#include "Block.h"

Block::Block() {
	m_Active = false;
	m_Transparent = false;
	m_BlockType = BlockType::Default;
}

bool Block::IsActive() const {
	return m_Active;
}

bool Block::IsTransparent() const {
	return m_Transparent;
}

void Block::SetActive(bool active) {
	m_Active = active;
}

void Block::SetType(BlockType type){
    m_BlockType = type;
	if (type == BlockType::Default)
		m_Active = false;
	if (type == BlockType::BirchLeaf || type == BlockType::OakLeaf || type == BlockType::Water || type == BlockType::Glass)
		m_Transparent = true;
	else
		m_Transparent = false;
}
