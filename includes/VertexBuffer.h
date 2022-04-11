#pragma once
#include "OpenGLObject.h"

struct Vertex;
class VertexBuffer : public OpenGLObject{
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void LoadData(const void* data, unsigned int size);

	void Bind() const override;
	void Unbind() const override;
};

