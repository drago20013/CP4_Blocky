#pragma once
#include <cstdint>
#include <memory>
#include <FastNoise/FastNoise.h>
#include <mutex>

#include "../VertexBuffer.h"
#include "../VertexArray.h"
#include "../VertexBufferLayout.h"
#include "../Shader.h"
#include "../Texture.h"
#include "Block.h"

#define CHUNK_SIZE 16
#define CHUNK_SIZE_HALF CHUNK_SIZE/2
#define CHUNK_HEIGHT 127
#define CHUNK_HEIGHT_HALF CHUNK_HEIGHT/2
#define CHUNK_AREA CHUNK_SIZE*CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_AREA*CHUNK_HEIGHT
#define WATER_LEVEL 42
#define SEED 2137

class WorldSegment;

class Chunk {
public:
	Chunk(int x, int z, WorldSegment* segment, std::shared_ptr<Shader>& ChunkShader, std::shared_ptr<Renderer>& renderer);
	~Chunk();

	BlockType Get(int x, int y, int z) const;
	void Set(int x, int y, int z, BlockType type);

	void SetActive(int x, int y, int z, bool activeLevel);
	void SetChanged(bool changedLevel);
	void SetModified();
	bool GetModified() { return m_Modified; }
	bool IsActive(int x, int y, int z) const;
	bool IsTransparent(int x, int y, int z) const;
	bool IsLoaded() const{ 
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
	void PlaceTree(int, int, int);

private:
	static FastNoise::SmartNode<> m_rootNoiseNode;
	
	std::shared_ptr<Shader> m_ChunkShader;
	std::shared_ptr<Renderer> m_Renderer;

	int m_PosX;
	int m_PosZ;
	WorldSegment* m_Segment;
	Block m_Blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexBuffer> m_TransparentVBO;
	std::unique_ptr<VertexBuffer> m_WaterVBO;
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexArray> m_TransparentVAO;
	std::unique_ptr<VertexArray> m_WaterVAO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::vector<Vertex> m_Vertecies[3];

	
	int m_Elements[3];
	bool m_Changed;
	bool m_Loaded;
	bool m_Modified;
	std::vector<float> m_noiseOutput;
	std::vector<float> m_treesNoise;

	glm::mat4 m_Model;
};
