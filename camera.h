#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	float yaw = 0.0f;
	float pitch = 0.0f;
	float radius = 10.0f;

	float lastX = 400, lastY = 300;
	bool mouse_down = false;

public:
	float mouse_sensitivity = 0.005f;
	float scroll_sensitivity = 1.0f;

	glm::vec3 position;
	glm::mat4 view;

	Camera(float _radius);

	void update_view();
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif