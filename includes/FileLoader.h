#pragma once
#include <regex>
#include <filesystem>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class FileLoader
{
public:
    FileLoader();

    ShaderProgramSource LoadShader(const std::string & filename);

private:
    std::regex m_OpenGLVersion;
    std::regex m_Vertex;
    std::regex m_Fragment;
    std::filesystem::path m_ShaderPath;
};

