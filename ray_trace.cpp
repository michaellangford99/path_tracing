#include "ray_trace.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <map>

#include <float.h>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

#include "shader.h"
#include "camera.h"
#include "fullscreen_quad.h"

// Soo.
// we need to allocate a buffer for the raw data to go ti
// create a function that generates a GL image from that.
// and then takes as input the inverse view projection matrices to transform from projection space rays to world space
//at least I think
//that is a good solution.
//then I can trace rays through some sort of object tree
//

/* things I need
 object tree
 some sort of registration system for properties that aren't just a 'view' of the object tree
 a way for the system to automatically register new properties of the object tree
 .... properties of shaders. so automatically extract all types and names of uniforms

 a way to load .obj files in a sane way
 */

/*
class element {
	glm::vec3 
};*/

unsigned char* allocate_image_memory(int width, int height)
{
	unsigned char *data = new unsigned char[width*height * 3];

	memset((void*)data, 0, width*height * 3);

	return data;
}

void delete_image_memory(unsigned char* data)
{
	delete data;
}

unsigned int bind_image()
{
	unsigned int texture;
	glGenTextures(1, &texture);
	return texture;
}

unsigned int bind_image_memory(unsigned int texture, unsigned char* data, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

/*
for each pixel, randomly sample

recursively sample through reflections
if no intersections with light source after X bounces, return zero

monte carlo integration done per sample

divide sampled value by brdf prob, using random vector drawn from distribution def by brdf



*/

/*node_t* generate_test_scene()
{
	node_t* root = new node_t();
	root->s.radius = 1;
	root->s.center = glm::vec3(0, 0, 0);
}*/

std::vector<sphere_t> generate_test_scene()
{
	std::vector<sphere_t> slist = std::vector<sphere_t>();
	sphere_t s = {glm::vec3(0, 0, 0), 1, glm::vec3(1, 0.5, 1)};
	slist.push_back(s);
	s.center = glm::vec3(1, 0, 1);
	slist.push_back(s);
	s.center = glm::vec3(-1, 0, 1);
	s.color = glm::vec3(1, 1, 1);
	slist.push_back(s);

	return slist;
}

bool intersect_sphere(ray_t* r, sphere_t* s, float* t1, float *t2)
{
	float a = glm::dot(r->direction, r->direction);
	float b = 2 * glm::dot(r->direction, r->origin - s->center);
	float c = glm::dot(s->center, s->center) + glm::dot(r->origin, r->origin) - 2 * glm::dot(s->center, r->origin) - s->radius*s->radius;

	float discr = b*b - 4*a*c;

	if (discr < 0) return false;
	//if (discr >= 0)

	*t1 = (-b - glm::sqrt(discr)) / (2 * a);
	*t2 = (-b + glm::sqrt(discr)) / (2 * a);

	return true;

	/*
	//solve for tc
	glm::vec3 L = s->center - r->origin;
	float tc = glm::dot(L, r->direction);

	if (tc < 0.0) return false;
	float d2 = (tc*tc) - glm::dot(L,L);

	float radius2 = s->radius * s->radius;
	if (d2 > radius2) return false;

	//solve for t1c
	float t1c = glm::sqrt(radius2 - d2);

	//solve for intersection points
	*t1 = tc - t1c;
	*t2 = tc + t1c;

	return true;*/
}

intersection_t* intersect_ray(ray_t* ray, std::vector<sphere_t> slist)
{
	float closest_intersection_distance = 9999999.0f;//no #def oh well
	int closest_index = 0;
	for (int i = 0; i < slist.size(); i++)
	{
		float t1, t2;
		if (intersect_sphere(ray, &(slist[i]), &t1, &t2))
		{
			float fm = fmin(t1, t2);
			if (fm < closest_intersection_distance)
			{
				closest_intersection_distance = fm;
				closest_index = i;
			}
		}
	}

	//oopsie poopsie
	if (closest_intersection_distance >= 9999988.0f) return nullptr;

	intersection_t* intersect = new intersection_t();
	intersect->position = ray->origin + ray->direction*closest_intersection_distance;
	intersect->normal = glm::normalize(intersect->position - slist[closest_index].center);
	intersect->color = slist[closest_index].color;

	return intersect;
}

int last_x = 0;
int last_y = 0;

ray_color trace_light_ray(ray_t* ray, std::vector<sphere_t> scene, )

void demo_trace(unsigned char* image_memory, int width, int height, std::vector<sphere_t> scene, glm::mat4x4 view, glm::mat4x4 projection, glm::vec3 camera_position, int ray_count)
{
	//generate rays going out of camera
	glm::mat4 inv_view_projection = glm::inverse(projection * view);
	glm::vec4 camera_pos = inv_view_projection * glm::vec4(0, 0, -1, 1);
	camera_pos /= camera_pos.w;

	/*std::cout << "<" << camera_position.x << ", "
		<< camera_position.y << ", "
		<< camera_position.z << ">, "
			  << "<" 
		<< camera_pos.x << ", "
		<< camera_pos.y << ", "
		<< camera_pos.z << ">"
		<< std::endl;*/

	int x = last_x;
	int y = last_y;
	int ray_count_current = 0;

	while (ray_count_current < ray_count)
	{

		glm::vec4 r = glm::vec4((x - width / 2) / (float)width, (y - height / 2) / (float)height, 0, 1);
		r = inv_view_projection * r;
		r /= r.w;

		/*if (x == 0 && y == 0)
		{
			printf("RAY=<%f,%f,%f<\n", r.x, r.y, r.z);
		}*/

		ray_t ray;
		ray.direction = glm::normalize(glm::vec3(r.x, r.y, r.z) - camera_position);
		ray.origin = camera_position;

		//sphere_t test;
		//test.center = glm::vec3(1, 0, 0);
		//test.radius = 1.0f;

		intersection_t* intersection = intersect_ray(&ray, scene);

		if (intersection != nullptr)
		{
			glm::vec3 light = glm::normalize(glm::vec3(0.2, -1, 0.5));

			float lighting = glm::dot(-light, intersection->normal);

			lighting = fmin(lighting, 1);
			lighting = fmax(lighting, 0);

			image_memory[x * 3 + width * y * 3 + 0] = int(255 * lighting);
			image_memory[x * 3 + width * y * 3 + 1] = int(255 * lighting);
			image_memory[x * 3 + width * y * 3 + 2] = int(255 * lighting);
		}
		else
		{
			image_memory[x * 3 + width * y * 3 + 0] = 0;
			image_memory[x * 3 + width * y * 3 + 1] = 0;
			image_memory[x * 3 + width * y * 3 + 2] = 0;
		}
		ray_count_current++;

		x++;
		if (x >= width)
		{
			x = 0;
			y++;
			if (y >= height)
				y = 0;
		}

	}

	last_x = x;
	last_y = y;
}