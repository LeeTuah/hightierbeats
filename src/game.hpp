# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include "includes/camera.hpp"
# include "includes/shaders.hpp"
# include "includes/texture.hpp"

# include <vector>

enum GameState {
	GAME_PAUSED,
	GAME_RUNNING,
	GAME_ZERO_HP
};

struct Shard {
	glm::vec3 position;
	glm::vec3 velocity;
	bool active;
};

class Game {
public:
	GameState game_state;

	Camera* camera;
	int SCR_WIDTH, SCR_HEIGHT;
	
	unsigned int shard_VAO, shard_VBO;
	unsigned int font_VAO, font_VBO;

	Shader* main_shader;
	Shader* text_shader;

	glm::mat4 view, projection, model, text_projection;

	Game(int width, int height);

	void process_input(GLFWwindow* window, float delta_time);

	void render(float delta_time);

private:
	void generate_VAOs();
};

Game::Game(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	game_state = GAME_RUNNING;

	camera = new Camera(glm::vec3(0.0f));
	camera->yaw = 89.0f;

	main_shader = new Shader("shaders/main.vert", "shaders/main.frag");
	text_shader = new Shader("shaders/text.vert", "shaders/text.frag");

	generate_VAOs();
}

void Game::process_input(GLFWwindow* window, float delta_time) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void Game::render(float delta_time) {
	// glfwSetInputMode for mouse later
	glClearColor(0.2f, 0.2f, 0.0f, 1.0f);

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
	
	main_shader->set_float3("shard.ambient", 1.0f, 1.0f, 1.0f);
	main_shader->set_float3("shard.diffuse", 1.0f, 1.0f, 1.0f);
	main_shader->set_float3("shard.specular", 1.0f, 1.0f, 1.0f);

	main_shader->set_float("shard.shininess", 32.0f);

	main_shader->set_float3("sun.ambient", 0.1f, 0.1f, 0.1f);
	main_shader->set_float3("sun.diffuse", 0.3f, 0.3f, 0.3f);
	main_shader->set_float3("sun.specular", 1.0f, 1.0f, 1.0f);

	main_shader->set_float3("sun.direction", 0.0f, 1.0f, 0.0f);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(+5.0f, 0.0f, -5.0f));
	model = glm::rotate(model, (float)glm::radians(sin(glfwGetTime())), glm::normalize(glm::vec3(1.0f)));
	main_shader->set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 24);

	// TODO: move the shard from one side of the screen to the other

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(+5.0f, 0.0f, +5.0f));
	model = glm::rotate(model, (float)glm::radians(sin(glfwGetTime())), glm::normalize(glm::vec3(1.0f)));
	main_shader->set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 24);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(+1.0f, 0.0f, +0.0f));
	model = glm::rotate(model, (float)glm::radians(glfwGetTime()), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
	main_shader->set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 24);
}

void Game::generate_VAOs() {
	const float shard_length = 0.5f;
	std::vector<float> shard_vertices = {
		+shard_length, 0.0f, 0.0f,
		0.0f, +shard_length, 0.0f,
		0.0f, 0.0f, +shard_length,

		-shard_length, 0.0f, 0.0f,
		0.0f, +shard_length, 0.0f,
		0.0f, 0.0f, +shard_length,

		-shard_length, 0.0f, 0.0f,
		0.0f, +shard_length, 0.0f,
		0.0f, 0.0f, -shard_length,

		+shard_length, 0.0f, 0.0f,
		0.0f, +shard_length, 0.0f,
		0.0f, 0.0f, -shard_length,

		+shard_length, 0.0f, 0.0f,
		0.0f, -shard_length, 0.0f,
		0.0f, 0.0f, +shard_length,

		-shard_length, 0.0f, 0.0f,
		0.0f, -shard_length, 0.0f,
		0.0f, 0.0f, +shard_length,

		-shard_length, 0.0f, 0.0f,
		0.0f, -shard_length, 0.0f,
		0.0f, 0.0f, -shard_length,

		+shard_length, 0.0f, 0.0f,
		0.0f, -shard_length, 0.0f,
		0.0f, 0.0f, -shard_length
	};
	std::vector<float> shard_data;

	for (int i = 0; i < shard_vertices.size(); i += 9) {
		glm::vec3 vertex_one(shard_vertices[i], shard_vertices[i + 1], shard_vertices[i + 2]);
		glm::vec3 vertex_two(shard_vertices[i + 3], shard_vertices[i + 4], shard_vertices[i + 5]);
		glm::vec3 vertex_thr(shard_vertices[i + 6], shard_vertices[i + 7], shard_vertices[i + 8]);

		glm::vec3 dir_vec_one = vertex_two - vertex_one;
		glm::vec3 dir_vec_two = vertex_thr - vertex_one;
		glm::vec3 normal = glm::normalize(glm::cross(dir_vec_one, dir_vec_two));

		shard_data.push_back(vertex_one.x); 
		shard_data.push_back(vertex_one.y);
		shard_data.push_back(vertex_one.z);
		shard_data.push_back(normal.x);
		shard_data.push_back(normal.y);
		shard_data.push_back(normal.z);

		shard_data.push_back(vertex_two.x); 
		shard_data.push_back(vertex_two.y);
		shard_data.push_back(vertex_two.z);
		shard_data.push_back(normal.x);
		shard_data.push_back(normal.y);
		shard_data.push_back(normal.z);

		shard_data.push_back(vertex_thr.x); 
		shard_data.push_back(vertex_thr.y);
		shard_data.push_back(vertex_thr.z);
		shard_data.push_back(normal.x);
		shard_data.push_back(normal.y);
		shard_data.push_back(normal.z);
	}

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

	glGenVertexArrays(1, &font_VAO);
	glBindVertexArray(font_VAO);

	glGenBuffers(1, &font_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, font_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}