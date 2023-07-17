#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stddef.h>
#include <map>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct uniform_descriptor {
		GLenum uniform_type;
		std::string  uniform_name;
		GLsizei uniform_size;
		//void* local_value_addr;
		int local_int;
		float local_float;
		glm::vec2 local_float2;
		glm::vec3 local_float3;
		glm::vec4 local_float4;
		glm::mat3x3 local_float4x4;
		glm::mat3x3 local_float3x3;

		float min;
		float max;
};

class Shader
{
public:
	// the program ID
	unsigned int ID;

	std::map<GLenum, std::string> uniform_types;
	//std::map<GLenum, size_t size> uniform_syzes;
	std::vector<struct uniform_descriptor> uniform_list;

	// constructor reads and builds the shader
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

	//generate imgui editor
	void generate_uniform_table();
	void generate_imgui_editor();
	void set_imgui_uniforms();
};

#endif