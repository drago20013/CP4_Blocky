#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "Camera.h"

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern float aspectRatio;

enum class Collision {
    NONE = 0,
    RIGHT,
    LEFT,
    FRONT,
    BACK
};

class Player {
public:

    Player(glm::vec3 pos = glm::vec3(0.0f, 70.0f, 0.0f), glm::vec3 dimensions = glm::vec3(0.8f, 1.8f, 0.8f), float speed = 10.0f);
    
    void ProcessMouse(GLFWwindow* window, double& xposIn, double& yposIn);
    void ProcessScroll(GLFWwindow* window, double& yoffset);
    void ProcessMove(GLFWwindow* window, float& deltaTime);

    void Update(float& deltaTime);

    glm::mat4 GetMVP() { return (m_Proj * m_View * m_Model); }
    void SetModelM(const glm::mat4& model);
    void SetFirstMouse() {
        m_FirstMouse = true;
        m_LastX = SCR_WIDTH / 2.0f;
        m_LastY = SCR_HEIGHT / 2.0f;
    }

    const glm::vec3& GetPosition() const { return m_Pos; }
    /*const glm::vec3& GetRightVec() const { return m_Cam.GetRight(); }
    const glm::vec3& GetForwardVec() const { return m_Cam.GetForward(); }*/
    const glm::vec3& GetDeltaPosition() const { return m_dPos; }
    const glm::vec3& GetDimensions() const { return m_Dimensions; }

    void SetDeltaPosition(glm::vec3 newDeltaPos) { m_dPos = newDeltaPos; }
    void SetPosition(glm::vec3 newPos);
    void SetCamPosition(glm::vec3 newPos);
    void SetOnGround(bool activeLevel) { m_OnGround = activeLevel; }
    void SetCollision(Collision activeCollision) { m_Collision = activeCollision; }

private:
    glm::vec3 m_Pos;
    glm::vec3 m_dPos;
    glm::vec3 m_Dimensions;

    bool m_SpacePressed;
    Collision m_Collision;

    bool m_OnGround;
    float m_Speed;
    float m_Gravity;

    Camera m_Cam;
    glm::vec3 m_CamPos;
    float m_LastX;
    float m_LastY;
    bool m_FirstMouse;

    glm::mat4 m_Proj;
    glm::mat4 m_View;
    glm::mat4 m_Model;
};

