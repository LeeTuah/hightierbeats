# ifndef GAME_HPP_GENERATE_VAOS
# define GAME_HPP_GENERATE_VAOS

# include "../game.hpp"

inline void Game::generate_VAOs() {
	const float shard_length[] = {0.2f, 1.0f, 0.5f};
	std::vector<float> shard_vertices = {
		+shard_length[0], 0.0f, 0.0f,
		0.0f, +shard_length[1], 0.0f,
		0.0f, 0.0f, +shard_length[2],

		0.0f, 0.0f, +shard_length[2],
		0.0f, +shard_length[1], 0.0f,
		-shard_length[0], 0.0f, 0.0f,

		-shard_length[0], 0.0f, 0.0f,
		0.0f, +shard_length[1], 0.0f,
		0.0f, 0.0f, -shard_length[2],

		0.0f, 0.0f, -shard_length[2],
		0.0f, +shard_length[1], 0.0f,
		+shard_length[0], 0.0f, 0.0f,

		0.0f, 0.0f, +shard_length[2],
		0.0f, -shard_length[1], 0.0f,
		+shard_length[0], 0.0f, 0.0f,

		-shard_length[0], 0.0f, 0.0f,
		0.0f, -shard_length[1], 0.0f,
		0.0f, 0.0f, +shard_length[2],

		0.0f, 0.0f, -shard_length[2],
		0.0f, -shard_length[1], 0.0f,
		-shard_length[0], 0.0f, 0.0f,

		+shard_length[0], 0.0f, 0.0f,
		0.0f, -shard_length[1], 0.0f,
		0.0f, 0.0f, -shard_length[2]
	};
	std::vector<float> shard_data = generate_normals(shard_vertices);

	glGenVertexArrays(1, &shard_VAO);
	glBindVertexArray(shard_VAO);

	glGenBuffers(1, &shard_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, shard_VBO);
	glBufferData(GL_ARRAY_BUFFER, shard_data.size() * sizeof(float), shard_data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenVertexArrays(1, &core_particle_VAO);
	glBindVertexArray(core_particle_VAO);

	glGenBuffers(1, &core_particle_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, core_particle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shard_data.size(), shard_data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &core_particle_VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, core_particle_VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * losing_shards.size(), losing_shards.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0));
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	glBindVertexArray(0);

	const float golden_ratio = (1.0f + sqrt(5)) / 2.0f;
	const float width = 0.5f;
	const float length = width * golden_ratio;

	std::vector<glm::vec3> golden_rect = {
		glm::vec3(-width, +length, 0),
		glm::vec3(+width, +length, 0),
		glm::vec3(-width, -length, 0),
		glm::vec3(+width, -length, 0),

		glm::vec3(0, -width, +length),
		glm::vec3(0, +width, +length),
		glm::vec3(0, -width, -length),
		glm::vec3(0, +width, -length),

		glm::vec3(+length, 0, -width),
		glm::vec3(+length, 0, +width),
		glm::vec3(-length, 0, -width),
		glm::vec3(-length, 0, +width),
	};

	std::vector<float> core_vertices = {
		golden_rect[0].x, golden_rect[0].y, golden_rect[0].z,
		golden_rect[11].x, golden_rect[11].y, golden_rect[11].z,
		golden_rect[5].x, golden_rect[5].y, golden_rect[5].z,

		golden_rect[0].x, golden_rect[0].y, golden_rect[0].z,
		golden_rect[5].x, golden_rect[5].y, golden_rect[5].z,
		golden_rect[1].x, golden_rect[1].y, golden_rect[1].z,

		golden_rect[0].x, golden_rect[0].y, golden_rect[0].z,
		golden_rect[1].x, golden_rect[1].y, golden_rect[1].z,
		golden_rect[7].x, golden_rect[7].y, golden_rect[7].z,

		golden_rect[0].x, golden_rect[0].y, golden_rect[0].z,
		golden_rect[7].x, golden_rect[7].y, golden_rect[7].z,
		golden_rect[10].x, golden_rect[10].y, golden_rect[10].z,

		golden_rect[0].x, golden_rect[0].y, golden_rect[0].z,
		golden_rect[10].x, golden_rect[10].y, golden_rect[10].z,
		golden_rect[11].x, golden_rect[11].y, golden_rect[11].z,

		golden_rect[1].x, golden_rect[1].y, golden_rect[1].z,
		golden_rect[5].x, golden_rect[5].y, golden_rect[5].z,
		golden_rect[9].x, golden_rect[9].y, golden_rect[9].z,

		golden_rect[5].x, golden_rect[5].y, golden_rect[5].z,
		golden_rect[11].x, golden_rect[11].y, golden_rect[11].z,
		golden_rect[4].x, golden_rect[4].y, golden_rect[4].z,

		golden_rect[11].x, golden_rect[11].y, golden_rect[11].z,
		golden_rect[10].x, golden_rect[10].y, golden_rect[10].z,
		golden_rect[2].x, golden_rect[2].y, golden_rect[2].z,

		golden_rect[10].x, golden_rect[10].y, golden_rect[10].z,
		golden_rect[7].x, golden_rect[7].y, golden_rect[7].z,
		golden_rect[6].x, golden_rect[6].y, golden_rect[6].z,

		golden_rect[7].x, golden_rect[7].y, golden_rect[7].z,
		golden_rect[1].x, golden_rect[1].y, golden_rect[1].z,
		golden_rect[8].x, golden_rect[8].y, golden_rect[8].z,

		golden_rect[3].x, golden_rect[3].y, golden_rect[3].z,
		golden_rect[9].x, golden_rect[9].y, golden_rect[9].z,
		golden_rect[4].x, golden_rect[4].y, golden_rect[4].z,

		golden_rect[3].x, golden_rect[3].y, golden_rect[3].z,
		golden_rect[4].x, golden_rect[4].y, golden_rect[4].z,
		golden_rect[2].x, golden_rect[2].y, golden_rect[2].z,

		golden_rect[3].x, golden_rect[3].y, golden_rect[3].z,
		golden_rect[2].x, golden_rect[2].y, golden_rect[2].z,
		golden_rect[6].x, golden_rect[6].y, golden_rect[6].z,

		golden_rect[3].x, golden_rect[3].y, golden_rect[3].z,
		golden_rect[6].x, golden_rect[6].y, golden_rect[6].z,
		golden_rect[8].x, golden_rect[8].y, golden_rect[8].z,

		golden_rect[3].x, golden_rect[3].y, golden_rect[3].z,
		golden_rect[8].x, golden_rect[8].y, golden_rect[8].z,
		golden_rect[9].x, golden_rect[9].y, golden_rect[9].z,

		golden_rect[4].x, golden_rect[4].y, golden_rect[4].z,
		golden_rect[9].x, golden_rect[9].y, golden_rect[9].z,
		golden_rect[5].x, golden_rect[5].y, golden_rect[5].z,

		golden_rect[2].x, golden_rect[2].y, golden_rect[2].z,
		golden_rect[4].x, golden_rect[4].y, golden_rect[4].z,
		golden_rect[11].x, golden_rect[11].y, golden_rect[11].z,

		golden_rect[6].x, golden_rect[6].y, golden_rect[6].z,
		golden_rect[2].x, golden_rect[2].y, golden_rect[2].z,
		golden_rect[10].x, golden_rect[10].y, golden_rect[10].z,

		golden_rect[8].x, golden_rect[8].y, golden_rect[8].z,
		golden_rect[6].x, golden_rect[6].y, golden_rect[6].z,
		golden_rect[7].x, golden_rect[7].y, golden_rect[7].z,

		golden_rect[9].x, golden_rect[9].y, golden_rect[9].z,
		golden_rect[8].x, golden_rect[8].y, golden_rect[8].z,
		golden_rect[2].x, golden_rect[2].y, golden_rect[2].z,

	};
	std::vector<float> core_data = generate_normals(core_vertices);

	glGenVertexArrays(1, &core_VAO);
	glBindVertexArray(core_VAO);

	glGenBuffers(1, &core_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, core_VBO);
	glBufferData(GL_ARRAY_BUFFER, core_data.size() * sizeof(float), core_data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	std::vector<float> shield_vertices;

	auto push_four = [&](glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4) {
		shield_vertices.insert(shield_vertices.end(), {
			v1.x, v1.y, v1.z,
			v2.x, v2.y, v2.z,
			v3.x, v3.y, v3.z,

			v1.x, v1.y, v1.z,
			v3.x, v3.y, v3.z,
			v4.x, v4.y, v4.z,
		});
	};

	shield.segments = 8;
	float total_angle = glm::radians(90.0f);

	float radius_in = 1.4f;
	float radius_out = 1.6;
	float thick_x = 0.2f;

	float angle_change = total_angle / (float)shield.segments;
	float starting_angle = -total_angle / 2.0f;

	float angle_one, angle_two, y1_in, y1_out, z1_in, z1_out, y2_in, y2_out, z2_in, z2_out;
	glm::vec3 in_bl, in_br, in_tl, in_tr, out_bl, out_br, out_tl, out_tr;

	for (int i = 0; i < shield.segments; i++) {
		angle_one = starting_angle + i * angle_change;
		angle_two = starting_angle + (i + 1) * angle_change;

		y1_in = radius_in * sin(angle_one);
		z1_in = -radius_in * cos(angle_one) + 1.5f;
		y1_out = radius_out * sin(angle_one);
		z1_out = -radius_out * cos(angle_one) + 1.5f;

		y2_in = radius_in * sin(angle_two);
		z2_in = -radius_in * cos(angle_two) + 1.5f;
		y2_out = radius_out * sin(angle_two);
		z2_out = -radius_out * cos(angle_two) + 1.5f;

		in_bl = glm::vec3(-thick_x, y1_in, z1_in);
		in_br = glm::vec3( thick_x, y1_in, z1_in);
		in_tl = glm::vec3(-thick_x, y2_in, z2_in);
		in_tr = glm::vec3( thick_x, y2_in, z2_in);

		out_bl = glm::vec3(-thick_x, y1_out, z1_out);
		out_br = glm::vec3( thick_x, y1_out, z1_out);
		out_tl = glm::vec3(-thick_x, y2_out, z2_out);
		out_tr = glm::vec3( thick_x, y2_out, z2_out);

		push_four(out_bl, out_br, out_tr, out_tl);
		push_four(in_br, in_bl, in_tl, in_tr);
		push_four(out_br, in_br, in_tr, out_tr);
		push_four(in_bl, out_bl, out_tl, in_tl);

		if (i == 0) push_four(in_bl, in_br, out_br, out_bl);
		if (i == shield.segments - 1) push_four(out_tl, out_tr, in_tr, in_tl);
	}

	std::vector<float> shield_data = generate_normals(shield_vertices);
	
	glGenVertexArrays(1, &shield_VAO);
	glBindVertexArray(shield_VAO);

	glGenBuffers(1, &shield_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, shield_VBO);
	glBufferData(GL_ARRAY_BUFFER, shield_data.size() * sizeof(float), shield_data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	const float square_size = 0.1f;
	const float square_offset = 0.043f;
	float square_vertices[] = {
		+square_size + square_offset, +square_size,
		-square_size + square_offset, +square_size,
		-square_size - square_offset, -square_size,

		-square_size - square_offset, -square_size,
		+square_size - square_offset, -square_size,
		+square_size + square_offset, +square_size
	};

	glGenVertexArrays(1, &square_VAO);
	glBindVertexArray(square_VAO);

	glGenBuffers(1, &square_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, square_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	float rect_vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		+0.5f, -0.5f, 1.0f, 0.0f,
		+0.5f, +0.5f, 1.0f, 1.0f,

		+0.5f, +0.5f, 1.0f, 1.0f,
		-0.5f, +0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f
	};

	glGenVertexArrays(1, &rect_VAO);
	glBindVertexArray(rect_VAO);

	glGenBuffers(1, &rect_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, rect_VBO);
	glBufferData(GL_ARRAY_BUFFER, std::size(rect_vertices) * sizeof(float), rect_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenVertexArrays(1, &font_VAO);
	glBindVertexArray(font_VAO);

	glGenBuffers(1, &font_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, font_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

inline std::vector<float> Game::generate_normals(std::vector<float> vertices) {
	std::vector<float> data;

	for (size_t i = 0; i < vertices.size(); i += 9) {
		glm::vec3 vertex_one(vertices[i], vertices[i + 1], vertices[i + 2]);
		glm::vec3 vertex_two(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
		glm::vec3 vertex_thr(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

		glm::vec3 dir_vec_one = vertex_two - vertex_one;
		glm::vec3 dir_vec_two = vertex_thr - vertex_one;
		glm::vec3 normal = glm::normalize(glm::cross(dir_vec_one, dir_vec_two));

		data.push_back(vertex_one.x); 
		data.push_back(vertex_one.y);
		data.push_back(vertex_one.z);
		data.push_back(normal.x);
		data.push_back(normal.y);
		data.push_back(normal.z);

		data.push_back(vertex_two.x); 
		data.push_back(vertex_two.y);
		data.push_back(vertex_two.z);
		data.push_back(normal.x);
		data.push_back(normal.y);
		data.push_back(normal.z);

		data.push_back(vertex_thr.x); 
		data.push_back(vertex_thr.y);
		data.push_back(vertex_thr.z);
		data.push_back(normal.x);
		data.push_back(normal.y);
		data.push_back(normal.z);
	}

	return data;
}

# endif