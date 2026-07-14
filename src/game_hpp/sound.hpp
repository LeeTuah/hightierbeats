# ifndef GAME_HPP_SOUND
# define GAME_HPP_SOUND

# include "../game.hpp"

inline void Game::play_sound(ma_sound *sound) {
	ma_sound_set_volume(&bgm, sound_volume);
	ma_sound_start(&bgm);

	is_sound_playing = true;
}

inline void Game::play_sfx(ma_sound *sfx) {
	ma_sound_stop(sfx);
	ma_sound_seek_to_pcm_frame(sfx, 0);
	ma_sound_set_volume(sfx, sfx_volume);
	ma_sound_start(sfx);
}

inline void Game::pause_sound(ma_sound *sound) {
	ma_sound_stop(&bgm);
	is_sound_playing = false;
}

inline float Game::get_audio_length(ma_sound *sound) {
	ma_uint64 song_length_in_frames;
	ma_uint32 sample_rate = ma_engine_get_sample_rate(&audio_engine);

	ma_sound_get_length_in_pcm_frames(sound, &song_length_in_frames);

	return (float)song_length_in_frames / (float)sample_rate;
}

inline float Game::calc_audio_time() {
	if (not is_sound_playing and game_state != GAME_MAPMAKER) return 0.0f;

	ma_uint64 cursor_pcm = 0;
	ma_sound_get_cursor_in_pcm_frames(&bgm, &cursor_pcm);

	ma_uint32 sample_rate = ma_engine_get_sample_rate(&audio_engine);

	return (float)cursor_pcm / (float)sample_rate;
}

inline void Game::set_audio_time(ma_sound* sound, float timestamp) {
	ma_uint32 sample_rate = ma_engine_get_sample_rate(&audio_engine);
	ma_uint64 target_frame = (float)sample_rate * timestamp;

	ma_sound_seek_to_pcm_frame(sound, target_frame);
}

# endif