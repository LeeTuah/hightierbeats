# version 330 core

in vec2 tex_coords;
out vec4 frag_color;

void main() {
	float sdf_distance = distance(tex_coords, vec2(0.5f)) - 0.5f;

	vec3 result_color = vec3(0.0f);
	float transparancy = smoothstep(0.0f, 0.2f, sdf_distance);

	frag_color = vec4(result_color, transparancy);
}