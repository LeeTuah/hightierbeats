# version 330 core

layout (location = 0) in vec3 in_vec_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_offset;

uniform mat4 view;
uniform mat4 projection;
uniform float scale;

out vec3 frag_pos;
out vec3 normal;

void main() {
	vec3 scaled_down = in_vec_pos * scale;
	vec3 model_pos = scaled_down + in_offset;

	frag_pos = model_pos;
	normal = normalize(in_normal);

	gl_Position = projection * view * vec4(model_pos, 1.0f);
}