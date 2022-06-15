#pragma once
#include "Chunk.h"
#include "../Player.h"
#include <unordered_map>
#include <mutex>

#define SEGMENT_SIZE 16
#define SEGMENT_AREA SEGMENT_SIZE*SEGMENT_SIZE

struct SegmentPos
{
	int x;
	int z;

	bool operator==(const SegmentPos& rhs) const {
		return x == rhs.x && z == rhs.z;
	}
};

template<std::same_as<SegmentPos> T>
struct std::hash<T>
{
	std::size_t operator()(T const& t) const noexcept
	{
		std::size_t h1 = std::hash<int>{}(t.x);
		std::size_t h2 = std::hash<int>{}(t.z);
		return h1 ^ (h2 << 1);
	}
};

class WorldSegment {
public:
	WorldSegment(std::shared_ptr<Player> player);
	~WorldSegment();

	BlockType Get(int x, int y, int z) const;
	bool IsActive(int x, int y, int z) const;
	void Set(int x, int y, int z, BlockType type);
	void SetActive(int x, int y, int z, bool activeLevel);

	void Render();
    void Load();
    void Unload();
	void Update();
	void Generate();
	void CheckCollision();
	void GenereteSegment();

private:
	std::shared_ptr<Player> m_Player;
	std::unordered_map<SegmentPos, Chunk*> m_Chunks;
	std::vector<Chunk*> m_ToRender;
    std::vector<Chunk*> m_ToLoad;
	std::vector<Chunk*> m_ToUnload;
	std::vector<Chunk*> m_ToGenerate;
	std::mutex m_ToLoadMutex;
	std::mutex m_ToGenerateMutex;
	std::mutex m_UnLoadMutex;

	std::shared_ptr<Shader> m_ChunkShader;
};
