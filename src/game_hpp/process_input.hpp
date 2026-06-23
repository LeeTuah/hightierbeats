# ifndef GAME_HPP_PROCESS_INPUT
# define GAME_HPP_PROCESS_INPUT

# include "../game.hpp"

inline void Game::process_input(GLFWwindow* window, float delta_time) {
	float current_time = glfwGetTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (game_state == GAME_MAIN_MENU) {
		if (current_time - last_menu_input_time <= menu_input_process_delay) return;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			if (current_menu_tile != main_menu_tiles.begin()) {
				last_menu_tile = current_menu_tile--;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 1.0f;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			if (current_menu_tile != main_menu_tiles.end() - 1) {
				last_menu_tile = current_menu_tile++;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 1.0f;
			}
		}
	}

	else if (game_state == GAME_RUNNING) {
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
}

# endif