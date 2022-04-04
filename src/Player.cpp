#include "Player.h"

Player::Player() : m_Cam(glm::vec3(7.5f, 128.0f, -3.0f)), m_Proj(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Model(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))) {
	m_LastX = SCR_WIDTH / 2.0f;
	m_LastY = SCR_HEIGHT / 2.0f;
	m_FirstMouse = true;
}

void Player::Update() {
    m_Proj = glm::perspective(glm::radians(m_Cam.Zoom), aspectRatio, 0.1f, 100.0f);
    m_View = m_Cam.GetViewMatrix();
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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_Cam.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_Cam.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_Cam.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_Cam.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}
