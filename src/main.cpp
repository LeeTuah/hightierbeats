# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include "game.hpp"

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

float delta_time = 0.0f;
float last_time = 0.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main () {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HighTierBeats", NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Game* htb_engine = new Game(SCR_WIDTH, SCR_HEIGHT);

	// htb_engine->play_sound();
	while (not glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float current_time = glfwGetTime();
		delta_time = current_time - last_time;
		last_time = current_time;

		htb_engine->check_for_collisions();

		htb_engine->process_input(window, delta_time);
		htb_engine->update(delta_time);

		if (htb_engine->game_state == GAME_RUNNING)
			htb_engine->render_game();

		else if (htb_engine->game_state == GAME_MAIN_MENU)
			htb_engine->render_menu();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

