# ifndef GAME_HPP_PROCESS_INPUT
# define GAME_HPP_PROCESS_INPUT

# include "../game.hpp"

inline void Game::process_input(GLFWwindow* window, float delta_time) {
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

	bool is_strike_frame = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) or
						  (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) or
						  (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) or
						  (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
}

# endif