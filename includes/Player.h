#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern float aspectRatio;

class Player{
public:
	Player();
	void ProcessMouse(GLFWwindow* window, double& xposIn, double& yposIn);
	void ProcessScroll(GLFWwindow* window, double& xoffset, double& yoffset);
	void ProcessInput(GLFWwindow *window, float& deltaTime);

	void Update();
	glm::mat4 GetMVP() { return (m_Proj * m_View * m_Model); }
	void SetModelM(const glm::mat4& model);

	const glm::vec3& GetPosition() const { return m_Cam.GetPosition(); }

private:
	Camera m_Cam;
	float m_LastX;
	float m_LastY;
	bool m_FirstMouse;
	glm::mat4 m_Proj;
	glm::mat4 m_View;
	glm::mat4 m_Model;
};

