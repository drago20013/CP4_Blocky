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

class Chunk {
public:
	Chunk();
	~Chunk();

	uint8_t Get(int x, int y, int z) const;
	void Set(int x, int y, int z, uint8_t type);

	void Update();
	void Render(const glm::mat4& MVP);

private:
	Block m_Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]; //Blocks storage, for now defined only as their type
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::unique_ptr<Shader> m_ChunkShader;
	glm::vec4byte* m_Vertecies;
	Renderer m_Renderer;
	Texture m_WhiteTexture;
	int m_Elements;
	bool m_Changed;

};