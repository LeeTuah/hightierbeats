# ifndef GAME_HPP_SOUND
# define GAME_HPP_SOUND

# include "../game.hpp"

inline void Game::play_sound() {
	ma_sound_set_volume(&bgm, sound_volume);
	ma_sound_start(&bgm);

	is_sound_playing = true;
}

inline void Game::pause_sound() {
	ma_sound_stop(&bgm);
	is_sound_playing = false;
}

inline float Game::calc_audio_time() {
	if (not is_sound_playing) return 0.0f;

	ma_uint64 cursor_pcm;
	ma_sound_get_cursor_in_pcm_frames(&bgm, &cursor_pcm);

	ma_uint32 sample_rate = ma_engine_get_sample_rate(&audio_engine);

	return (float)cursor_pcm / (float)sample_rate;
}

# endif