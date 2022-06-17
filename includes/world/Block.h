#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


namespace glm {
	typedef glm::tvec3<GLubyte> vec3b;
	typedef glm::tvec2<GLubyte> vec2b;
}

struct Vertex {
	GLubyte VertexData1; // x and y1, 
	GLubyte VertexData2; // y2 and z, 
	GLubyte VertexData3; //blockType, vertexpos
};

enum class BlockType {
	BlockType_Default = 7, //Air
	BlockType_Grass = 0,
	BlockType_Dirt = 1,
	BlockType_Water = 2,
	BlockType_Stone = 3,
	BlockType_Wood = 4,
	BlockType_Sand = 5
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
