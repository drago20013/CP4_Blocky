#pragma once
#include <cstdint>
#include <memory>
#include <FastNoise/FastNoise.h>
#include <mutex>
#include <atomic>

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

class WorldSegment;

class Chunk {
public:
	Chunk(int x, int z, WorldSegment* segment, std::shared_ptr<Shader>& ChunkShader);
	~Chunk();

	BlockType Get(int x, int y, int z) const;
	void Set(int x, int y, int z, BlockType type);

	void SetActive(int x, int y, int z, bool activeLevel);
	void SetChanged(bool changedLevel);
	bool IsActive(int x, int y, int z);
	bool IsLoaded() { 
		return m_Loaded; 
	};
	void ReMesh();
	int GetPosX() { return m_PosX; }
	int GetPosZ() { return m_PosZ; }
	glm::mat4& GetModel() { return m_Model; }
	void SetModel(glm::mat4 newModel) { m_Model = newModel; }

	void Update();
    void Unload();
    void Load();
	void Generate();
	void Render(const glm::mat4& MVP);

private:
	static FastNoise::SmartNode<> m_rootNoiseNode;
	
	std::shared_ptr<Shader> m_ChunkShader;

	int m_PosX;
	int m_PosZ;
	WorldSegment* m_Segment;
	Block m_Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]; //Blocks storage, for now defined only as their type
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::vector<Vertex> m_Vertecies;
	Renderer m_Renderer;
	Texture m_WhiteTexture;
	int m_Elements;
	std::atomic<bool> m_Changed;
	std::atomic<bool> m_Loaded;
	std::vector<float> m_noiseOutput;
	std::mutex m_VerteciesMutex;

	glm::mat4 m_Model;
};
