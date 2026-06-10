# version 330 core

struct Shard {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;

	vec3 color;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;
};

struct Spotlight {
	vec3 position;
	vec3 direction;

	float inner_cutoff;
	float outer_cutoff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	vec3 color;
};

vec3 calculate_dir_light(DirLight light, Shard shard, vec3 viewer_dirn, vec3 normal) {
	vec3 light_dirn = normalize(-(light.direction));
	
	vec3 ambient_light = light.ambient * shard.ambient;

	float diffuse_dot_value = max(dot(light_dirn, normal), 0.0f);
	vec3 diffuse_light = diffuse_dot_value * light.diffuse * shard.diffuse;

	vec3 reflected_dirn = normalize(reflect(-light_dirn, normal));
	float specular_dot_value = pow(max(dot(reflected_dirn, viewer_dirn), 0.0f), shard.shininess);
	vec3 specular_light = specular_dot_value * light.specular * shard.diffuse;

	return (ambient_light + diffuse_light + specular_light);
}


in vec3 frag_pos;
in vec3 normal;

out vec4 FragColor;

uniform Shard shard;
uniform DirLight sun;

void main() {
	vec3 norm = normalize(normal);
	vec3 viewer_pos = vec3(0.0f);
	vec3 viewer_dirn = normalize(viewer_pos - frag_pos);

	vec3 result = vec3(0.0f);

	result += calculate_dir_light(sun, shard, viewer_dirn, norm);

	FragColor = vec4(result, 1.0f);
}