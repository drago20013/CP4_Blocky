#include <filesystem>
#include "Chunk.h"

extern std::filesystem::path g_WorkDir;

Chunk::Chunk() : m_IndexCount(0), m_Va(std::make_unique<VertexArray>()), 
m_Layout(std::make_unique<VertexBufferLayout>()), m_Shader(std::make_unique<Shader>((g_WorkDir/"res/shaders/Basic.shader").string())) { // Create the blocks
    m_Blocks = new Block **[CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++) {
        m_Blocks[i] = new Block * [CHUNK_SIZE];
        for (int j = 0; j < CHUNK_SIZE; j++) {
            m_Blocks[i][j] = new Block[CHUNK_SIZE];
        }
    }

    m_Layout->Push_attrib<glm::vec3>(1);
    m_Layout->Push_attrib<glm::vec4>(1);
    m_Layout->Push_attrib<glm::vec2>(1);
    m_Layout->Push_attrib<float>(1);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                (z%3||y%3) ? m_Blocks[x][y][z].SetActive(true): m_Blocks[x][y][z].SetActive(false);
            }
        }
    }

    m_Texture = new Texture((g_WorkDir/"res/textures/dirt.jpg").string());
    m_Texture->Bind();
    m_Shader->Bind();
    m_Shader->SetUniform1i("u_Texture", 0);
}

Chunk::~Chunk() { // Delete the blocks
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			delete[] m_Blocks[i][j];
		}
		delete[] m_Blocks[i];
	}
	delete[] m_Blocks;
    delete m_Texture;
}

void Chunk::Render(Renderer* pRenderer, const glm::mat4 & MVP) {
    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_MVP", MVP);
    pRenderer->Draw(*m_Va, m_IndexCount, *m_Shader);
}

void Chunk::CreateMesh(){
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				if (m_Blocks[x][y][z].IsActive() == false) { 
					continue;
				}
				CreateCube(x, y, z);
			}
		}
	}
    m_Mesh = std::make_unique<VertexBuffer>(m_Vertecies.data(), sizeof(Vertex) * m_Vertecies.size());
    m_Va->AddBuffer(*m_Mesh, *m_Layout);
}

void Chunk::CreateCube(int x, int y, int z) {
    //Front
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+-0.5f, z+-0.5f), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f}); //0
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+-0.5f, z+-0.5f), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f}); //1
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+0.5f,  z+-0.5f), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f}); //2
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+0.5f,  z+-0.5f), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f}); //3
    //Back                                 
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+-0.5f, z+0.5f), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f }); //4
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+-0.5f, z+0.5f), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f }); //5
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+0.5f,  z+0.5f), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f }); //6
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+0.5f,  z+0.5f), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f }); //7
    //Right                                
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+-0.5f, z+-0.5f), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f }); //8
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+-0.5f, z+ 0.5f), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f }); //9
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+0.5f,  z+ 0.5f), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f }); //10
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+0.5f,  z+-0.5f), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f }); //11
    //Left                                 
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+-0.5f, z+0.5f), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f }); //12
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+-0.5f, z+-0.5f), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f }); //13
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+0.5f,  z+-0.5f), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f }); //14
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+0.5f,  z+0.5f), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f }); //15
    //Up                                   
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+0.5f,  z+-0.5f), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f }); //16
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+0.5f,  z+-0.5f), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f }); //17
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+0.5f,  z+0.5f), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f }); //18
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+0.5f,  z+0.5f), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f }); //19
    //Down                                 
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+-0.5f, z+0.5f), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f }); //20
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+-0.5f, z+0.5f), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f }); //21
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  y+-0.5f, z+-0.5f), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f }); //22
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, y+-0.5f, z+-0.5f), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f }); //23
 
    m_IndexCount += 36;
}