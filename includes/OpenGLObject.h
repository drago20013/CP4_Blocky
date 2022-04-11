#pragma once

class OpenGLObject {
protected:
	unsigned int m_RenderID{};
public:
	virtual void Bind() const {};
	virtual void Unbind() const = 0;
};