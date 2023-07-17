#ifndef RAY_TRACE_H
#define RAY_TRACE_H
#include <vector>
//#include "ray_trace.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned char* allocate_image_memory(int width, int height);
unsigned int bind_image();
void delete_image_memory(unsigned char* data);
unsigned int bind_image_memory(unsigned int texture, unsigned char* data, int width, int height);

//later

//Need to implement its own intersection class
/*
typedef struct {
	glm::vec3 center;
	float radius;
	glm::vec3 color;
	//...
} sphere_t;
*/

typedef struct {
	glm::vec3 origin;
	glm::vec3 direction;
} ray_t;

typedef glm::vec3 ray_color;

typedef struct {
	ray_t inbound;
	ray_t outbound;
	glm::vec3 position;
	ray_color color;
	element* intersected_element;
} intersection_t;

typedef struct {
	glm::vec3 c0;
	glm::vec3 c1;
} bounding_box_t;

class brdf
{
public:
 float sample_brdf(glm::vec3 inbound, glm::vec3 outbound) = 0;
}

class element
{
public:
	glm::vec3 position;
	bounding_box_t bounds;
	virtual intersection_t intersect(ray_t* ray) = 0;
};

class light_t
{
public:
	ray_color color;
	intersection_t* intersect(ray_t* ray)
	{

	}
};

class directional_light : light_t
{
public:
	glm::vec3 direction;
	intersection_t* intersect(ray_t* ray)
	{
		intersection_t* i = new intersection_t();

	}
};

std::vector<sphere_t> generate_test_scene();
void demo_trace(unsigned char* image_memory, int width, int height, std::vector<sphere_t> scene, glm::mat4x4 view, glm::mat4x4 projection, glm::vec3 camera_position, int ray_count);

#endif