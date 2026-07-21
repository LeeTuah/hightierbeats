# version 330 core

in vec2 tex_coords;
out vec4 frag_color;

uniform sampler2D tex;

uniform bool vignette;

void main() {
	vec4 result = texture(tex, tex_coords);

	if (vignette) {
		float sdf_distance = distance(tex_coords, vec2(0.5f)) - 0.5f;
		float transparancy = smoothstep(0.0f, 0.2f, sdf_distance);
		result.rgb *= clamp(1 - transparancy, 0.3f, 1.0f);
	}

	frag_color = result;
}