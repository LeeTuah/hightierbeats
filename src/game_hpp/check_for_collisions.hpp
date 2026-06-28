# ifndef GAME_HPP_CHECK_FOR_COLLISIONS
# define GAME_HPP_CHECK_FOR_COLLISIONS

# include "../game.hpp"

inline void Game::check_for_collisions() {
	audio_time = calc_audio_time();

	float current_time = audio_time;
	float reaction_time = 0.200f;

	auto update_health = [&](int sign, Shard &shard, BeatTiming beat_status) {
		if (sign > 0) {
			if (beat_status == BEAT_PERFECT) health_point += 10;
			else if (beat_status == BEAT_GREAT) health_point += 5;
			else if (beat_status == BEAT_GOOD) health_point += 2;

			combo_point++;
		} else {
			health_point -= 15;

			if (combo_point > max_combo_reached) max_combo_reached = combo_point;
			combo_point = 0;
		}
		shard.active = false;
		shard.destroyed = true;
		total_shards_destroyed++;

		if (beat_status != BEAT_NULL) {
			last_beat_status = beat_status;
			beat_clock_time = current_time;
		}
	};

	for (auto &shard : shards) {
		if (not shard.active) continue;

		float split_reaction_time = reaction_time / 2.0f;
		float time_difference = abs(current_time - shard.impact_time);
		
		if (
			is_strike_frame and
			time_difference <= split_reaction_time
			and shield.alignment == shard.alignment
			and game_state == GAME_RUNNING
		) {
			BeatTiming beat_status;
			if (time_difference <= split_reaction_time / 3.0f) {
				beat_status = BEAT_PERFECT;
				sum_of_total_accuracy += ACCURACY_PERFECT;
			}

			else if (time_difference <= split_reaction_time / 1.5f) {
				beat_status = BEAT_GREAT;
				sum_of_total_accuracy += ACCURACY_GREAT;
			}

			else {
				beat_status = BEAT_GOOD;
				sum_of_total_accuracy += ACCURACY_GOOD;
			}

			float combo_decay = (combo_point > 1)? 2.0f / (float)combo_point : 1.0f;
			score_point += beat_status + (int)(beat_status * combo_decay);
			
			update_health(1, shard, beat_status);
			continue;
		}
		else if (shard.impact_time < current_time and time_difference > split_reaction_time) {
			core_shake_intensity = CORE_SHAKE_INTENSITY;
			
			update_health(-1, shard, BEAT_NULL);
			continue;
		}
	}
}

# endif