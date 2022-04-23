#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "Camera.h"

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern float aspectRatio;

class Player {
public:
    Player(glm::vec3 pos = glm::vec3(0.0f, 128.0f, 0.0f), glm::vec3 dimensions = glm::vec3(1.0f, 2.0f, 1.0f), float speed = 10.0f);
    void ProcessMouse(GLFWwindow* window, double& xposIn, double& yposIn);
    void ProcessScroll(GLFWwindow* window, double& xoffset, double& yoffset);
    void ProcessInput(GLFWwindow* window, float& deltaTime);

    void Update();
    glm::mat4 GetMVP() { return (m_Proj * m_View * m_Model); }
    void SetModelM(const glm::mat4& model);
    void SetFirstMouse() {
        m_FirstMouse = true;
        m_LastX = SCR_WIDTH / 2.0f;
        m_LastY = SCR_HEIGHT / 2.0f;
    }

    const glm::vec3& GetPosition() const { return m_Cam.GetPosition(); }

private:
    glm::vec3 m_Pos;
    glm::vec3 m_dPos;
    glm::vec3 m_Dimensions;
    bool m_OnGround;
    float m_Speed;
    Camera m_Cam;
    float m_LastX;
    float m_LastY;
    bool m_FirstMouse;
    glm::mat4 m_Proj;
    glm::mat4 m_View;
    glm::mat4 m_Model;
};

