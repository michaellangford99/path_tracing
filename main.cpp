#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <map>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

#include "shader.h"
#include "camera.h"
#include "fullscreen_quad.h"
#include "ray_trace.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

/*float cube_vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};*/

//All globals

Camera camera(10.0f);
Shader ourShader;

unsigned int test_generate_image()
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width = 500;
	int height = 500;
	int nrChannels = 3;

	unsigned char *data = (unsigned char*)malloc(width*height * 3);

	for (int i = 0; i < height; i++)
	{
		data[i*nrChannels*width + i * nrChannels] = 255;
		data[i*nrChannels*width + i * nrChannels+1] = 255;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	free(data);

	return texture;
}

unsigned int load_and_bind_texture(std::string path)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	/*for (int i = 0; i < height; i++)
	{
		data[i*nrChannels*width + i * nrChannels] = 255;
		data[i*nrChannels*width + i * nrChannels+1] = 255;
	}*/

	if (data)
	{
		unsigned int rbg_rgba = path.substr(path.length() - (3), 3) == "png" ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, rbg_rgba, width, height, 0, rbg_rgba, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return NULL;
	}
	stbi_image_free(data);

	return texture;
}

GLFWwindow* create_glfw_window(int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return NULL;
	}

	glViewport(0, 0, width, height);
	return window;
}

void setup_camera(GLFWwindow* window)
{
	//setup camera and camera control
	camera = Camera(10.0f);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void setup_imgui(GLFWwindow* window)
{
	//
	//Initialize ImGUI
	//
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void layout_draw_imgui()
{
	// Tell OpenGL a new frame is about to begin
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	// ImGUI window creation
	ImGui::Begin("My name is window, ImGUI window");
	// Text that appears in the window
	ImGui::Text("Hello there adventurer!");
	// Checkbox that appears in the window
	//ImGui::Checkbox("Draw Triangle", &drawTriangle);
	// Slider that appears in the window
	//ImGui::SliderFloat("greenValue", &greenValue, 0.0f, 1.0f);
	//ImGui::SliderFloat("radius", &radius, 1.0f, 20.0f);

	//ImGui::SliderFloat("mouse_sensitivity", &(camera.mouse_sensitivity), 0.001f, 0.01f);
	//ImGui::SliderFloat("scroll_sensitivity", &(camera.scroll_sensitivity), 0.2f, 2.0f);

	//ImGui::SliderFloat3("position", glm::value_ptr(position), -2, 2);

	/*ImGui::BeginMenuBar();
	ImGui::BeginMenu("test menu", true);
	//ImGui::SliderAngle("angle", &test_angle, -180.0f, 180.0f);
	ImGui::EndMenu();
	ImGui::EndMenuBar();*/

	//ImGui::CollapsingHeader("Layout & Scrolling");
	//if (ImGui::CollapsingHeader("Layout & Scrolling"))
	//{
	//	if (ImGui::TreeNode("Child windows"))
	//	{
			ourShader.generate_imgui_editor();

	//		ImGui::TreePop();
	//	}
	//}
	//ImGui::TreeNode("Child windows");
	//ImGui::TreeNode("Child windows");
	//ImGui::TreePop();


	// Fancy color editor that appears in the window
	//ImGui::ColorEdit4("sunrise_color", sunrise_color);
	//ImGui::ColorEdit4("sunset_color", sunset_color);
	// Ends the window
	ImGui::End();

	ImGui::ShowDemoWindow();
	ImGui::ShowDebugLogWindow();

	// Renders the ImGUI elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main()
{
	GLFWwindow* window = create_glfw_window(800, 600);
	if (window == NULL) return -1;

	setup_camera(window);

	//setup shaders
	ourShader = Shader("vertex.glsl", "fragment.glsl");
	ourShader.generate_uniform_table();

	//load and bind texture
	//unsigned texture0 = test_generate_image();//load_and_bind_texture("content/container.jpg");
	//unsigned texture1 = load_and_bind_texture("content/awesomeface.png");
	
	//load geometry
	fullscreen_quad_init();
	
	//setup imgui
	setup_imgui(window);

	int img_width = 80*5;
	int img_height = 60*5;

	unsigned char* data = allocate_image_memory(img_width, img_height);
	unsigned int texture0 = bind_image();

	std::vector<sphere_t> scene = generate_test_scene();

	//
	// bind the unindexed vertex array for the cube
	//
	//CUBE==============================
	/*unsigned int cube_VBO, cube_VAO;
	glGenVertexArrays(1, &cube_VAO);
	glGenBuffers(1, &cube_VBO);

	glBindVertexArray(cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	
	//each vertext contains vec3 pos, vec2 texcoord
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);*/

	while (!glfwWindowShouldClose(window))
	{
		//check key and mouse input
		processInput(window);


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//bind / apply shader?
		ourShader.use();

		//generate transformation matrices
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, position);

		//update view and pull view matrix out
		camera.update_view();
		glm::mat4 view = camera.view;

		int viewport_size[4];
		glGetIntegerv(GL_VIEWPORT, viewport_size);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), (float)viewport_size [2] / (float)viewport_size[3], 0.1f, 100.0f);
		
		//Ray trace:
		demo_trace(data, img_width, img_height, scene, camera.view, projection, camera.position, 1000);
		texture0 = bind_image_memory(texture0, data, img_width, img_height);

		//set auto-edited uniforms.
		//afterwards reset those that are supposed to be set internally
		ourShader.set_imgui_uniforms();

		glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "inv_view_projection"), 1, GL_FALSE, glm::value_ptr(glm::inverse(projection * view)));
		glUniform3f(glGetUniformLocation(ourShader.ID, "camera_position"), camera.position.x, camera.position.y, camera.position.z);
		//glUniform1f(glGetUniformLocation(ourShader.ID, "eps"), shader_epsilon);
		//glUniform3f(glGetUniformLocation(ourShader.ID, "spos"), sphere_position.x, sphere_position.y, sphere_position.z);


		//only needs to happen once but hey whatever
		glUniform1i(glGetUniformLocation(ourShader.ID, "texture0"), 0);//set texture0 sampler to grab texture 0
		//glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 1);//set texture1 sampler to grab texture 1

		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture1);

		fullscreen_quad_draw();

		layout_draw_imgui();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.mouse_callback(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	camera.mouse_button_callback(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.scroll_callback(window, xoffset, yoffset);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}