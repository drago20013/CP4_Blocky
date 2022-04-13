#pragma once
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoords;
	float TexID;
	float DirectLight;
};

enum class BlockType {
	BlockType_Default = 0,
	BlockType_Grass = 255,
	BlockType_Dirt,
	BlockType_Water,
	BlockType_Stone,
	BlockType_Wood,
	BlockType_Sand,
	BlockType_NumTypes,
};

class Block {
public: 
	Block();
	bool IsActive() const;
	void SetActive(bool active);
    void SetType(BlockType type);
    BlockType GetType()const {return m_BlockType;}
private: 
	bool m_Active;
	BlockType m_BlockType;
};