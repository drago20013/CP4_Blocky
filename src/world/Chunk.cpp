#include <filesystem>
#include "Chunk.h"

extern std::filesystem::path g_WorkDir;

constexpr float LIGHT_TOP = 1.0f;
constexpr float LIGHT_X = 0.8f;
constexpr float LIGHT_Z = 0.6f;
constexpr float LIGHT_BOT = 0.4f;


Chunk::Chunk() : m_IndexCount(0), m_Va(std::make_unique<VertexArray>()), 
m_Layout(std::make_unique<VertexBufferLayout>()), m_Shader(std::make_unique<Shader>((g_WorkDir/"res/shaders/Basic.shader").string())) { // Create the blocks
    m_Blocks = new Block **[CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++) {
        m_Blocks[i] = new Block * [CHUNK_HEIGHT];
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            m_Blocks[i][j] = new Block[CHUNK_SIZE];
        }
    }

    m_Layout->Push_attrib<glm::vec3>(1); //position
    m_Layout->Push_attrib<glm::vec4>(1); //color
    m_Layout->Push_attrib<glm::vec2>(1); //texcoord
    m_Layout->Push_attrib<float>(1); //textslot
    m_Layout->Push_attrib<float>(1); //directlight

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                //(!(z%2||x%2||y%2)) ? m_Blocks[x][y][z].SetActive(true): m_Blocks[x][y][z].SetActive(false);
                m_Blocks[x][y][z].SetActive(true);
            }
        }
    }

    m_Texture = new Texture((g_WorkDir/"res/textures/dirt.png").string());
    m_Texture->Bind();
    m_Shader->Bind();
    m_Shader->SetUniform1i("u_Texture", 0);
}

Chunk::~Chunk() { // Delete the blocks
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_HEIGHT; ++j) {
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
		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				if (m_Blocks[x][y][z].IsActive() == false) { 
					continue;
				}
				CreateCube(x, y, z);
			}
		}
	}
    m_Mesh = std::make_unique<VertexBuffer>(m_Vertecies.data(), (unsigned int)(sizeof(Vertex) * m_Vertecies.size()));
    m_Va->AddBuffer(*m_Mesh, *m_Layout);
}

void Chunk::CreateCube(int x, int y, int z) {
    //Front
    m_Vertecies.push_back({ glm::vec3( 0.5f+x, -0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f, LIGHT_Z }); //1
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f, LIGHT_Z }); //2
    m_Vertecies.push_back({ glm::vec3(-0.5f+x,  0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f, LIGHT_Z }); //3
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, -0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f, LIGHT_Z }); //0
    //Back                                                   
    m_Vertecies.push_back({ glm::vec3( 0.5f+x, -0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f, LIGHT_Z }); //4
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, -0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f, LIGHT_Z }); //5
    m_Vertecies.push_back({ glm::vec3(-0.5f+x,  0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f, LIGHT_Z }); //6
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f, LIGHT_Z }); //7
    //Right                                                    
    m_Vertecies.push_back({ glm::vec3( 0.5f+x, -0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f, LIGHT_X }); //8
    m_Vertecies.push_back({ glm::vec3( 0.5f+x, -0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f, LIGHT_X }); //9
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f, LIGHT_X }); //10
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f, LIGHT_X }); //11
    //Left                                                   
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, -0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f, LIGHT_X }); //12
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, -0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f, LIGHT_X }); //13
    m_Vertecies.push_back({ glm::vec3(-0.5f+x,  0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f, LIGHT_X }); //14
    m_Vertecies.push_back({ glm::vec3(-0.5f+x,  0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f, LIGHT_X }); //15
    //Up                                                     
    m_Vertecies.push_back({ glm::vec3(-0.5f+x,  0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f, LIGHT_TOP }); //16
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f, LIGHT_TOP }); //17
    m_Vertecies.push_back({ glm::vec3( 0.5f+x,  0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f, LIGHT_TOP }); //18
    m_Vertecies.push_back({ glm::vec3(-0.5f+x,  0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f, LIGHT_TOP }); //19
    //Down                                                  
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, -0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  0.0f), 1.f, LIGHT_BOT }); //20
    m_Vertecies.push_back({ glm::vec3( 0.5f+x, -0.5f+y,  0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  0.0f), 1.f, LIGHT_BOT }); //21
    m_Vertecies.push_back({ glm::vec3( 0.5f+x, -0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(1.0f,  1.0f), 1.f, LIGHT_BOT }); //22
    m_Vertecies.push_back({ glm::vec3(-0.5f+x, -0.5f+y, -0.5f+z), glm::vec4(1.f), glm::vec2(0.0f,  1.0f), 1.f, LIGHT_BOT }); //23
 
    m_IndexCount += 36;


}
