# version 330 core

layout (location = 0) in vec2 in_vertex_coords;
layout (location = 1) in vec2 in_texture_coords;

out vec2 tex_coords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	tex_coords = in_texture_coords;

	gl_Position = projection * view * model * vec4(in_vertex_coords, 0.0f, 1.0f);
}