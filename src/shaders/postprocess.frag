# version 330 core

in vec2 tex_coords;
out vec4 frag_color;

uniform sampler2D tex;

uniform bool chromatic;
uniform vec2 ca_offset;

uniform bool hdr;
uniform bool vignette;

void main() {
	vec4 result = texture(tex, tex_coords);

	if (hdr) {
		float exposure = 1.0f;
		result.rgb = vec3(1.0f) - exp(-result.rgb * exposure);
	}

	if (chromatic) {
		vec2 direction = tex_coords - vec2(0.5f, 0.3f);
		result.r = texture(tex, tex_coords - (direction * ca_offset)).r;
		result.g = texture(tex, tex_coords).g;
		result.b = texture(tex, tex_coords + (direction * ca_offset)).b;
	}

	if (vignette) {
		float sdf_distance = distance(tex_coords, vec2(0.5f)) - 0.5f;
		float transparancy = smoothstep(0.0f, 0.2f, sdf_distance);
		result.rgb *= clamp(1.0f - transparancy, 0.4f, 1.0f);
	}

	frag_color = result;
}