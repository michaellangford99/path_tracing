#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(float _radius)
{
	radius = _radius;
}

void Camera::update_view()
{
	float camY = sin(pitch) * radius;
	float proj_radius = cos(pitch) * radius;
	float camX = sin(yaw) * proj_radius;
	float camZ = cos(yaw) * proj_radius;
	position = glm::vec3(camX, camY, camZ);
	view = glm::lookAt(position, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

void Camera::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	xoffset *= mouse_sensitivity;
	yoffset *= mouse_sensitivity;

	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		if (mouse_down)
		{
			yaw -= xoffset;
			pitch -= yoffset;

			if (pitch > 89.0f*glm::pi<float>() / 180.0f)
				pitch = 89.0f*glm::pi<float>() / 180.0f;
			if (pitch < -89.0f*glm::pi<float>() / 180.0f)
				pitch = -89.0f*glm::pi<float>() / 180.0f;
		}
	}
}

void Camera::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mouse_down = true;
	else
		mouse_down = false;
}

void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		radius -= scroll_sensitivity*yoffset;
	}
}