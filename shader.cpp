#include "shader.h"

Shader::Shader()
{
	//just chill, it's useless
}

void Shader::generate_imgui_editor()
{
	ImGui::SeparatorText("Shader Parameters");
	//ImGui::SliderFloat("shader_epsilon", &shader_epsilon, 0.00000f, 1.0f);
	//ImGui::SliderFloat3("sphere position", glm::value_ptr(sphere_position), -5, 5);
	//ImGui::SliderAngle("angle", &test_angle, -180.0f, 180.0f);
	//ImGui::SliderAngle("angle", &test_angle, -180.0f, 180.0f);
	ImGui::BeginTable("", 3);
	for (int i = 0; i < uniform_list.size(); i++)
	{
		/*
			GL_INT] = _CR
			GL_FLOAT] = _
			GL_FLOAT_VEC2
			GL_FLOAT_VEC3
			GL_FLOAT_VEC4
			--> useless in my opinion GL_FLOAT_MAT4
			*/
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		//ImGui::PushItemWidth(ImGui::CalcItemWidth()/2);
		//ImGui::AlignTextToFramePadding();
		//ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

		if ((uniform_list[i].uniform_name.find("color", 0) != -1) && ((uniform_list[i].uniform_type == GL_FLOAT_VEC3) || (uniform_list[i].uniform_type == GL_FLOAT_VEC4)))
		{
			//need a color picker
			if (uniform_list[i].uniform_type == GL_FLOAT_VEC3) ImGui::ColorEdit3(uniform_list[i].uniform_name.c_str(), glm::value_ptr(uniform_list[i].local_float3));
			if (uniform_list[i].uniform_type == GL_FLOAT_VEC4) ImGui::ColorEdit4(uniform_list[i].uniform_name.c_str(), glm::value_ptr(uniform_list[i].local_float4));
		}
		else
		{
			switch (uniform_list[i].uniform_type)
			{
			case GL_INT:   ImGui::SliderInt(uniform_list[i].uniform_name.c_str(), &uniform_list[i].local_int, uniform_list[i].min, uniform_list[i].max); break;
			case GL_FLOAT: ImGui::SliderFloat(uniform_list[i].uniform_name.c_str(), &uniform_list[i].local_float, uniform_list[i].min, uniform_list[i].max); break;
			case GL_FLOAT_VEC2: ImGui::SliderFloat2(uniform_list[i].uniform_name.c_str(), glm::value_ptr(uniform_list[i].local_float2), uniform_list[i].min, uniform_list[i].max); break;
			case GL_FLOAT_VEC3: ImGui::SliderFloat3(uniform_list[i].uniform_name.c_str(), glm::value_ptr(uniform_list[i].local_float3), uniform_list[i].min, uniform_list[i].max); break;
			case GL_FLOAT_VEC4: ImGui::SliderFloat4(uniform_list[i].uniform_name.c_str(), glm::value_ptr(uniform_list[i].local_float4), uniform_list[i].min, uniform_list[i].max); break;
			default: break;
			}

			//ImGui::SameLine();

			ImGui::TableSetColumnIndex(1);

			std::stringstream ssm;
			ssm << "min" << i;

			std::stringstream ssx;
			ssx << "max" << i;


			switch (uniform_list[i].uniform_type)
			{
			case GL_INT:
			case GL_FLOAT:
			case GL_FLOAT_VEC2:
			case GL_FLOAT_VEC3:
			case GL_FLOAT_VEC4: ImGui::InputFloat(ssm.str().c_str(), &uniform_list[i].min, 1); ImGui::TableSetColumnIndex(2); ImGui::InputFloat(ssx.str().c_str(), &uniform_list[i].max, 1); break;
			default: break;
			}
		}
	}

	ImGui::EndTable();
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// similiar for Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::generate_uniform_table()
{
	GLint uniform_count = 0;
	glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniform_count);

	std::cout << "# Active uniforms: " << uniform_count << "\n";

	/*uniform_types[GL_INT] = _CRT_STRINGIZE_(GL_INT);
	uniform_types[GL_FLOAT] = _CRT_STRINGIZE_(GL_FLOAT);
	uniform_types[GL_FLOAT_VEC2] = _CRT_STRINGIZE_(GL_FLOAT_VEC2);
	uniform_types[GL_FLOAT_VEC3] = _CRT_STRINGIZE_(GL_FLOAT_VEC3);
	uniform_types[GL_FLOAT_VEC4] = _CRT_STRINGIZE_(GL_FLOAT);
	uniform_types[GL_FLOAT_MAT4] = _CRT_STRINGIZE_(GL_FLOAT_MAT4);

		GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C*/

	for (int i = 0; i < uniform_count; i++)
	{
		char info_string[20];
		GLsizei actual_length;
		GLsizei actual_size;
		GLenum gl_type;
		glGetActiveUniform(ID, i, 20, &actual_length, &actual_size, &gl_type, info_string);
		std::string uniform_name = std::string(info_string, actual_length);
		std::cout << "Uniform - " << uniform_name << "\n";
		std::cout << "size - " << actual_size << "\n";
		std::cout << "type - " << gl_type << "\n";

		struct uniform_descriptor desc = { gl_type, uniform_name, actual_size /*intentionally skip local variable initialization*/ };

		desc.min = -10;
		desc.max = 10;

		uniform_list.push_back(desc);
	}
}

void Shader::set_imgui_uniforms()
{
	for (int i = 0; i < uniform_list.size(); i++)
	{
		switch (uniform_list[i].uniform_type)
		{
		case GL_INT:        glUniform1i(glGetUniformLocation(ID, uniform_list[i].uniform_name.c_str()), uniform_list[i].local_int);   break;
		case GL_FLOAT:      glUniform1f(glGetUniformLocation(ID, uniform_list[i].uniform_name.c_str()), uniform_list[i].local_float); break;
		case GL_FLOAT_VEC2: glUniform2f(glGetUniformLocation(ID, uniform_list[i].uniform_name.c_str()), uniform_list[i].local_float2.x, uniform_list[i].local_float2.y); break;
		case GL_FLOAT_VEC3: glUniform3f(glGetUniformLocation(ID, uniform_list[i].uniform_name.c_str()), uniform_list[i].local_float3.x, uniform_list[i].local_float3.y, uniform_list[i].local_float3.z); break;
		case GL_FLOAT_VEC4: glUniform4f(glGetUniformLocation(ID, uniform_list[i].uniform_name.c_str()), uniform_list[i].local_float4.x, uniform_list[i].local_float4.y, uniform_list[i].local_float4.z, uniform_list[i].local_float4.w); break;
		}
	}
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}