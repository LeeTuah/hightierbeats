# ifndef GAME_HPP_UPDATE
# define GAME_HPP_UPDATE

# include "../game.hpp"

inline void Game::update(float delta_time) {
	float current_time = audio_time;

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

	if (animating_menu_tile and game_state == GAME_MAIN_MENU)
		menu_scale += menu_tile_size_change * delta_time;

	if (menu_scale >= max_menu_scale and game_state == GAME_MAIN_MENU) {
		menu_scale = max_menu_scale;
		animating_menu_tile = false;
	}

	if (beatmap_scale >= max_beatmap_scale and game_state == GAME_SELECTING_BEATMAP and beatmap_tile_distance_change >= 1.0f) {
		beatmap_scale = max_beatmap_scale;
		animating_menu_tile = false;
		beatmap_tile_distance_change = 1.0f;
	}

	if (game_state == GAME_RUNNING) {
		for (auto &shard : shards) {
			if (shard.destroyed) continue;

			if (not shard.active and current_time >= shard.spawn_time)
				shard.active = true;

			if (shard.active)
				shard.position += shard.direction * shard.velocity * delta_time;
		}

		total_accuracy = (total_shards_destroyed != 0)? (sum_of_total_accuracy / (total_shards_destroyed * ACCURACY_PERFECT)) * 100.0f : 0.0f;
		total_accuracy = std::round(total_accuracy * 10.0f) / 10.0f;

		if (ma_sound_at_end(&bgm) and not game_state == GAME_ZERO_HP) {
			is_sound_playing = false;
			if (combo_point > max_combo_reached) max_combo_reached = combo_point;

			game_state = GAME_WIN;
		}
	} else if (game_state == GAME_ZERO_HP) {
		int index = 0;
		for (auto &shard_pos : losing_shards) { 
			shard_pos += losing_shards_directions[index++] * losing_shard_velocity * delta_time;
			if (not is_core_shards_flying) shard_pos -= glm::vec3(0.0f, - delta_time * gravity_for_core_particles, 0.0f);
		}
		losing_shard_velocity -= 0.99 * delta_time * losing_shard_velocity;
	} else if (game_state == GAME_WIN) {

	}

	if (game_state == GAME_ZERO_HP)
		sound_pitch -= 0.5 * delta_time * sound_pitch;

	if (sound_pitch <= 0.09f) {
		ma_sound_stop(&bgm);
		is_sound_playing = false;

		if (game_state == GAME_ZERO_HP){
			game_state = GAME_MAIN_MENU;

			animating_menu_tile = false;

			current_menu_tile = main_menu_tiles.begin();
			last_menu_tile = main_menu_tiles.end();

			current_user_beatmap_index = 0;
		}
	}

	if (is_sound_playing)
		ma_sound_set_pitch(&bgm, sound_pitch);

	if (current_time - beat_clock_time > BEAT_DISPLAY_DELAY and last_beat_status != BEAT_NULL)
		last_beat_status = BEAT_NULL;

	if (fps_counter and current_time - last_fps_clock_time > FPS_COUNTING_DELAY) {
		fps = 1.0f / delta_time;
		last_fps_clock_time = current_time;
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