#include "FileLoader.h"
#include <fstream>
#include <sstream>

extern std::filesystem::path g_WorkDir;

ShaderProgramSource FileLoader::LoadShader(const std::string& filename)
{
    for (auto& entry : std::filesystem::recursive_directory_iterator(m_ShaderPath)) {
        if (entry.path().extension() == ".glsl" && entry.path().stem() == filename) {
            std::ifstream inFile(entry.path());

            if (!inFile) {
                printf("Cannot open a file: %s\n", entry.path().string().c_str());
            }
            std::string data, tmp;
            bool versionMatch{}, vertexMatch{}, fragmentMatch{};
            while (getline(inFile, tmp)) {
                data += tmp + "\n";
                if (std::regex_match(tmp, m_OpenGLVersion))
                    versionMatch = true;
                else if (std::regex_match(tmp, m_Vertex))
                    vertexMatch = true;
                else if (std::regex_match(tmp, m_Fragment))
                    fragmentMatch = true;
            }

            if (!versionMatch && !vertexMatch && !fragmentMatch) {
                printf("Your shader file is incomplete.\n");
                return { "Wrong Shader", "Content" };
            }

            enum class ShaderType {
                NONE = -1, VERTEX = 0, FRAGMENT = 1
            };

            ShaderType type = ShaderType::NONE;
            std::string line;
            std::stringstream ss[2];

            std::istringstream iss(data);

            while(getline(iss, line))
            {
                if (line.find("#shader") != std::string::npos) {
                    if (line.find("vertex") != std::string::npos)
                        type = ShaderType::VERTEX;
                    else if (line.find("fragment") != std::string::npos)
                        type = ShaderType::FRAGMENT;
                }
                else {
                    ss[(int)type] << line << '\n';
                }
            }
            return { ss[0].str(), ss[1].str() };
        }
    }
}

FileLoader::FileLoader()
{
    m_OpenGLVersion = (R"((#version 330 core))");
    m_Vertex = (R"((#shader vertex))");
    m_Fragment = (R"((#shader fragment))");;
    m_ShaderPath = g_WorkDir / "res/shaders";
}