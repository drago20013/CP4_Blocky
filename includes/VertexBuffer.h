#pragma once
struct Vertex;
class VertexBuffer {
private:
	unsigned int m_RenderID;
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void LoadData(const void* data, unsigned int size);

	void Bind() const;
	void Unbind() const;
};

