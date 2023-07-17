#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec4 Position;

uniform vec4 test_color;
uniform vec4 sunrise_color;
uniform vec4 sunset_color;
uniform sampler2D texture0;
uniform sampler2D texture1;

uniform mat4 inv_view_projection;
uniform vec3 camera_position;
//uniform float eps;

uniform vec3 spos;

float sphere_sdf(vec3 raypos, vec3 sphere_pos, float radius)
{
	return distance(raypos, sphere_pos) - radius;
}

float blob_sdf(vec3 raypos, vec3 sphere_pos, float radius)
{
	vec3 ray = normalize(raypos-sphere_pos);
	float theta = atan(ray.x, ray.z);
	float phi = atan(sqrt(ray.x*ray.x + ray.z*ray.z), ray.y);

	radius += 0.05*radius * sin(theta*10) * sin(phi*10);
	return distance(raypos, sphere_pos) - radius;
}

/*vec3 calc_normal(void* func, int f)
{

}*/

void main()
{
    FragColor = texture(texture0, TexCoord) ;//* test_color;// * vec4(ourColor, 1.0);
	return;

	//FragColor = vec4(TexCoord, 0.0, 1.0);

	//FragColor = inputColor*TexCoord.x;

	//FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), inputColor.g);

	vec4 pos = inv_view_projection * Position;

	vec4 unproj_pos = inv_view_projection * vec4(Position.xy, 1, 1);
	unproj_pos  = unproj_pos / unproj_pos.w;
	vec3 world_pos = unproj_pos.xyz;

	vec3 sunrise = vec3(1, 0, 0);
	vec3 sunset  = vec3(-1, 0, 0);

	//FragColor = mix(sunset_color, sunrise_color, dot(normalize(unproj_pos.xyz), sunrise)*0.5 + 0.5) * sqrt(unproj_pos.y);// + 

	//return;

	vec3 ray = normalize(world_pos - camera_position);
	vec3 march_pos = camera_position;

	vec3 light_dir = normalize(vec3(0.3f, -0.5f, 0.3f));


	//background if no ray intersection
	vec3 color = vec3(0,0,0);
	float eps = 0.0001;

	for (int i = 0; i < 100; i++)
	{
		float sdf = blob_sdf(march_pos, spos, 1.0f);
		sdf = max(0, sdf);
		if (sdf < eps)
		{
			vec3 normal = normalize(march_pos - spos);
			float light = dot(light_dir, -normal);
			color += vec3(light, light, light);
			break;
		}
		
		march_pos += ray * sdf;
	}

	FragColor = vec4(color.xyz, 1.0);
}