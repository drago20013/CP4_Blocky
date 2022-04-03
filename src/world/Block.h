#pragma once

enum class BlockType {
	Default = 0,
	Grass,
	Dirt,
	Water,
	Stone,
	Wood,
	Sand,
	NumTypes,
};

class Block {
public:
	Block();
	~Block();
	inline bool IsActive() { return m_Active; }
	void SetActive(bool active) { m_Active = active; }
private: 
	bool m_Active;
	BlockType m_BlockType;
};