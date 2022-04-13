#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


namespace glm {
	typedef glm::tvec3<GLubyte> vec3b;
	typedef glm::tvec2<GLubyte> vec2b;
}

struct Vertex {
	glm::vec3b Position;
	//float DirectLight;
};

enum class BlockType {
	BlockType_Default = 0,
	BlockType_Grass,
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
