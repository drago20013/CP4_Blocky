#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//world, player, camera, block, chunck

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
unsigned int NEW_SCR_WIDTH = SCR_WIDTH;
unsigned int NEW_SCR_HEIGHT = SCR_HEIGHT;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Blocky", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    unsigned int cubeIndices[] = {
        0, 1, 2, 2, 3, 0, //Front
        4, 5, 6, 6, 7, 4, //Back
        8, 9, 10, 10, 11, 8, //Right
        12, 13, 14, 14, 15, 12, //Left
        16, 17, 18, 18, 19, 16, //Up
        20, 21, 22, 22, 23, 20  //Down
    };

    VertexArray* va = new VertexArray;
    VertexBuffer* vb = new VertexBuffer(nullptr, sizeof(Vertex) * 1000);

    VertexBufferLayout layout;
    layout.Push<glm::vec3>(1);
    layout.Push<glm::vec4>(1);
    layout.Push<glm::vec2>(1);
    layout.Push<float>(1);
    va->AddBuffer(*vb, layout);

    IndexBuffer* ib = new IndexBuffer(cubeIndices, 36);

    Shader* shader = new Shader("res/shaders/Basic.shader"); // 
    shader->Bind();
    //shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    Texture* texture = new Texture("res/textures/dirt.jpg"); // Filesystem
    texture->Bind();
    shader->SetUniform1i("u_Texture", 0);

    va->Unbind();
    shader->Unbind();
    vb->Unbind();
    ib->Unbind();

    Renderer render;

    float r = 0.7f;
    float increament = 0.05f;

    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame
    float lastTime = 0.0f;
    int nbFrames = 0;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;
        nbFrames++;
        if (currentFrame - lastFrame >= 2.0) {
            printf("FPS: %d -> %f ms\n", nbFrames, 1000.0 / (float)nbFrames);
            nbFrames = 0;
            lastFrame = currentFrame;
        }

        processInput(window);

        render.Clear();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);
        proj = glm::perspective(3.1415f / 2.0f, (float)NEW_SCR_WIDTH / (float)NEW_SCR_HEIGHT, 0.1f, 100.0f);
        view = glm::lookAt(glm::vec3(0.0f, 3.5f, -1.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

        shader->Bind();
        shader->SetUniformMat4f("u_MVP", (proj * view * model));
        //shader->SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        float cubeVerticies[] = {
        -0.5f, -0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 0.0f, 1.f, //0 //Front
         0.5f, -0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 0.0f, 1.f, //1
         0.5f,  0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 1.0f, 1.f, //2
        -0.5f,  0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 1.0f, 1.f, //3
                                                            
         0.5f, -0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 0.0f, 1.f, //4 //Back
        -0.5f, -0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 0.0f, 1.f, //5
        -0.5f,  0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 1.0f, 1.f, //6
         0.5f,  0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 1.0f, 1.f, //7
                                                             
         0.5f, -0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 0.0f, 1.f, //8 //Right
         0.5f, -0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 0.0f, 1.f, //9
         0.5f,  0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 1.0f, 1.f, //10
         0.5f,  0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 1.0f, 1.f, //11
                                                             
        -0.5f, -0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 0.0f, 1.f, //12 //Left
        -0.5f, -0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 0.0f, 1.f, //13
        -0.5f,  0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 1.0f, 1.f, //14
        -0.5f,  0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 1.0f, 1.f, //15
                                                           
        -0.5f,  0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 0.0f, 1.f, //16 //Up
         0.5f,  0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 0.0f, 1.f, //17
         0.5f,  0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 1.0f, 1.f, //18
        -0.5f,  0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 1.0f, 1.f, //19
                                                           
        -0.5f, -0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 0.0f, 1.f, //20 //Down
         0.5f, -0.5f,  0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 0.0f, 1.f, //21
         0.5f, -0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 1.0f, 1.0f, 1.f, //22
        -0.5f, -0.5f, -0.5f, 1.0f, .0f, .0f, 1.f, 0.0f, 1.0f, 1.f  //23
        };

        //Set dynamic vertex buffer
        vb->Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVerticies), cubeVerticies));
        
        render.Draw(*va, *ib, *shader);
 

        if (r > 2.0f)
            increament = -0.8f;
        else if (r < 0.7f)
            increament = 0.8f;

        r += increament * deltaTime;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete shader;
    delete va;
    delete ib;
    delete vb;
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    GLCall(glViewport(0, 0, width, height));
    NEW_SCR_WIDTH = width;
    NEW_SCR_HEIGHT = height;
}