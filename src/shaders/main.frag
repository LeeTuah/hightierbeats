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

	float diffuse_levels = 7.0f;
	float toon_diffuse = floor(diffuse_dot_value * diffuse_levels) / diffuse_levels;
	vec3 diffuse_light = toon_diffuse * light.diffuse * shard.diffuse;

	vec3 reflected_dirn = normalize(reflect(-light_dirn, normal));
	float specular_dot_value = pow(max(dot(reflected_dirn, viewer_dirn), 0.0f), shard.shininess);

	float specular_levels = 10.0f;
	float toon_specular = floor(specular_dot_value * specular_levels) / specular_levels;
	vec3 specular_light = toon_specular * light.specular * shard.specular;

	return (ambient_light + diffuse_light + specular_light);
}


in vec3 frag_pos;
in vec3 normal;

out vec4 FragColor;

uniform vec3 viewer_pos;
uniform Shard shard;

# define TOTAL_SUNS 10
uniform int allocated_suns;
uniform DirLight suns[TOTAL_SUNS];

void main() {
	vec3 norm = normalize(normal);
	vec3 viewer_dirn = normalize(viewer_pos - frag_pos);

	vec3 result = vec3(0.0f);

	for (int i = 0; i < allocated_suns; i++)
		result += calculate_dir_light(suns[i], shard, viewer_dirn, norm);

	FragColor = vec4(result, 1.0f);
}