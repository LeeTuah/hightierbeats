# version 330 core

layout (location = 0) in vec4 vertex_data;

out vec2 tex_coords;

uniform mat4 projection;

void main () {
	gl_Position = projection * vec4(vertex_data.x, vertex_data.y, 0.0f, 1.0f);

	tex_coords = vec2(vertex_data.z, vertex_data.w);
}