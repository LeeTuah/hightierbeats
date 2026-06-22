# version 330 core

in vec2 tex_coords;

out vec4 FragColor;

uniform sampler2D tex;
uniform float background_dim;
uniform vec3 color;

uniform bool use_texture;

void main() {
	vec3 result;
	if (use_texture)
		result = texture(tex, tex_coords).rgb * vec3(clamp(background_dim, 0.0f, 1.0f));
	else
		result = color;

	FragColor = vec4(result, 1.0f);
}