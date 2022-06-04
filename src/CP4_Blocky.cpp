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
#include "glm/geometric.hpp"

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
float aspectRatio =
    static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
unsigned int NEW_SCR_WIDTH = SCR_WIDTH;
unsigned int NEW_SCR_HEIGHT = SCR_HEIGHT;
bool mouseHidden{};
bool flyMode{};
bool wireFrame{};

float deltaTime = 0.0f;  // Time between current frame and last frame
float lastTime = 0.0f;   // Time of last frame

#ifdef _MSC_VER
std::filesystem::path g_WorkDir(
    "C:\\Users\\Michal\\source\\repos\\CP4_Blocky\\x64\\Debug");
#else
std::filesystem::path g_WorkDir("/home/drago/CLionProjects/CP4_Blocky/");
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

    std::shared_ptr<Player> player = std::make_shared<Player>(
        glm::vec3(0.0f, 66.0f, 0.0f), glm::vec3(0.4f, 1.8f, 0.4f), 10.0f);

    glfwSetWindowUserPointer(window, player.get());

    auto playerScrollCallback = [](GLFWwindow* window, double xoffset,
                                   double yoffset) {
        if (mouseHidden)
            static_cast<Player*>(glfwGetWindowUserPointer(window))
                ->ProcessScroll(window, yoffset);
    };

    auto playerMouseCallback = [](GLFWwindow* window, double xposIn,
                                  double yposIn) {
        if (mouseHidden)
            static_cast<Player*>(glfwGetWindowUserPointer(window))
                ->ProcessMouse(window, xposIn, yposIn);
    };

    auto keyCallback = [](GLFWwindow* window, int key, int scancode, int action,
                          int mods) {
        if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
            if (!mouseHidden) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                mouseHidden = true;
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                mouseHidden = false;
                static_cast<Player*>(glfwGetWindowUserPointer(window))
                    ->SetFirstMouse();
            }
        }

        if (key == GLFW_KEY_X && action == GLFW_PRESS) {
            if (!wireFrame) {
                GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                wireFrame = true;
            } else {
                GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
                wireFrame = false;
            }
        }

        if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
            if (!flyMode) {
                flyMode = true;
            } else {
                flyMode = false;
            }
        }
    };

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, playerMouseCallback);
    glfwSetScrollCallback(window, playerScrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    //=======================================================

    {
        std::unique_ptr<Renderer> render = std::make_unique<Renderer>();

        std::unique_ptr<WorldSegmnet> segment =
            std::make_unique<WorldSegmnet>(player);

        for (int x = -65; x < 65; x++)
            for (int y = 0; y < CHUNK_HEIGHT / 2; y++)
                for (int z = -65; z < 65; z++) {
                    segment->Set(x, y, z, BlockType::BlockType_Grass);
                    segment->SetActive(x, y, z, true);
                }
        // segment->Set(0, 0, 0, BlockType::BlockType_Grass);
        // segment->SetActive(0, 0, 0, true);

        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = -63; z < 64; z++) {
                segment->Set(63, y, z, BlockType::BlockType_Grass);
                segment->SetActive(63, y, z, true);
                segment->Set(z, y, 63, BlockType::BlockType_Grass);
                segment->SetActive(z, y, 63, true);
            }
        }

        segment->Set(0, 66, 62, BlockType::BlockType_Grass);
        segment->SetActive(0, 66, 62, true);
        segment->Set(1, 66, 62, BlockType::BlockType_Grass);
        segment->SetActive(1, 66, 62, true);
        segment->Set(2, 66, 62, BlockType::BlockType_Grass);
        segment->SetActive(2, 66, 62, true);

        segment->Set(0, 64, -2, BlockType::BlockType_Grass);
        segment->SetActive(0, 64, -2, true);

        segment->Set(0, 65, -1, BlockType::BlockType_Grass);
        segment->SetActive(0, 65, -1, true);

        segment->Set(0, 66, 0, BlockType::BlockType_Grass);
        segment->SetActive(0, 66, 0, true);

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
                printf("FPS: %d -> %f ms\n", (int)(nbFrames * 0.5f),
                       2000.0 / (float)nbFrames);
                printf("Player position: x:%f y:%f z:%f\n",
                       player->GetPosition().x, player->GetPosition().y,
                       player->GetPosition().z);
                printf("Player velocity: x:%f y:%f z:%f\nMagnitude: %f\n",
                       player->GetVelocity().x, player->GetVelocity().y,
                       player->GetVelocity().z,
                       glm::length(player->GetVelocity()));
                nbFrames = 0;
                lastFrame = currentFrame;
            }

            render->Clear();

            if (mouseHidden) {
                player->ProcessMove(window, deltaTime);

                player->Update(deltaTime);

                segment->CheckCollision();

                segment->Update();
            }
            segment->Render();

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    GLCall(glViewport(0, 0, width, height));
    if (width && height) aspectRatio = (float)width / (float)height;
}
