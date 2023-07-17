#include "fullscreen_quad.h"

//using namespace std;

float plane_vertices[] = {
	// positions          // texture coords
	 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
	 1.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
	-1.0f,  1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

unsigned int plane_indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
};

//Shader fullscreen_quad_shader;
unsigned int plane_VBO, plane_VAO, plane_EBO;

void fullscreen_quad_init(/*string vertex_shader, string fragment_shader*/)
{
	//fullscreen_quad_shader = Shader(vertex_shader.c_str(), fragment_shader.c_str());

	glGenVertexArrays(1, &plane_VAO);
	glGenBuffers(1, &plane_VBO);
	glGenBuffers(1, &plane_EBO);


	glBindVertexArray(plane_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, plane_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_indices), plane_indices, GL_STATIC_DRAW);

	//each vertex contains vec3 pos, vec3 color, vec2 texcoord
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/*unsigned int fullscreen_quad_get_shader_ID()
{
	return fullscreen_quad_shader.ID;
}*/

void fullscreen_quad_draw()
{
	glBindVertexArray(plane_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//for reference later for unindexed primitives:
	//and now draw the cube:
		//glBindVertexArray(cube_VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
}

void fullscreen_quad_delete()
{
	glDeleteVertexArrays(1, &plane_VAO);
	glDeleteBuffers(1, &plane_VBO);
	glDeleteBuffers(1, &plane_EBO);
}