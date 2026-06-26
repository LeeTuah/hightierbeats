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
		else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
			std::string current_menu_label = (*current_menu_tile)->label;

			if (current_menu_label == "Play") {
				game_state = GAME_SELECTING_BEATMAP;
				load_all_beatmaps();
			}
			else if (current_menu_label == "Settings") {

			}
			else if (current_menu_label == "Credits") {

			}
			else if (current_menu_label == "Exit") {
				glfwSetWindowShouldClose(window, 1);
			}
		}
	}

	else if (game_state == GAME_SELECTING_BEATMAP) {
		if (current_time - last_menu_input_time <= menu_input_process_delay) return;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			if (current_user_beatmap_index > 0) {
				last_user_beatmap_index = current_user_beatmap_index--;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				is_beatmap_moving_up = false;
				beatmap_scale = 1.0f;
				beatmap_tile_distance_change = 0.0f;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			if (current_user_beatmap_index < beatmap_tiles.size() - 1) {
				last_user_beatmap_index = current_user_beatmap_index++;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				is_beatmap_moving_up = true;
				beatmap_scale = 1.0f;
				beatmap_tile_distance_change = 0.0f;
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