# ifndef GAME_HPP_CHECK_FOR_COLLISIONS
# define GAME_HPP_CHECK_FOR_COLLISIONS

# include "../game.hpp"

inline void Game::check_for_collisions() {
	audio_time = calc_audio_time();

	float current_time = audio_time;
	float reaction_time = 0.160f;

	for (auto &shard : shards) {
		if (not shard.active) continue;

		float split_reaction_time = reaction_time / 2.0f;
		// FIXME: shard collision issue
		if (
			((shard.impact_time >= current_time
			and shard.impact_time - current_time <= split_reaction_time)
			or
			(shard.impact_time <= current_time
			and current_time - shard.impact_time <= split_reaction_time))
			and shield.alignment == shard.alignment
		) {
			health_point += 10;
			shard.active = false;
			shard.destroyed = true;
			continue;
		}
		else if (shard.impact_time < current_time) {
			health_point -= 15;
			shard.active = false;
			shard.destroyed = true;
			continue;
		}
	}
}

# endif