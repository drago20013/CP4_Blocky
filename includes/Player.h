#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <string>
#include "Camera.h"
#include "Block.h"

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern float aspectRatio;

class Player {
public:
    Player(glm::vec3 pos = glm::vec3(0.0f, 66.0f, 0.0f),
           glm::vec3 dimensions = glm::vec3(0.4f, 1.8f, 0.4f),
           float speed = 10.0f);

    void ProcessMouse(GLFWwindow* window, double& xposIn, double& yposIn);
    void ProcessScroll(GLFWwindow* window, double& yoffset);
    void ProcessMove(GLFWwindow* window, float& deltaTime);

    void Update(float& deltaTime);

    glm::mat4 GetMVP() { return (m_Proj * m_View * m_Model); }
    glm::mat4 GetProjection() const { return m_Proj; }
    glm::mat4 GetView() const { return m_View; }
    glm::vec3 GetLookAt() const { return m_Cam.GetLookAt(); }
    void SetModelM(const glm::mat4& model);
    void SetFirstMouse() {
        m_FirstMouse = true;
        m_LastX = SCR_WIDTH * 0.5f;
        m_LastY = SCR_HEIGHT * 0.5f;
    }

    const glm::vec3& GetPosition() const { return m_Pos; }
    const glm::vec3 GetCamPosition() const { return m_Pos+m_CamPos; }
    const glm::vec3& GetLastPosition() const { return m_LastPos; }
    const glm::vec3 GetForwardVec() const { return m_Cam.GetForward(); }
    const glm::vec3& GetDimensions() const { return m_Dimensions; }
    const glm::vec3& GetVelocity() const { return m_Vel; }
    const BlockType GetEquipped() const;
    const std::string GetEquippedString() const;

    void SetDeltaPosition(glm::vec3 newDeltaPos) { m_Acc = newDeltaPos; }
    void SetPosition(glm::vec3 newPos);
    void SetVelocityX(float newVelX) { m_Vel.x = newVelX; };
    void SetVelocityY(float newVelY) { m_Vel.y = newVelY; };
    void SetVelocityZ(float newVelZ) { m_Vel.z = newVelZ; };
    void SetCamPosition(glm::vec3 newPos);
    void SetOnGround(bool activeLevel) { m_OnGround = activeLevel; }
    void ChangeBlockUp();
    void ChangeBlockDown();

private:
    glm::vec3 m_Pos;
    glm::vec3 m_LastPos;
    glm::vec3 m_Acc;
    glm::vec3 m_Vel;
    glm::vec3 m_Dimensions;


    bool m_OnGround;
    float m_Speed;
    float m_Gravity;
    BlockType m_Equipped;

    Camera m_Cam;
    glm::vec3 m_CamPos;
    float m_LastX;
    float m_LastY;
    bool m_FirstMouse;

    glm::mat4 m_Proj;
    glm::mat4 m_View;
    glm::mat4 m_Model;
};

