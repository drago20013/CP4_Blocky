#pragma once

#include <vector>
#include "Renderer.h"

struct VertexBufferElement {
	unsigned int type;
	unsigned int count;
	unsigned int normalized;

	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type) {
		case GL_FLOAT: return sizeof(GLfloat);
		case GL_UNSIGNED_INT: return sizeof(GLuint);
		case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout {
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout() : m_Stride(0) {}

	template<typename T>
	void Push(unsigned int count) {
		ASSERT(false);
	}

	template<>
	void Push<float>(unsigned int count) {
		m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<glm::vec2>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count * 2, GL_FALSE });
		m_Stride += 2 * count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<glm::vec3>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count * 3, GL_FALSE });
		m_Stride += 3 * count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<glm::vec4>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count * 4, GL_FALSE });
		m_Stride += 4 * count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};