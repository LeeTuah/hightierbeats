# version 330 core

layout (location = 0) in vec3 in_vec_pos;
layout (location = 1) in vec3 in_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_pos;
out vec3 normal;

void main() {
	frag_pos = vec3(model * vec4(in_vec_pos, 1.0f));
	
	mat3 normal_matrix = transpose(inverse(mat3(model)));
	normal = normalize(normal_matrix * in_normal);

	gl_Position = projection * view * model * vec4(in_vec_pos, 1.0f);
}