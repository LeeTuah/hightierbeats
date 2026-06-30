# ifndef GAME_HPP_PROCESS_INPUT
# define GAME_HPP_PROCESS_INPUT

# include "../game.hpp"

inline void Game::process_input(GLFWwindow* window, float delta_time) {
	float current_time = glfwGetTime();

	bool w_key_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
	bool s_key_pressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
	bool a_key_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	bool d_key_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	bool select_key_pressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	bool esc_key_pressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

	auto go_back_to_main_menu = [&]() {
		game_state = GAME_MAIN_MENU;
		animating_menu_tile = false;

		current_menu_tile = main_menu_tiles.begin();
		last_menu_tile = main_menu_tiles.end();

		current_user_beatmap_index = 0;
		last_menu_input_time = current_time;
		menu_scale = max_menu_scale;

		current_pause_menu_tile = pause_menu_tiles.begin();
		last_pause_menu_tile = pause_menu_tiles.end();
	};

	if (game_state == GAME_MAIN_MENU) {
		if (current_time - last_menu_input_time <= menu_input_process_delay) return;

		if (w_key_pressed) {
			if (current_menu_tile != main_menu_tiles.begin()) {
				last_menu_tile = current_menu_tile--;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 1.0f;
			}
		}
		else if (s_key_pressed) {
			if (current_menu_tile != main_menu_tiles.end() - 1) {
				last_menu_tile = current_menu_tile++;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 1.0f;
			}
		}
		else if (select_key_pressed) {
			std::string current_menu_label = (*current_menu_tile)->label;

			if (current_menu_label == "Play") {
				game_state = GAME_SELECTING_BEATMAP;
				load_all_beatmaps();
				last_menu_input_time = current_time;
			}
			else if (current_menu_label == "Settings") {

			}
			else if (current_menu_label == "Mapmaker") {
				game_state = GAME_MAPMAKER;
			}
			else if (current_menu_label == "Exit") {
				glfwSetWindowShouldClose(window, true);
			}
		}
		if (esc_key_pressed) {
			glfwSetWindowShouldClose(window, true);
		}
	}

	else if (game_state == GAME_SELECTING_BEATMAP) {
		if (current_time - last_menu_input_time <= menu_input_process_delay) return;

		if (w_key_pressed) {
			if (current_user_beatmap_index > 0) {
				last_user_beatmap_index = current_user_beatmap_index--;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				beatmap_scale = 1.0f;
				beatmap_tile_distance_change = 0.0f;
			}
		}
		else if (s_key_pressed) {
			if (current_user_beatmap_index < beatmap_tiles.size() - 1) {
				last_user_beatmap_index = current_user_beatmap_index++;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				beatmap_scale = 1.0f;
				beatmap_tile_distance_change = 0.0f;
			}
		}
		else if (select_key_pressed) {
			load_beatmap_from_file();
			game_state = GAME_RUNNING;

			play_sound();
		}
		if (esc_key_pressed) {
			go_back_to_main_menu();
		}
	}
	
	else if (game_state == GAME_WIN) {
		if (select_key_pressed) {
			go_back_to_main_menu();
		}
	}

	else if (game_state == GAME_PAUSED) {
		if (current_time - last_menu_input_time <= menu_input_process_delay) return;

		if (esc_key_pressed) {
			game_state = GAME_RUNNING;
			play_sound();

			last_menu_input_time = current_time;
		}

		if (w_key_pressed) {
			if (current_pause_menu_tile != pause_menu_tiles.begin()) {
				last_pause_menu_tile = current_pause_menu_tile--;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 0.6f;
			}
		}
		else if (s_key_pressed) {
			if (current_pause_menu_tile != pause_menu_tiles.end() - 1) {
				last_pause_menu_tile = current_pause_menu_tile++;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 0.6f;
			}
		}
		else if (select_key_pressed) {
			std::string current_pause_menu_label = (*current_pause_menu_tile)->label;

			if (current_pause_menu_label == "Continue") {
				game_state = GAME_RUNNING;
				play_sound();

				last_menu_input_time = current_time;
			}
			else if (current_pause_menu_label == "Exit") {
				pause_sound();
				go_back_to_main_menu();
			}
		}
	}

	else if (game_state == GAME_MAPMAKER) {
		if (esc_key_pressed)
		glfwSetWindowShouldClose(window, true);
	}

	else if (game_state == GAME_RUNNING) {
		if (esc_key_pressed) {
			if (current_time - last_menu_input_time <= menu_input_process_delay) return;

			game_state = GAME_PAUSED;
			pause_sound();

			last_menu_input_time = current_time;
		}

		if (w_key_pressed and a_key_pressed) {
			shield.alignment = WA;
		} else if (a_key_pressed and s_key_pressed) {
			shield.alignment = SA;
		} else if (d_key_pressed and s_key_pressed) {
			shield.alignment = DS;
		} else if (w_key_pressed and d_key_pressed) {
			shield.alignment = WD;
		}

		else if (w_key_pressed) {
			shield.alignment = W;
		} else if (s_key_pressed) {
			shield.alignment = S;
		} else if (a_key_pressed) {
			shield.alignment = A;
		} else if (d_key_pressed) {
			shield.alignment = D;
		}

		is_strike_frame = (w_key_pressed) or
						  (a_key_pressed) or
						  (s_key_pressed) or
						  (d_key_pressed);
	}
}

# endif