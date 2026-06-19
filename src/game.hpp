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

# include <algorithm>
# include <vector>
# include <map>
# include <random>

enum GameState {
	GAME_PAUSED,
	GAME_RUNNING,
	GAME_ZERO_HP
};

enum BeatTiming {
	BEAT_PERFECT = 500,
	BEAT_GREAT   = 300,
	BEAT_GOOD    = 100,
	BEAT_NULL    = 0
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
	
	bool fps_counter;
	float fps;
	float FPS_COUNTING_DELAY;
	float last_fps_clock_time;

	Camera* camera;
	int SCR_WIDTH, SCR_HEIGHT;
	
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
	
	Shard core;
	float shield_radius;
	float core_radius;
	glm::vec3 core_color;

	std::vector<Shard> shards;
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
	ma_sound bgm;
	bool is_sound_playing;
	float sound_volume, sound_pitch;
	float audio_time;

	Game(int width, int height);
	~Game();

	void load_beatmap_from_file(int index);
	void play_sound();
	float calc_audio_time();

	void process_input(GLFWwindow* window, float delta_time);

	void render();
	void check_for_collisions();
	void update(float delta_time);
private:
	void generate_VAOs();
	std::vector<float> generate_normals(std::vector<float> vertices);
};

/*
# ifndef GAME_HPP_LOAD_BEATMAP
# define GAME_HPP_LOAD_BEATMAP

# include "../game.hpp"

# endif
*/

# include "game_hpp/constructor.hpp"
# include "game_hpp/load_beatmap.hpp"
# include "game_hpp/sound.hpp"
# include "game_hpp/process_input.hpp"
# include "game_hpp/render.hpp"
# include "game_hpp/check_for_collisions.hpp"
# include "game_hpp/update.hpp"
# include "game_hpp/generate_vaos.hpp"

# endif