// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <filesystem>
#include <future>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>

#include "Player.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "WorldSegment.h"
#include "glm/geometric.hpp"
#include "vendor/stb_image/stb_image.h"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
float aspectRatio =
    static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
unsigned int NEW_SCR_WIDTH = SCR_WIDTH;
unsigned int NEW_SCR_HEIGHT = SCR_HEIGHT;
bool mouseHidden{};
bool flyMode{true};
bool wireFrame{};
bool placeBlock{};
bool destroyBlock{};

float deltaTime = 0.0f;  // Time between current frame and last frame
float lastTime = 0.0f;   // Time of last frame

#ifdef _MSC_VER
#ifdef _DEBUG
std::filesystem::path g_WorkDir(
    "C:\\Users\\Michal\\source\\repos\\CP4_Blocky\\x64\\Debug");
#else 
std::filesystem::path g_WorkDir(std::filesystem::current_path());
#endif
#else
std::filesystem::path g_WorkDir("/home/drago/CLionProjects/CP4_Blocky/");
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

    GLFWimage images[1];
    images->pixels = stbi_load((g_WorkDir.string() + "/res/textures/ico.png").c_str(), &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    std::shared_ptr<Player> player = std::make_shared<Player>(
        glm::vec3(0.0f, 70.0f, 0.0f), glm::vec3(.4f, 1.8f, .4f), 8.0f);

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

        if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            static_cast<Player*>(glfwGetWindowUserPointer(window))
                ->ChangeBlockDown();
        }

        if (key == GLFW_KEY_E && action == GLFW_PRESS) {
            static_cast<Player*>(glfwGetWindowUserPointer(window))
                ->ChangeBlockUp();
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

    auto mouseButtonCallback = [](GLFWwindow* window, int button, int action, int mods) {
        if (mouseHidden)
        {
            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            {
                placeBlock = true;
            }
            else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            {
                destroyBlock = true;
            }
        }
    };

    glfwSetMouseButtonCallback(window, mouseButtonCallback);
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

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
   
    //=======================================================
    {
        std::unique_ptr<VertexBuffer> m_VBO = std::make_unique<VertexBuffer>();
        std::unique_ptr<VertexArray> m_VAO = std::make_unique<VertexArray>();
        std::unique_ptr<VertexBufferLayout> m_Layout = std::make_unique<VertexBufferLayout>();
        std::unique_ptr<Shader> m_CrossShader = std::make_unique<Shader>("CrossShader");
        m_Layout->PushAttrib<glm::vec2>(1);
        float m_Cross[4][2] = {
            {-0.01, 0},
            {+0.01, 0},
            {0, -0.01},
            {0, +0.01}
        };

        auto DrawCross = [&]() {
            GLCall(glDisable(GL_DEPTH_TEST));
            m_VBO->LoadData(m_Cross, sizeof(m_Cross));
            m_VAO->AddBufferf(*m_VBO, *m_Layout);
            m_CrossShader->Bind();
            m_CrossShader->SetUniform1f("u_aspect", aspectRatio);
            GLCall(glDrawArrays(GL_LINES, 0, 4));
            GLCall(glEnable(GL_DEPTH_TEST));
        };


        std::unique_ptr<Renderer> render = std::make_unique<Renderer>();
        std::unique_ptr<WorldSegment> segment = std::make_unique<WorldSegment>(player);
        
            std::unique_ptr<VertexBuffer> m_LogoVBO = std::make_unique<VertexBuffer>();
            std::unique_ptr<VertexArray> m_LogoVAO = std::make_unique<VertexArray>();
            std::unique_ptr<VertexBufferLayout> m_LogoLayout = std::make_unique<VertexBufferLayout>();
            std::unique_ptr<Shader> m_LogoShader = std::make_unique<Shader>("Simple");
            std::shared_ptr<Texture> m_Logo = std::make_unique<Texture>((g_WorkDir / "res/textures/Logo.png").string());
            m_LogoLayout->PushAttrib<glm::vec2>(1);
            m_LogoLayout->PushAttrib<glm::vec2>(1);
            glm::vec2 data[6][2] = {
                {{-1.f, -1.f}, {0.f, 0.f}},
                {{ 1.f, -1.f}, {1.f, 0.f}},
                {{-1.f,  1.f}, {0.f, 1.f}},
                {{-1.f,  1.f}, {0.f, 1.f}},
                {{ 1.f, -1.f}, {1.f, 0.f}},
                {{ 1.f,  1.f}, {1.f, 1.f}}
            };
            m_LogoVBO->LoadData(data, sizeof(data));
            m_LogoVAO->AddBufferf(*m_LogoVBO, *m_LogoLayout);
            //Display logo
            m_Logo->Bind(2);
            m_LogoShader->Bind();
            m_LogoShader->SetUniform1i("u_Texture", 2);
            m_LogoShader->SetUniform1f("u_aspect", aspectRatio);
            render->Clear();
            render->Draw(*m_LogoVAO, *m_LogoShader, 6);
            glfwSwapBuffers(window);

            segment->GenereteSegment();
            auto loadInBack = std::async(std::launch::async, [&]() {
                segment->Initialize();
                });
            printf("Loading in the background, displaying blocky\n");
                     
            loadInBack.wait();
            printf("Loaded\n"); 

            bool ShowOverlay = true;
            int corner = 1;

        // render loop
        // -----------
        while (!glfwWindowShouldClose(window)) {
            float currentFrame = (float)glfwGetTime();
            deltaTime = currentFrame - lastTime;
            lastTime = currentFrame;

            render->Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (!mouseHidden) {
                deltaTime = 0;
                m_LogoShader->Bind();
                m_LogoShader->SetUniform1f("u_aspect", aspectRatio);
                render->Draw(*m_LogoVAO, *m_LogoShader, 6);
            }
            else {
                player->ProcessMove(window, deltaTime);

                player->Update(deltaTime);

                if (!flyMode) segment->CheckCollision();

                segment->GenereteSegment();

                if (placeBlock) {
                    segment->PlaceBlock();
                    placeBlock = false;
                }

                if (destroyBlock) {
                    segment->DestroyBlock();
                    destroyBlock = false;
                }

                segment->Update();

                segment->Render();

                DrawCross();
            }

            {
                ImGuiIO& io = ImGui::GetIO();
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
                if (corner != -1)
                {
                    const float PAD = 10.0f;
                    const ImGuiViewport* viewport = ImGui::GetMainViewport();
                    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
                    ImVec2 work_size = viewport->WorkSize;
                    ImVec2 window_pos, window_pos_pivot;
                    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
                    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
                    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
                    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
                    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                    window_flags |= ImGuiWindowFlags_NoMove;
                }
                ImGui::SetNextWindowBgAlpha(0.35f); 
                if (ImGui::Begin("Info", &ShowOverlay, window_flags))
                {
                    ImGui::Text("Application average %.3f ms/frame \n(%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                    ImGui::Separator();
                    ImGui::Text("Player position:\nx: %d y: %d, z: %d", (int)player->GetPosition().x, (int)player->GetPosition().y, (int)player->GetPosition().z);
                    ImGui::Text("Chosen block: %s", player->GetEquippedString().c_str());
                    if (!mouseHidden && ImGui::BeginPopupContextWindow())
                    {
                        if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
                        if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
                        if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
                        if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
                        if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
                        if (ShowOverlay && ImGui::MenuItem("Close")) ShowOverlay = false;
                        ImGui::EndPopup();
                    }
                }
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);


            glfwPollEvents();
        }
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
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
