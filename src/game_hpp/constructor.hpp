# ifndef GAME_HPP_CONSTRUCTOR
# define GAME_HPP_CONSTRUCTOR

# include "../game.hpp"

inline Game::Game(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	game_state = GAME_RUNNING;
	health_point = 100;

	camera = new Camera(glm::vec3(0.0f));

	main_shader = new Shader("shaders/main.vert", "shaders/main.frag");
	text_shader = new Shader("shaders/text.vert", "shaders/text.frag");
	screen_shader = new Shader("shaders/screen.vert", "shaders/screen.frag");
	particle_shader = new Shader("shaders/particle.vert", "shaders/main.frag");

	generate_VAOs();

	core.position = glm::vec3(10.0f, 0.0f, 0.0f);
	core.direction = glm::vec3(0.0f);

	core.rotation_angle = 0.0f;
	core.rotation_axis = glm::vec3(1.0f);

	core.active = true; // core.active means if it will be surrounded by wireframe or not

	shield_radius = 1.5f;
	core_radius = 0.7f;
	core_color = glm::vec3(0.0f, 1.0f, 0.0f);

	load_beatmap_from_file(0);

	float x_level = 10.0f, y_level = 8.0f, z_level = 14.0f;
	for (auto &shard : shards) {
		switch (shard.alignment) {
			case W:
				shard.position = glm::vec3(x_level, -y_level, 0.0f);
				break;
			
			case S:
				shard.position = glm::vec3(x_level, +y_level, 0.0f);
				break;

			case A:
				shard.position = glm::vec3(x_level, 0.0f, +z_level);
				break;

			case D:
				shard.position = glm::vec3(x_level, 0.0f, -z_level);
				break;


			case WD:
				shard.position = glm::vec3(x_level, -y_level, -z_level);
				break;

			case DS:
				shard.position = glm::vec3(x_level, +y_level, -z_level);
				break;

			case SA:
				shard.position = glm::vec3(x_level, +y_level, +z_level);
				break;

			case WA:
				shard.position = glm::vec3(x_level, -y_level, +z_level);
				break;
		} shard.direction = glm::normalize(core.position - shard.position);

		if (shard.direction.y == 0.0f and shard.direction.z != 0.0f) {
			shard.rotation_angle = 90.0f;
			shard.rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else if (shard.direction.y != 0.0f and shard.direction.z == 0.0f) {
			shard.rotation_angle = 45.0f;
			shard.rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		else if ((shard.direction.y > 0.0f and shard.direction.z < 0.0f) or (shard.direction.y < 0.0f and shard.direction.z > 0.0f)) {
			shard.rotation_angle = -45.0f;
			shard.rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else if ((shard.direction.y < 0.0f and shard.direction.z < 0.0f) or (shard.direction.y > 0.0f and shard.direction.z > 0.0f)) {
			shard.rotation_angle = 45.0f;
			shard.rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f);
		}

		shard.spawn_time = shard.impact_time - (glm::distance(core.position, shard.position) / shard.velocity);
	}

	total_losing_shards = 700;
	losing_shard_velocity = 15.0f;

	gravity_for_core_particles = 2.5f;
	is_core_shards_flying = true;
	for (int i = 0; i < total_losing_shards; i++) {
		losing_shards.push_back(core.position);
		losing_shards_directions.push_back(
			glm::normalize(glm::vec3((float)random_number() / 100.0f, (float)random_number() / 100.0f, (float)random_number() / 100.0f))
		);
	}

	shield.position = core.position + glm::vec3(0.0f, 0.0f, -1.5f);
	shield.pivot = core.position;
	shield.alignment = W;

	shield.rotation_angle = 0.0f;
	shield.rotation_axis = glm::vec3(-1.0f, 0.0f, 0.0f);

	sun_directions.push_back(glm::vec3(+1.0f, -1.0f, +0.0f));
	sun_directions.push_back(glm::vec3(+1.0f, +1.0f, +0.0f));

	if (ma_engine_init(NULL, &audio_engine) != MA_SUCCESS) 
		std::cout << "Audio engine failed to initialize!" << std::endl;
	
	if (ma_sound_init_from_file(&audio_engine, bg_song_path.c_str(), 0, NULL, NULL, &bgm) != MA_SUCCESS)
		std::cout << "Failed to load audio from source file!" << std::endl;

	is_sound_playing = false;
	sound_volume = 0.1f;
	sound_pitch = 1.0f;
}

inline Game::~Game() {
	ma_sound_uninit(&bgm);
	ma_engine_uninit(&audio_engine);
}

# endif