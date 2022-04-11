#pragma once

#include "OpenGLObject.h"
#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray : public OpenGLObject {
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const override;
	void Unbind() const override;
};

