#pragma once
#include <cstdint>
#include <memory>

#include "../VertexBuffer.h"
#include "../VertexArray.h"
#include "../VertexBufferLayout.h"
#include "../Shader.h"
#include "../Texture.h"
#include "Block.h"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 128
#define CHUNK_AREA CHUNK_SIZE*CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_AREA*CHUNK_HEIGHT

class WorldSegmnet;

class Chunk {
public:
	Chunk(int x, int z, WorldSegmnet* segment);
	~Chunk();

	BlockType Get(int x, int y, int z) const;
	void Set(int x, int y, int z, BlockType type);

	void SetActive(int x, int y, int z, bool activeLevel);
	bool IsActive(int x, int y, int z);
	int GetPosX() { return m_PosX; }
	int GetPosZ() { return m_PosZ; }
	glm::mat4& GetModel() { return m_Model; }
	void SetModel(glm::mat4 newModel) { m_Model = newModel; }

	void Update();
	void Render(const glm::mat4& MVP);

private:
	int m_PosX;
	int m_PosZ;
	WorldSegmnet* m_Segment;
	Block m_Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]; //Blocks storage, for now defined only as their type
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::unique_ptr<Shader> m_ChunkShader;
	Vertex* m_Vertecies;
	Renderer m_Renderer;
	Texture m_WhiteTexture;
	int m_Elements;
	bool m_Changed;

	glm::mat4 m_Model;
};
