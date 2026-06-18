# ifndef GAME_HPP_CHECK_FOR_COLLISIONS
# define GAME_HPP_CHECK_FOR_COLLISIONS

# include "../game.hpp"

inline void Game::check_for_collisions() {
	audio_time = calc_audio_time();

	float current_time = audio_time;
	float reaction_time = 0.150f;

	for (auto &shard : shards) {
		if (not shard.active) continue;

		float split_reaction_time = reaction_time / 2.0f;
		float time_difference = abs(current_time - shard.impact_time);
		
		// FIXME: shard collision issue
		if (
			time_difference <= split_reaction_time
			and shield.alignment == shard.alignment
			and game_state == GAME_RUNNING
		) {
			if (time_difference <= split_reaction_time / 3.0f)
				score_point += BEAT_PERFECT;

			else if (time_difference <= split_reaction_time / 1.5f)
				score_point += BEAT_GREAT;

			else
				score_point += BEAT_GOOD;

			health_point += 10;
			shard.active = false;
			shard.destroyed = true; 
			continue;
		}
		else if (shard.impact_time < current_time and time_difference > split_reaction_time) {
			health_point -= 15;
			shard.active = false;
			shard.destroyed = true;
			continue;
		}
	}
}

# endif