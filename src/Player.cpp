#include "Player.h"

#include <iostream>

#include "Camera.h"
#include "glm/geometric.hpp"

extern bool flyMode;

Player::Player(glm::vec3 pos, glm::vec3 dimensions, float speed)
    : m_Model(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))) {
    m_LastX = SCR_WIDTH * 0.5f;
    m_LastY = SCR_HEIGHT * 0.5f;
    m_FirstMouse = true;
    m_OnGround = false;
    m_Pos = pos;
    m_LastPos = pos;
    m_Dimensions = dimensions;
    m_CamPos = glm::vec3(0.0f, m_Dimensions.y - 0.5f, 0.0f);
    m_Speed = speed;
    m_Acc = glm::vec3(0.0f);
    m_Vel = glm::vec3(0.0f);
    m_Cam = Camera(m_Pos + m_CamPos, m_Speed);
    m_Gravity = -15.0f;
    m_Proj = glm::perspective(glm::radians(m_Cam.GetZoom()), aspectRatio, 0.1f,
                              1000.0f);
    m_View = m_Cam.GetViewMatrix();
    m_Equipped = BlockType::Dirt;
}

void Player::SetModelM(const glm::mat4& model) { m_Model = model; }

const BlockType Player::GetEquipped() const
{
    return m_Equipped;
}

const std::string Player::GetEquippedString() const
{
    std::string blockName{};
    switch ((int)m_Equipped) {
    case 0:
        blockName = "Grass";
        break;
    case 1:
        blockName = "Dirt";
        break;
    case 3:
        blockName = "Stone";
        break;
    case 4:
        blockName = "OakWood";
        break;
    case 5:
        blockName = "Sand";
        break;
    case 6:
        blockName = "OakLeaf";
        break;
    case 7:
        blockName = "Brick";
        break;
    case 8:
        blockName = "OakPlank";
        break;
    case 9:
        blockName = "Glass";
        break;
    case 10:
        blockName = "BirchWood";
        break;
    case 11:
        blockName = "BirchLeaf";
        break;
    case 12:
        blockName = "BirchPlank";
        break;
    case 13:
        blockName = "Pumpkin";
        break;
    }
    return blockName;
}

void Player::SetPosition(glm::vec3 newPos) {
    m_Pos = newPos;
    m_Cam.SetPosition(m_Pos + m_CamPos);
}

void Player::SetCamPosition(glm::vec3 newPos) { m_CamPos = newPos; }

void Player::ChangeBlockUp()
{
    int i = (int)m_Equipped;
    if (i == 1)
        i = 3;
    else if (i == 13)
        i = 0;
    else i++;
    
    m_Equipped = (BlockType)i;
}

void Player::ChangeBlockDown()
{
    int i = (int)m_Equipped;
    if (i == 3)
        i = 1;
    else if (i == 0)
        i = 13;
    else i--;
    
    m_Equipped = (BlockType)i;
}

void Player::ProcessMouse(GLFWwindow* window, double& xposIn, double& yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_FirstMouse) {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float xoffset = xpos - m_LastX;
    float yoffset =
        m_LastY - ypos;  // reversed since y-coordinates go from bottom to top

    m_LastX = xpos;
    m_LastY = ypos;

    m_Cam.ProcessMouseMovement(xoffset, yoffset);
}

void Player::ProcessScroll(GLFWwindow* window, double& yoffset) {
    m_Cam.ProcessMouseScroll(static_cast<float>(yoffset));
}

void Player::Update(float& deltaTime) {
    m_Proj = glm::perspective(glm::radians(m_Cam.GetZoom()), aspectRatio, 0.1f,
                              1000.0f);
    m_View = m_Cam.GetViewMatrix();

    m_LastPos = m_Pos;

    if (!flyMode && !m_OnGround) {
        m_Acc.y += m_Gravity * 1.f * deltaTime;
    }

    float r = glm::abs(m_Acc.x);
    if (r < glm::abs(m_Acc.y)) r = glm::abs(m_Acc.y);
    if (r < glm::abs(m_Acc.z)) r = glm::abs(m_Acc.z);

    if (glm::length(m_Acc) > 0) {
        m_Acc = glm::normalize(m_Acc) * r;
    }

    m_Vel += m_Acc;

    glm::vec3 friction =
        !flyMode ? 0.2f * glm::vec3(-m_Vel.x, 0.0f, -m_Vel.z) : 0.2f * -m_Vel;

    m_Vel += friction;

    m_Acc = glm::vec3(0);

    if (glm::length(m_Vel) > m_Speed) {
        glm::vec3 tmp = glm::normalize(m_Vel) * m_Speed; 
        m_Vel.y = m_Vel.y > m_Speed ? m_Speed : m_Vel.y;
        m_Vel.y = m_Vel.y < -m_Speed ? -m_Speed : m_Vel.y;
        m_Vel = glm::vec3(tmp.x, m_Vel.y, tmp.z);
    }

    m_Pos += m_Cam.GetForward() * m_Vel.z * deltaTime;
    m_Pos += m_Cam.GetRight() * m_Vel.x * deltaTime;
    m_Pos += m_Cam.GetWorldUp() * m_Vel.y * deltaTime;

    m_Cam.SetPosition(m_Pos + m_CamPos);

    m_OnGround = false;
}

void Player::ProcessMove(GLFWwindow* window, float& deltaTime) {
    //=================================
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_Acc.z += 600.f * deltaTime;
    }

    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_Acc.z -= 600.f * deltaTime;
    }
    //=================================
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_Acc.x -= 600.f * deltaTime;
    }

    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_Acc.x += 600.f * deltaTime;
    }

    if (!flyMode && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS &&
        m_OnGround) {
        m_Acc.y += 400.0f * deltaTime;
    }

    else if (!flyMode &&
             glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
             m_OnGround) {
        m_Vel.x *= 0.1f;
        m_Vel.z *= 0.1f;
    }

    else if (flyMode && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        m_Acc.y -= 600.f * deltaTime;
    }

    else if (flyMode && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        m_Acc.y += 600.f * deltaTime;
    }
}
