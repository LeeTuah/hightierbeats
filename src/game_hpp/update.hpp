# ifndef GAME_HPP_UPDATE
# define GAME_HPP_UPDATE

# include "../game.hpp"

inline void Game::update(float delta_time) {
	float current_time = audio_time;
	float glfw_current_time = glfwGetTime();

	if (health_point > 100) health_point = 100;
	else if (health_point <= 0 and game_state == GAME_RUNNING) {
		health_point = 0;
		game_state = GAME_ZERO_HP;
	}

	if (game_state == GAME_MAIN_MENU) {
		for (auto &menu_tile : main_menu_tiles)
			menu_tile->active = false;

		(*current_menu_tile)->active = true;
	}

	if (game_state == GAME_PAUSED) {
		for (auto &pause_tile : pause_menu_tiles)
			pause_tile->active = false;

		(*current_pause_menu_tile)->active = true;
	}

	if (game_state == GAME_SELECTING_BEATMAP and animating_menu_tile) {
		float gap_size = 110.0f;
		float center = (float)SCR_HEIGHT / 2.0f;

		beatmap_tile_distance_change += beatmap_tile_speed * delta_time;
		if (beatmap_tile_distance_change > 1.0f)
			beatmap_tile_distance_change = 1.0f;

		beatmap_scale += beatmap_tile_size_change * delta_time;
		if (beatmap_scale >= max_beatmap_scale)
			beatmap_scale = max_beatmap_scale;

		for (auto i = 0; i < beatmap_tiles.size(); i++) {
			float distance_from_center = i - current_user_beatmap_index;
			float prev_distance_from_center = i - last_user_beatmap_index;

			float target_y_pos = center - (distance_from_center * gap_size);
			float prev_y_pos = center - (prev_distance_from_center * gap_size);

			float del_change_in_y = (target_y_pos - prev_y_pos) * beatmap_tile_distance_change;

			beatmap_tiles[i]->position.y = prev_y_pos + del_change_in_y;
		}
	}

	if (animating_menu_tile and (game_state == GAME_MAIN_MENU or game_state == GAME_PAUSED))
		menu_scale += menu_tile_size_change * delta_time;

	if (menu_scale >= max_menu_scale and game_state == GAME_MAIN_MENU) {
		menu_scale = max_menu_scale;
		animating_menu_tile = false;
	}
	if (menu_scale >= max_pause_scale and game_state == GAME_PAUSED) {
		menu_scale = max_pause_scale;
		animating_menu_tile = false;
	}

	if (beatmap_scale >= max_beatmap_scale and game_state == GAME_SELECTING_BEATMAP and beatmap_tile_distance_change >= 1.0f) {
		beatmap_scale = max_beatmap_scale;
		animating_menu_tile = false;
		beatmap_tile_distance_change = 1.0f;
	}

	if (game_state == GAME_RUNNING or game_state == GAME_PAUSED) {
		ca_offset = max_ca_offset * (100.0f - (float)health_point) / 100.0f;
		visual_time += delta_time;

		if (abs(visual_time - current_time) > 0.025f) visual_time = current_time;

		for (auto &shard : shards) {
			if (shard.destroyed) continue;

			if (not shard.active and current_time >= shard.spawn_time)
				shard.active = true;

			if (shard.active and game_state != GAME_PAUSED) {
				float dist_from_core = (shard.impact_time - visual_time) * shard.velocity;
				shard.position = core.position - (shard.direction * dist_from_core);
			}
		}

		total_accuracy = (total_shards_destroyed != 0)? (sum_of_total_accuracy / (total_shards_destroyed * (int)ACCURACY_PERFECT)) * 100.0f : 0.0f;
		total_accuracy = std::round(total_accuracy * 10.0f) / 10.0f;

		if (ma_sound_at_end(&bgm) and game_state != GAME_ZERO_HP) {
			is_sound_playing = false;
			if (combo_point > max_combo_reached) max_combo_reached = combo_point;

			game_state = GAME_WIN;

			if (win_animation_style == SCORES_BUILD_UP)
				play_sfx(&win_sound_scores_build_up);

			else if (win_animation_style == SCORES_PUNCHED_IN)
				play_sfx(&win_sound_punched_in);
		}
	} else if (game_state == GAME_ZERO_HP) {
		int index = 0;
		for (auto &shard_pos : losing_shards) { 
			shard_pos += losing_shards_directions[index++] * losing_shard_velocity * delta_time;
			if (not is_core_shards_flying) shard_pos -= glm::vec3(0.0f, - delta_time * gravity_for_core_particles, 0.0f);
		}
		losing_shard_velocity -= 0.99 * delta_time * losing_shard_velocity;
	} else if (game_state == GAME_WIN) {
		if (win_animation_style == SCORES_BUILD_UP and (not win_screen_animation_completed)) {
			del_score += ((float)score_point / win_screen_load_time) * delta_time;
			del_accuracy += ((float)total_accuracy / win_screen_load_time) * delta_time;
			del_combo += ((float)max_combo_reached / win_screen_load_time) * delta_time;

			if (del_score > score_point) del_score = score_point;

			if (del_accuracy > total_accuracy) del_accuracy = total_accuracy;
			if (del_combo > max_combo_reached) del_combo = max_combo_reached;

			if (del_score == score_point and del_accuracy == total_accuracy and del_combo == max_combo_reached)
				win_screen_animation_completed = true;
		} else if (win_animation_style == SCORES_BUILD_UP and win_screen_animation_completed) {
			ma_sound_stop(&win_sound_scores_build_up);
		}
		else if (win_animation_style == SCORES_PUNCHED_IN and current_win_label != win_label_animation_order.end()) {
			if ((*current_win_label)->scale <= 0.6f and (not (*current_win_label)->big_label))
				(*current_win_label)->scale = 0.6f;

			if ((*current_win_label)->scale <= 5.0f and (*current_win_label)->big_label)
				(*current_win_label)->scale = 5.0f;

			if ((*current_win_label)->rotation_angle <= 0.0f)
				(*current_win_label)->rotation_angle = 0.0f;

			if (
				(*current_win_label)->scale == 0.6f and 
				(*current_win_label)->rotation_angle == 0.0f and 
				(not (*current_win_label)->big_label)
			) {
				(*current_win_label)->animated = true;
				current_win_label++;
				if (current_win_label != win_label_animation_order.end())
					play_sfx(&win_sound_punched_in);
			}
			else if (
				(*current_win_label)->scale == 5.0f and 
				(*current_win_label)->rotation_angle == 0.0f and 
				(*current_win_label)->big_label
			) {
				(*current_win_label)->animated = true;
				current_win_label++;
				if (current_win_label != win_label_animation_order.end())
					play_sfx(&win_sound_punched_in);
			}

			if (current_win_label != win_label_animation_order.end()) {
				if ((*current_win_label)->big_label)
				(*current_win_label)->scale -= (win_skill_rating_init_scale / win_label_animation_time) * delta_time;

				else
				(*current_win_label)->scale -= (win_label_init_scale / win_label_animation_time) * delta_time;

				(*current_win_label)->rotation_angle -= (win_label_init_angle / win_label_animation_time) * delta_time;
			}
		}
	} else if (game_state == GAME_MAPMAKER) {
		for (auto &shard : shards) {
			if (not shard.active and current_time >= shard.spawn_time)
				shard.active = true;

			if (shard.active and (current_time < shard.spawn_time or current_time > shard.impact_time))
				shard.active = false;

			if (shard.active and game_state != GAME_PAUSED) {
				float dist_from_core = (shard.impact_time - current_time) * shard.velocity;
				shard.position = core.position - (shard.direction * dist_from_core);
			}
		}
	}

	if (game_state == GAME_ZERO_HP)
		sound_pitch -= 0.5 * delta_time * sound_pitch;

	if (sound_pitch <= 0.09f) {
		ma_sound_stop(&bgm);
		is_sound_playing = false;
		
		sound_pitch = 1.0f;
		ma_sound_set_pitch(&bgm, sound_pitch);

		if (enable_auto_restart_on_loss) {
			load_beatmap_from_file();
			game_state = GAME_RUNNING;

			play_sound(&bgm);
		}
		else {
			game_state = GAME_MAIN_MENU;
			animating_menu_tile = false;

			current_menu_tile = main_menu_tiles.begin();
			last_menu_tile = main_menu_tiles.end();

			current_user_beatmap_index = 0;
			last_menu_input_time = glfw_current_time;
			menu_scale = max_menu_scale;

			current_pause_menu_tile = pause_menu_tiles.begin();
			last_pause_menu_tile = pause_menu_tiles.end();
			menu_input_process_delay = 0.250f;
		}
	}

	if (is_sound_playing)
		ma_sound_set_pitch(&bgm, sound_pitch);

	if (current_time - beat_clock_time > BEAT_DISPLAY_DELAY and last_beat_status != BEAT_NULL)
		last_beat_status = BEAT_NULL;

	if (fps_counter and glfw_current_time - last_fps_clock_time > FPS_COUNTING_DELAY) {
		fps = 1.0f / delta_time;
		last_fps_clock_time = glfw_current_time;
	}

	float core_shake_offset = core_shake_intensity * delta_time * CORE_SHAKE_DECAY_SPEED;
	core_offset_one = 0.0f;
	core_offset_two = 0.0f;

	if (core_shake_intensity > 0.0f) {
		core_offset_one = ((float)random_number() / 100.0f) * core_shake_intensity;
		core_offset_two = ((float)random_number() / 100.0f) * core_shake_intensity;

		core_shake_intensity -= core_shake_offset;
		if (core_shake_intensity <= 0.01f) core_shake_intensity = 0.0f;
	}
}

# endif