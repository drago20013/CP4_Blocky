#pragma once

#include "Renderer.h"
#include "OpenGLObject.h"

class Texture : public OpenGLObject{
private:
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path);
	Texture(const uint32_t& color);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const override;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};

