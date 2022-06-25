#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


namespace glm {
	typedef glm::tvec3<GLubyte> vec3b;
	typedef glm::tvec2<GLubyte> vec2b;
}

struct Vertex {
	GLubyte VertexData1; // x, y1, 
	GLubyte VertexData2; // y2, vertexpos, directlight,
	GLubyte VertexData3; // z, blockType,
	GLubyte VertexData4; // blockType2
};

enum class BlockType {
	Default = 14, //Air
	Grass = 0,
	Dirt = 1,
	Water = 2,
	Stone = 3,
	OakWood = 4,
	Sand = 5,
	OakLeaf = 6,
	Brick = 7,
	OakPlank = 8,
	Glass = 9,
	BirchWood = 10,
	BirchLeaf = 11,
	BirchPlank = 12,
	Pumpkin = 13
};

class Block {
public: 
	Block();
	bool IsActive() const;
	bool IsTransparent() const;
	void SetActive(bool active);
    void SetType(BlockType type);
    BlockType GetType()const {return m_BlockType;}
private: 
	bool m_Active;
	bool m_Transparent;
	BlockType m_BlockType;
};
