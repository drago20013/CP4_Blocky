#include "Player.h"
#include "Camera.h"

Player::Player(glm::vec3 pos, glm::vec3 dimensions, float speed) :  m_Proj(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Model(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))) {
	m_LastX = SCR_WIDTH / 2.0f;
	m_LastY = SCR_HEIGHT / 2.0f;
	m_FirstMouse = true;
    m_OnGround = false;
    m_Pos = pos;
    m_Dimensions = dimensions;
    m_Speed = speed;
    m_dPos = glm::vec3(0.0f);
    m_Cam = Camera(m_Pos, m_Speed);
}

void Player::Update() {
    m_Proj = glm::perspective(glm::radians(m_Cam.GetZoom()), aspectRatio, 0.1f, 1000.0f);
    m_View = m_Cam.GetViewMatrix();
    m_dPos.x = m_dPos.z = 0.0f;
}

void Player::SetModelM(const glm::mat4& model) {
    m_Model = model;
}

void Player::ProcessMouse(GLFWwindow* window, double& xposIn, double& yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos; // reversed since y-coordinates go from bottom to top

    m_LastX = xpos;
    m_LastY = ypos;

    m_Cam.ProcessMouseMovement(xoffset, yoffset);
}

void Player::ProcessScroll(GLFWwindow* window, double& xoffset, double& yoffset){
    m_Cam.ProcessMouseScroll(static_cast<float>(yoffset));
}

void Player::ProcessInput(GLFWwindow* window, float& deltaTime){
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && m_OnGround){
        m_dPos.y = 2.0f;
        m_OnGround = false;
        m_Cam.ProcessKeyboard(Camera_Movement::JUMP, m_dPos, deltaTime);
        m_Pos = m_Cam.GetPosition();            
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        m_dPos.z = 1.0f;
        m_Cam.ProcessKeyboard(Camera_Movement::FORWARD, m_dPos, deltaTime);
        m_Pos = m_Cam.GetPosition();            
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        m_dPos.z = 1.0f;
        m_Cam.ProcessKeyboard(Camera_Movement::BACKWARD, m_Pos, deltaTime);
        m_Pos = m_Cam.GetPosition();            
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        m_dPos.x = 1.0f;
        m_Cam.ProcessKeyboard(Camera_Movement::LEFT, m_dPos, deltaTime);
        m_Pos = m_Cam.GetPosition();            
    }
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        m_dPos.x = 1.0f;
        m_Cam.ProcessKeyboard(Camera_Movement::RIGHT, m_dPos, deltaTime);
        m_Pos = m_Cam.GetPosition();            
    }
}
