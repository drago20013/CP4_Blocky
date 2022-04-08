#pragma once
#include "Chunk.h"
#include "../Player.h"
#include <unordered_map>

#define SEGMENT_SIZE 16
//#define SEGMENT_HEIGHT 128
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
		return h1 ^ (h2 << 1); // or use boost::hash_combine
	}
};

class WorldSegmnet {
public:
	WorldSegmnet();
	~WorldSegmnet();

	uint8_t Get(int x, int y, int z) const;
	void Set(int x, int y, int z, uint8_t type);

	void Render(Player& player);

private:
	std::unordered_map<SegmentPos, Chunk*> m_Chunks;
};