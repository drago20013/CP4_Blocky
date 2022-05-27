#include "Player.h"

#include "Camera.h"

extern bool flyMode;

Player::Player(glm::vec3 pos, glm::vec3 dimensions, float speed)
    : m_Model(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))) {
    m_LastX = SCR_WIDTH / 2.0f;
    m_LastY = SCR_HEIGHT / 2.0f;
    m_FirstMouse = true;
    m_OnGround = false;
    m_deltaPos = glm::vec3(0);
    m_Pos = pos;
    m_LastPos = pos;
    m_Dimensions = dimensions;
    m_CamPos = glm::vec3(0.0f, m_Dimensions.y - 0.3f, 0.0f);
    m_Speed = speed;
    m_Acc = glm::vec3(0.0f);
    m_Vel = glm::vec3(0.0f);
    m_Cam = Camera(m_Pos + m_CamPos, m_Speed);
    m_Gravity = -30.0f;
    m_SpacePressed = false;
    m_Collisions[(int)Collision::GROUND] = false;
    m_Proj = glm::perspective(glm::radians(m_Cam.GetZoom()), aspectRatio, 0.1f,
                              1000.0f);
    m_View = m_Cam.GetViewMatrix();
}

void Player::SetModelM(const glm::mat4& model) { m_Model = model; }

void Player::SetPosition(glm::vec3 newPos) {
    m_Pos = newPos;
    m_Cam.SetPosition(m_Pos + m_CamPos);
}

void Player::SetCamPosition(glm::vec3 newPos) { m_CamPos = newPos; }

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

//TODO (drago): Redesign, adds acceleration and calculates velocity
void Player::Update(float& deltaTime) {
    m_Proj = glm::perspective(glm::radians(m_Cam.GetZoom()), aspectRatio, 0.1f,
                              1000.0f);
    m_View = m_Cam.GetViewMatrix();

    m_LastPos = m_Pos;

    //friction

  /*  if (flyMode)
        if (m_dAcc.y < -0.8f)
            m_dAcc.y += 50 * deltaTime;
        else if (m_dAcc.y > 0.8f)
            m_dAcc.y -= 50 * deltaTime;
        else
            m_dAcc.y = 0;*/
    if (!m_OnGround) {
        m_Acc.y += m_Gravity * 1 * deltaTime;
    }

    m_Vel += m_Acc;
    //TODO (drago): fucking gravity is destroying sth with detection
    
    if (m_OnGround) {
        glm::vec3 friction = 10.f * glm::vec3(-m_Vel.x * 100 * m_Gravity, 0.0f, -m_Vel.z * 100 * m_Gravity);
        m_Vel += friction;
    }

    m_Acc = glm::vec3(0);

    m_Pos += m_Vel * deltaTime;
    m_Cam.SetPosition(m_Pos + m_CamPos);

    m_OnGround = false;

    for (auto& col : m_Collisions) {
        col = false;
    }
}

void Player::Move(float& deltaTime) {
   /* if (m_Collisions[(int)Collision::GROUND] ||
        m_Collisions[(int)Collision::UP]) {
        m_deltaPos.y = 0;
        m_Acc.y = 0;
    }
    if (m_Collisions[(int)Collision::FRONT] ||
        m_Collisions[(int)Collision::BACK]) {
        m_deltaPos.z = 0;
        m_Acc.z = 0;
    }
    if (m_Collisions[(int)Collision::LEFT] ||
        m_Collisions[(int)Collision::RIGHT]) {
        m_deltaPos.x = 0;
        m_Acc.x = 0;
    }

    if (glm::length(m_Vel) > m_Speed) {
        m_Vel = glm::normalize(m_Vel) * m_Speed;
    }

    m_Pos += m_Vel * deltaTime;
    m_Cam.SetPosition(m_Pos + m_CamPos);*/
}

//TODO (drago): Redesign, ProcessMove adds acceleration
void Player::ProcessMove(GLFWwindow* window, float& deltaTime) {
    if (!flyMode && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS &&
        m_OnGround) {
        m_Acc.y += 80.f * deltaTime;
        if (m_Vel.y == m_Speed * .8f) {
            m_OnGround = false;
        }
    }

    else if (!flyMode &&
             glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
             m_OnGround) {
        m_Acc.x *= 0.75f;
        m_Acc.z *= 0.75f;
    }

    else if (flyMode && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        m_Acc.y -= 10.f * deltaTime;
    }

    else if (flyMode && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        m_Acc.y += 10.f * deltaTime;
    }

    /*else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        m_OnGround = false;
    }*/

    //=================================
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_Acc.z += 10.f * deltaTime;
    }

    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_Acc.z -= 10.f * deltaTime;
    }
    //=================================
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_Acc.x -= 10.f * deltaTime;
    }

    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_Acc.x += 10.f * deltaTime;
    }
}
