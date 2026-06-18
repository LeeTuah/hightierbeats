# version 330 core

in vec2 tex_coords;

out vec4 FragColor;

uniform sampler2D tex;
uniform float background_dim;

void main() {
	vec3 result = texture(tex, tex_coords).rgb * vec3(clamp(background_dim, 0.0f, 1.0f));
	FragColor = vec4(result, 1.0f);
}