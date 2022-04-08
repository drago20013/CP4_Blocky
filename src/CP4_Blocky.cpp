// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Player.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "WorldSegment.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
bool mouseHidden{};
bool wireFrame{};

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
float aspectRatio =
    static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
unsigned int NEW_SCR_WIDTH = SCR_WIDTH;
unsigned int NEW_SCR_HEIGHT = SCR_HEIGHT;

float deltaTime = 0.0f;  // Time between current frame and last frame
float lastTime = 0.0f;   // Time of last frame

Player player;

#ifdef _MSC_VER
std::filesystem::path g_WorkDir(
    "C:\\Users\\Michal\\source\\repos\\CP4_Blocky\\x64\\Debug");
#else
std::filesystem::path g_WorkDir("/home/drago/CLionProjects/CP4_Blocky/");
#endif

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Blocky", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    mouseHidden = 1;

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    //=======================================================
    {
        Renderer render;

        WorldSegmnet* segment = new WorldSegmnet;
        for (int i = -1; i < 1; i++)
            for (int j = -1; j < 1; j++)
                for (int x = 0; x < CHUNK_SIZE; x++)
                    for (int y = 0; y < 4; y++)
                        for (int z = 0; z < CHUNK_SIZE; z++) {
                            segment->Set(x + CHUNK_SIZE * i, y,
                                         z + CHUNK_SIZE * j, 255 / (y + 1));
                        }

        /*segment->Set(0, 0, 0, 0);
        segment->Set(0, 0, 1, 0);
        segment->Set(0, 0, 2, 0);
        segment->Set(0, 0, 3, 0);
        segment->Set(-1, 0, 3, 128);
        segment->Set(0, 0, -2, 64);*/

        float lastFrame = 0.0f;
        int nbFrames = 0;

        // render loop
        // -----------
        while (!glfwWindowShouldClose(window)) {
            float currentFrame = (float)glfwGetTime();
            deltaTime = currentFrame - lastTime;
            lastTime = currentFrame;
            nbFrames++;
            if (currentFrame - lastFrame >= 2.0) {
                printf("FPS: %d -> %f ms\n", nbFrames,
                       1000.0 / (float)nbFrames);
                nbFrames = 0;
                lastFrame = currentFrame;
            }

            processInput(window);

            render.Clear();

            segment->Render(player);

            player.Update();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        delete segment;
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (mouseHidden) player.ProcessInput(window, deltaTime);
}

// key callback listen to the key callback event instead of querying the key
// state in every frame.
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        if (!mouseHidden) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseHidden = 1;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseHidden = 0;
            player.SetFirstMouse();
        }
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        if (!wireFrame) {
            GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
            wireFrame = 1;
        } else {
            GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
            wireFrame = 0;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    GLCall(glViewport(0, 0, width, height));
    if (width && height) aspectRatio = (float)width / (float)height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (mouseHidden) player.ProcessMouse(window, xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (mouseHidden) player.ProcessScroll(window, xoffset, yoffset);
}
