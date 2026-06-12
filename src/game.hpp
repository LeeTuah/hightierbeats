# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include "includes/camera.hpp"
# include "includes/shaders.hpp"
# include "includes/texture.hpp"

# include <vector>
# include <map>

enum GameState {
	GAME_PAUSED,
	GAME_RUNNING,
	GAME_ZERO_HP
};

enum ShieldAlignment {
	W = 90,
	A = 180,
	S = -90,
	D = 0,
	WD = 45,
	DS = -45,
	SA = -135,
	WA = 135
};

struct Shard {
	glm::vec3 position;
	glm::vec3 velocity;

	float rotation_angle;
	glm::vec3 rotation_axis;

	bool active;
};

struct Shield {
	glm::vec3 position;
	glm::vec3 pivot;

	float rotation_angle;
	glm::vec3 rotation_axis;

	ShieldAlignment alignment;
	
	int segments;
};

class Game {
public:
	GameState game_state;

	Camera* camera;
	int SCR_WIDTH, SCR_HEIGHT;
	
	unsigned int shard_VAO, shard_VBO;
	unsigned int core_VAO, core_VBO;
	unsigned int shield_VAO, shield_VBO;
	unsigned int font_VAO, font_VBO;

	Shader* main_shader;
	Shader* text_shader;

	glm::mat4 view, projection, model, text_projection;
	std::vector<glm::vec3> sun_directions;
	
	std::vector<Shard> shards;
	Shard core;
	Shield shield;

	Game(int width, int height);

	void process_input(GLFWwindow* window, float delta_time);

	void render(float delta_time);
	void check_for_collisions();
private:
	void generate_VAOs();
	std::vector<float> generate_normals(std::vector<float> vertices);
};

Game::Game(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	game_state = GAME_RUNNING;

	camera = new Camera(glm::vec3(0.0f));

	main_shader = new Shader("shaders/main.vert", "shaders/main.frag");
	text_shader = new Shader("shaders/text.vert", "shaders/text.frag");

	generate_VAOs();

	shards.push_back({glm::vec3(+10.0f, 0.0f, +10.0f), glm::vec3(0.0f, 0.0f, 0.0f), 90.0f, glm::vec3(0.0f, 0.0f, 1.0f), false});
	shards.push_back({glm::vec3(+10.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 2.5f), 90.0f, glm::vec3(0.0f, 0.0f, 1.0f), false});

	core.position = glm::vec3(10.0f, 0.0f, 0.0f);
	core.velocity = glm::vec3(0.0f);

	core.rotation_angle = 0.0f;
	core.rotation_axis = glm::vec3(1.0f);

	core.active = true; // core.active means if it will be surrounded by wireframe or not

	shield.position = core.position + glm::vec3(0.0f, 0.0f, -1.5f);
	shield.pivot = core.position;
	shield.alignment = D;

	shield.rotation_angle = 0.0f;
	shield.rotation_axis = glm::vec3(-1.0f, 0.0f, 0.0f);
	// shield.rotation_axis = glm::vec3(0.0f, -1.0f, 0.0f);

	sun_directions.push_back(glm::vec3(+1.0f, -1.0f, +0.0f));
	sun_directions.push_back(glm::vec3(+1.0f, +1.0f, +0.0f));
}

void Game::process_input(GLFWwindow* window, float delta_time) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		shield.alignment = WA;
	} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		shield.alignment = SA;
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		shield.alignment = DS;
	} else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		shield.alignment = WD;
	}

	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		shield.alignment = W;
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		shield.alignment = S;
	} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		shield.alignment = A;
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		shield.alignment = D;
	}
}

void Game::render(float delta_time) {
	// glfwSetInputMode for mouse later
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	projection = glm::perspective(camera->zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera->get_lookat_matrix();
	text_projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
	model = glm::mat4(1.0f);

	main_shader->use();

	glBindVertexArray(shard_VAO);
	main_shader->set_mat4("projection", projection);
	main_shader->set_mat4("view", view);
	
	main_shader->set_float3("viewer_pos", camera->position);

	main_shader->set_float3("shard.ambient", 1.0f, 1.0f, 1.0f);
	main_shader->set_float3("shard.diffuse", 1.0f, 1.0f, 1.0f);
	main_shader->set_float3("shard.specular", 1.0f, 1.0f, 1.0f);

	main_shader->set_float("shard.shininess", 32.0f);

	main_shader->set_int("allocated_suns", sun_directions.size());
	for (size_t i = 0; i < sun_directions.size(); i++){
		std::string name = "suns[" + std::to_string(i) + "].";

		main_shader->set_float3(name + "ambient", 0.1f, 0.1f, 0.1f);
		main_shader->set_float3(name + "diffuse", 0.3f, 0.3f, 0.3f);
		main_shader->set_float3(name + "specular", 0.0f, 0.0f, 0.0f);

		main_shader->set_float3(name + "direction", sun_directions[i]);
	}

	for (auto &shard : shards) {
		if (shard.active) shard.position += shard.velocity * delta_time;

		model = glm::mat4(1.0f);
		model = glm::translate(model, shard.position);
		if (shard.rotation_angle != 0.0f)
			model = glm::rotate(model, glm::radians(shard.rotation_angle), glm::normalize(shard.rotation_axis));

		main_shader->set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 24);
	}

	glBindVertexArray(core_VAO);
	core.rotation_angle = 25.0f * glfwGetTime();

	model = glm::mat4(1.0f);
	model = glm::translate(model, core.position);
	model = glm::rotate(model, (float)glm::radians(core.rotation_angle), glm::normalize(core.rotation_axis));
	main_shader->set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 60);

	if (core.active) {
		model = glm::scale(model, glm::vec3(1.1f));
		main_shader->set_mat4("model", model);
		glDrawArrays(GL_LINE_LOOP, 0, 60);
	}

	glBindVertexArray(shield_VAO);
	shield.rotation_angle = (float)shield.alignment;

	model = glm::mat4(1.0f);
	model = glm::translate(model, core.position);
	if (shield.rotation_angle != 0)
		model = glm::rotate(model, glm::radians(shield.rotation_angle), glm::normalize(shield.rotation_axis));
	model = glm::translate(model, shield.position - core.position);
	main_shader->set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 24 * shield.segments + 12);
}

void Game::check_for_collisions() {
	// check for collisions here
}

void Game::generate_VAOs() {
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

	glGenVertexArrays(1, &font_VAO);
	glBindVertexArray(font_VAO);

	glGenBuffers(1, &font_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, font_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

std::vector<float> Game::generate_normals(std::vector<float> vertices) {
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