# ifndef GAME_HPP_
# define GAME_HPP_

# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include <nlohmann/json.hpp>
# include <fstream>
# include <sstream>
# include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

# include "includes/camera.hpp"
# include "includes/shaders.hpp"
# include "includes/texture.hpp"

# include "includes/miniaudio.h"
# include "imgui.h"
# include "imgui_stdlib.h"

# include <algorithm>
# include <vector>
# include <map>
# include <random>
# include <any>
# include <functional>
# include <format>

# define TOTAL_HIT_SOUNDS 22

enum GameState {
	GAME_MAIN_MENU,
	GAME_SELECTING_BEATMAP,
	GAME_MAPMAKER,
	GAME_PAUSED,
	GAME_RUNNING,
	GAME_WIN,
	GAME_ZERO_HP
};

enum BeatTiming {
	BEAT_PERFECT = 500,
	BEAT_GREAT   = 300,
	BEAT_GOOD    = 100,
	BEAT_NULL    = 0
};

enum BeatAccuracy {
	ACCURACY_PERFECT = 100,
	ACCURACY_GREAT   = 80,
	ACCURACY_GOOD    = 60,
	ACCURACY_NULL    = 0
};

enum SkillRatings{
	SKILL_SSP = 99,
	SKILL_SS  = 95,
	SKILL_S   = 90,
	SKILL_A   = 80,
	SKILL_B   = 70,
	SKILL_C   = 60,
	SKILL_D   = 50,
	SKILL_F   = 40
};

enum ShieldAlignment {
	W  =   90,
	A  =  180,
	S  =  -90,
	D  =    0,
	WD =   45,
	DS =  -45,
	SA = -135,
	WA =  135
};

enum WinAnimationStyle {
	SCORES_BUILD_UP,
	SCORES_PUNCHED_IN
};

struct WinLabels {
	glm::vec3 position;
	float scale;
	float rotation_angle;

	bool animated;
	bool big_label;
	std::string label;

	float x_offset;
};

struct MenuTile {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 color;

	std::string label;
	float label_x_offset;
	std::any function_ptr;

	bool active;
};

struct BeatmapTile : MenuTile {
	int index;
};

struct Shard {
	glm::vec3 position;
	ShieldAlignment alignment;
	glm::vec3 direction;
	float velocity;

	float rotation_angle;
	glm::vec3 rotation_axis;

	float impact_time;
	float spawn_time;

	bool active;
	bool destroyed;
};

struct Shield {
	glm::vec3 position;
	glm::vec3 pivot;

	float rotation_angle;
	glm::vec3 rotation_axis;

	ShieldAlignment alignment;
	
	int segments;
};

int random_number(int begin = -100, int end = 100) {
	std::random_device rd;
	std::mt19937 mt(rd());

	std::uniform_int_distribution<> distribution(begin, end);
	return distribution(mt);
}

class Game {
public:
	GameState game_state;

	int health_point;
	int score_point;
	int combo_point;
	bool is_strike_frame;
	int max_combo_reached;

	bool enable_auto_restart_on_loss;
	WinAnimationStyle win_animation_style;

	float sum_of_total_accuracy, total_shards_destroyed;
	float total_accuracy;
	
	bool fps_counter;
	float fps;
	float FPS_COUNTING_DELAY;
	float last_fps_clock_time;

	float win_screen_load_time;
	float del_score, del_combo, del_accuracy;
	float win_screen_animation_completed;

	float win_label_animation_time;
	float win_label_init_scale, win_skill_rating_init_scale, win_label_init_angle;
	std::vector<WinLabels*> win_label_animation_order;
	std::vector<WinLabels*>::iterator current_win_label;

	Camera* camera;
	int SCR_WIDTH, SCR_HEIGHT;
	
	unsigned int menu_tile_VAO, menu_tile_VBO;
	unsigned int shard_VAO, shard_VBO;
	unsigned int core_particle_VAO, core_particle_VBO, core_particle_VBO2;
	unsigned int core_VAO, core_VBO;
	unsigned int shield_VAO, shield_VBO;
	unsigned int square_VAO, square_VBO;
	unsigned int rect_VAO, rect_VBO;
	unsigned int font_VAO, font_VBO;

	Shader* main_shader;
	Shader* text_shader;
	Shader* screen_shader;
	Shader* particle_shader;
	Shader* flat_shader;

	glm::mat4 view, projection, model, text_projection;
	std::vector<glm::vec3> sun_directions;
	
	MenuTile play_tile, settings_tile, mapmaker_tile, exit_tile;
	std::vector<MenuTile*> main_menu_tiles;
	std::vector<MenuTile*>::iterator current_menu_tile, last_menu_tile;
	
	MenuTile continue_tile, exit_level_tile;
	std::vector<MenuTile*> pause_menu_tiles;
	std::vector<MenuTile*>::iterator current_pause_menu_tile, last_pause_menu_tile;

	float menu_input_process_delay;
	float last_menu_input_time;
	
	bool animating_menu_tile;
	float max_menu_scale, max_pause_scale;
	float menu_scale;
	float menu_tile_size_change;

	float max_beatmap_scale;
	float beatmap_scale;
	float beatmap_tile_size_change;

	float beatmap_tile_distance_change;
	float beatmap_tile_speed;

	std::vector<Texture2D*> menu_video_frames;
	int total_menu_video_frames;
	float menu_video_fps;

	std::vector<Texture2D*> all_beatmaps_backgrounds;

	Shard core;
	float shield_radius;
	float core_radius;
	glm::vec3 core_color;

	float CORE_SHAKE_INTENSITY;
	float CORE_SHAKE_DECAY_SPEED;
	float core_shake_intensity;
	float core_offset_one, core_offset_two;

	int current_user_beatmap_index, last_user_beatmap_index;
	std::vector<BeatmapTile*> beatmap_tiles;

	std::vector<Shard> shards;
	float shard_radius;
	Shield shield;

	std::vector<glm::vec3> losing_shards;
	std::vector<glm::vec3> losing_shards_directions;
	int total_losing_shards;
	float losing_shard_velocity;
	float gravity_for_core_particles;
	bool is_core_shards_flying;

	BeatTiming last_beat_status;
	float beat_clock_time;
	float BEAT_DISPLAY_DELAY;

	std::string bg_song_path;
	std::string bg_image_path;

	Texture2D *background_image;
	Texture2D *black_img;
	float background_dim;

	character_class *vcr_osd_mono;
	character_class *rajdhani_regular;

	ma_engine audio_engine;
	ma_sound bgm, move_cursor_sound, click_cursor_sound, close_cursor_sound;
	ma_sound win_sound_scores_build_up, win_sound_punched_in;

	ma_sound hit_sounds[TOTAL_HIT_SOUNDS];
	ma_sound dmg_sounds[TOTAL_HIT_SOUNDS];
	int current_hit_sound_index, current_dmg_sound_index;

	bool is_sound_playing;
	float sound_volume, sound_pitch, sfx_volume;
	float audio_time, visual_time;

	float mapmaker_font_size;
	bool mapmaker_timeline_opened, mapmaker_properties_opened;
	bool mapmaker_play_music;

	bool is_background_loaded, is_song_loaded, is_folder_loaded;
	std::string play_button_label;
	std::string loaded_bg_path, loaded_song_path, loaded_folder_name;
	float base_velocity, velocity_multiplier;
	std::string song_name, artist_name, creator_name;
	float beats_per_minute, song_offset, song_divisor;

	bool play_pause_pressed;
	bool snap_fwd_pressed, snap_bwd_pressed;
	bool next_shard_pressed, prev_shard_pressed;
	bool place_shard_pressed, delete_shard_pressed;
	bool esc_mapmaker_key_confirmed, esc_mapmaker_key_pressed;

	ShieldAlignment current_alignment_selected;
	int current_alignment_selected_int, song_divisor_int;

	Shard current_shard;

	Game(int width, int height);
	~Game();

	void load_beatmap_from_file();
	void load_all_beatmaps();
	Shard generate_shard_data(Shard shard, glm::vec3 core_elevation = glm::vec3(0.0f));

	void play_sound(ma_sound *sound);
	void play_sfx(ma_sound *sfx);
	void pause_sound(ma_sound *sound);
	float calc_audio_time();
	float get_audio_length(ma_sound *sound);
	void set_audio_time(ma_sound* sound, float timestamp);

	void process_input(GLFWwindow* window, float delta_time);

	void render_menu();
	void render_game();
	void render_mapmaker();

	void check_for_collisions();
	void update(float delta_time);
private:
	void generate_VAOs();
	std::vector<float> generate_normals(std::vector<float> vertices);
};

# include "game_hpp/constructor.hpp"
# include "game_hpp/load_beatmap.hpp"
# include "game_hpp/sound.hpp"
# include "game_hpp/process_input.hpp"
# include "game_hpp/render_menu.hpp"
# include "game_hpp/render_game.hpp"
# include "game_hpp/render_mapmaker.hpp"
# include "game_hpp/check_for_collisions.hpp"
# include "game_hpp/update.hpp"
# include "game_hpp/generate_vaos.hpp"

# endif