#pragma once

#include <memory>
#include <vector>

#include "Block.h"
#include "../Renderer.h"
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../Texture.h"

class Chunk {
public:
	Chunk();
	~Chunk();
	void Update(float dt);
	void Render(Renderer* pRenderer, const glm::mat4& MVP);
	void CreateMesh();

private: // The blocks data
	Block*** m_Blocks;
	std::unique_ptr<VertexBuffer> m_Mesh;
	std::unique_ptr<VertexArray> m_Va;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::unique_ptr<Shader> m_Shader;
	std::vector<Vertex> m_Vertecies;
	Texture* m_Texture;
	unsigned int m_IndexCount;
	void CreateCube(int x, int y, int z);
};