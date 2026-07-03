# ifndef GAME_HPP_PROCESS_INPUT
# define GAME_HPP_PROCESS_INPUT

# include "../game.hpp"

inline void Game::process_input(GLFWwindow* window, float delta_time) {
	float current_time = glfwGetTime();

	auto key_pressed = [&](int key) {
		return glfwGetKey(window, key) == GLFW_PRESS;
	};

	bool w_key_pressed = key_pressed(GLFW_KEY_W) or key_pressed(GLFW_KEY_UP);
	bool s_key_pressed = key_pressed(GLFW_KEY_S) or key_pressed(GLFW_KEY_DOWN);
	bool a_key_pressed = key_pressed(GLFW_KEY_A) or key_pressed(GLFW_KEY_LEFT);
	bool d_key_pressed = key_pressed(GLFW_KEY_D) or key_pressed(GLFW_KEY_RIGHT);
	bool select_key_pressed = key_pressed(GLFW_KEY_ENTER) or key_pressed(GLFW_KEY_SPACE);
	bool esc_key_pressed = key_pressed(GLFW_KEY_ESCAPE);
	bool shift_key_pressed = key_pressed(GLFW_KEY_LEFT_SHIFT) or key_pressed(GLFW_KEY_RIGHT_SHIFT);

	bool pause_key_pressed = key_pressed(GLFW_KEY_SPACE) or key_pressed(GLFW_KEY_K);
	bool fwd_key_pressed = key_pressed(GLFW_KEY_RIGHT) or key_pressed(GLFW_KEY_L);
	bool bwd_key_pressed = key_pressed(GLFW_KEY_LEFT) or key_pressed(GLFW_KEY_J);
	bool place_shard_key_pressed = key_pressed(GLFW_KEY_Q);
	bool delete_shard_key_pressed = key_pressed(GLFW_KEY_E);

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
		menu_input_process_delay = 0.250f;
	};

	if (game_state == GAME_MAIN_MENU) {
		if (current_time - last_menu_input_time <= menu_input_process_delay) return;

		if (w_key_pressed) {
			if (current_menu_tile != main_menu_tiles.begin()) {
				last_menu_tile = current_menu_tile--;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 1.0f;

				play_sfx(&move_cursor_sound);
			}
		}
		else if (s_key_pressed) {
			if (current_menu_tile != main_menu_tiles.end() - 1) {
				last_menu_tile = current_menu_tile++;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 1.0f;

				play_sfx(&move_cursor_sound);
			}
		}
		else if (select_key_pressed) {
			std::string current_menu_label = (*current_menu_tile)->label;

			play_sfx(&click_cursor_sound);
			if (current_menu_label == "Play") {
				game_state = GAME_SELECTING_BEATMAP;
				load_all_beatmaps();
				last_menu_input_time = current_time;
			}
			else if (current_menu_label == "Settings") {

			}
			else if (current_menu_label == "Mapmaker") {
				game_state = GAME_MAPMAKER;
				
				menu_input_process_delay = 0.170f;
				shards.clear();
				mapmaker_play_music = false;

				is_folder_loaded = false;
				is_background_loaded = false;
				is_song_loaded = false;

				play_button_label = "►";

				loaded_folder_name = "";
				loaded_bg_path = "";
				loaded_song_path = "";

				base_velocity = 8.0f;
				velocity_multiplier = 1.0f;

				song_name = "";
				artist_name = "";
				creator_name = "";

				current_alignment_selected_int = 0;
				current_alignment_selected = WA;

				play_pause_pressed = false;
				snap_fwd_pressed = false;
				snap_bwd_pressed = false;
				next_shard_pressed = false;
				prev_shard_pressed = false;
				place_shard_pressed = false;
				delete_shard_pressed = false;
			}
			else if (current_menu_label == "Exit") {
				glfwSetWindowShouldClose(window, true);
			}
		}
		if (esc_key_pressed) {
			play_sfx(&close_cursor_sound);
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
				play_sfx(&move_cursor_sound);
			}
		}
		else if (s_key_pressed) {
			if (current_user_beatmap_index < beatmap_tiles.size() - 1) {
				last_user_beatmap_index = current_user_beatmap_index++;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				beatmap_scale = 1.0f;
				beatmap_tile_distance_change = 0.0f;
				play_sfx(&move_cursor_sound);
			}
		}
		else if (select_key_pressed) {
			play_sfx(&click_cursor_sound);
			load_beatmap_from_file();
			game_state = GAME_RUNNING;

			play_sound(&bgm);
		}
		if (esc_key_pressed) {
			play_sfx(&close_cursor_sound);
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
			play_sfx(&close_cursor_sound);
			game_state = GAME_RUNNING;
			play_sound(&bgm);

			last_menu_input_time = current_time;
		}

		if (w_key_pressed) {
			if (current_pause_menu_tile != pause_menu_tiles.begin()) {
				last_pause_menu_tile = current_pause_menu_tile--;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 0.6f;
				play_sfx(&move_cursor_sound);
			}
		}
		else if (s_key_pressed) {
			if (current_pause_menu_tile != pause_menu_tiles.end() - 1) {
				last_pause_menu_tile = current_pause_menu_tile++;
				last_menu_input_time = current_time;

				animating_menu_tile = true;
				menu_scale = 0.6f;
				play_sfx(&move_cursor_sound);
			}
		}
		else if (select_key_pressed) {
			std::string current_pause_menu_label = (*current_pause_menu_tile)->label;

			if (current_pause_menu_label == "Continue") {
				play_sfx(&click_cursor_sound);
				game_state = GAME_RUNNING;
				play_sound(&bgm);

				last_menu_input_time = current_time;
			}
			else if (current_pause_menu_label == "Exit") {
				play_sfx(&close_cursor_sound);
				pause_sound(&bgm);
				go_back_to_main_menu();
			}
		}
	}

	else if (game_state == GAME_MAPMAKER) {
		if (current_time - last_menu_input_time <= menu_input_process_delay) return;

		if (esc_key_pressed) {
			play_sfx(&close_cursor_sound);
			go_back_to_main_menu();
		}
		else if (pause_key_pressed) {
			play_pause_pressed = true;
			last_menu_input_time = current_time;
		}
		else if (shift_key_pressed and fwd_key_pressed) {
			next_shard_pressed = true;
			last_menu_input_time = current_time;
		}
		else if (shift_key_pressed and bwd_key_pressed) {
			prev_shard_pressed = true;
			last_menu_input_time = current_time;
		}
		else if (fwd_key_pressed) {
			snap_fwd_pressed = true;
			last_menu_input_time = current_time;
		}
		else if (bwd_key_pressed) {
			snap_bwd_pressed = true;
			last_menu_input_time = current_time;
		}
		else if (place_shard_key_pressed) {
			place_shard_pressed = true;
			last_menu_input_time = current_time;
		}
		else if (delete_shard_key_pressed) {
			delete_shard_pressed = true;
			last_menu_input_time = current_time;
		}
	}

	else if (game_state == GAME_RUNNING) {
		if (esc_key_pressed) {
			if (current_time - last_menu_input_time <= menu_input_process_delay) return;

			game_state = GAME_PAUSED;
			pause_sound(&bgm);

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