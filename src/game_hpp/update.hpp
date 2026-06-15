# ifndef GAME_HPP_UPDATE
# define GAME_HPP_UPDATE

# include "../game.hpp"

inline void Game::update(float delta_time) {
	float current_time = audio_time;

	if (health_point > 100) health_point = 100;
	else if (health_point < 0) health_point = 0;
	else if (health_point == 0) {
		game_state = GAME_ZERO_HP;
	}

	if (game_state == GAME_RUNNING) {
		for (auto &shard : shards) {
			if (shard.destroyed) continue;

			if (not shard.active and current_time >= shard.spawn_time)
				shard.active = true;

			if (shard.active and current_time >= shard.impact_time)
				shard.active = false;

			if (shard.active)
				shard.position += shard.direction * shard.velocity * delta_time;
		}
	} else if (game_state == GAME_ZERO_HP) {
		int index = 0;
		for (auto &shard_pos : losing_shards) { 
			shard_pos += losing_shards_directions[index++] * losing_shard_velocity * delta_time;
			if (not is_core_shards_flying) shard_pos -= glm::vec3(0.0f, - delta_time * gravity_for_core_particles, 0.0f);
		}
		losing_shard_velocity -= 0.99 * delta_time * losing_shard_velocity;
	}

	if (game_state == GAME_ZERO_HP)
		sound_pitch -= 0.5 * delta_time * sound_pitch; 

	ma_sound_set_pitch(&bgm, sound_pitch);
}

# endif