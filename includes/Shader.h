#pragma once

#include <string>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGLObject.h"

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader : public OpenGLObject {
private:
	std::string m_FilePath;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const override;
	void Unbind() const override;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform4v(const std::string& name, glm::vec4 v);
	void SetUniformMat4f(const std::string& name,const glm::mat4& matrix);
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	unsigned int GetUniformLocation(const std::string& name);
};