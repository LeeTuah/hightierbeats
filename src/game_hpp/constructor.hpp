# ifndef GAME_HPP_CONSTRUCTOR
# define GAME_HPP_CONSTRUCTOR

# include "../game.hpp"

inline Game::Game(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	glfwSwapInterval(0); // disable vsync

	game_state = GAME_MAIN_MENU;
	health_point = 100;
	score_point = 0;
	combo_point = 0;

	FPS_COUNTING_DELAY = 0.250f;
	fps_counter = true;
	fps = 0.0f;
	last_fps_clock_time = 0.0f;

	camera = new Camera(glm::vec3(0.0f));

	main_shader = new Shader("shaders/main.vert", "shaders/main.frag");
	text_shader = new Shader("shaders/text.vert", "shaders/text.frag");
	screen_shader = new Shader("shaders/screen.vert", "shaders/screen.frag");
	particle_shader = new Shader("shaders/particle.vert", "shaders/main.frag");
	flat_shader = new Shader("shaders/flat.vert", "shaders/flat.frag");

	generate_VAOs();

	MenuTile base_tile;
	const float menu_tiles_gap = 110.0f;

	menu_input_process_delay = 0.250f;
	last_menu_input_time = 0.0f;

	base_tile.position = glm::vec3(SCR_WIDTH - 250, SCR_HEIGHT - 200, 1.0f);
	base_tile.scale = glm::vec3(1.0f);
	base_tile.color = glm::vec3(0.7f);

	base_tile.label = "";
	base_tile.function_ptr = nullptr;
	base_tile.active = false;

	play_tile = base_tile;
	play_tile.label = "Play";

	settings_tile = base_tile;
	settings_tile.label = "Settings";
	settings_tile.position = base_tile.position - glm::vec3(0.0f, menu_tiles_gap, 0.0f);

	credits_tile = base_tile;
	credits_tile.label = "Credits";
	credits_tile.position = base_tile.position - glm::vec3(0.0f, 2 * menu_tiles_gap, 0.0f);

	exit_tile = base_tile;
	exit_tile.label = "Exit";
	exit_tile.position = base_tile.position - glm::vec3(0.0f, 3 * menu_tiles_gap, 0.0f);

	main_menu_tiles.push_back(&play_tile);
	main_menu_tiles.push_back(&settings_tile);
	main_menu_tiles.push_back(&credits_tile);
	main_menu_tiles.push_back(&exit_tile);

	current_menu_tile = main_menu_tiles.begin();
	last_menu_tile = main_menu_tiles.end();

	max_menu_scale = 1.1f;
	menu_tile_size_change = 0.8f;
	menu_scale = max_menu_scale;
	animating_menu_tile = false;

	total_menu_video_frames = 28;
	menu_video_fps = 30.0f;
	std::string current_frame;
	for (int i = 1; i <= total_menu_video_frames; i++) {
		if (i <= 9) current_frame += "0";
		current_frame += std::to_string(i);

		menu_video_frames.push_back(new Texture2D("assets/menu_background/frame_" + current_frame + ".jpg"));
		current_frame = "";
	} 

	core.position = glm::vec3(10.0f, 0.0f, 0.0f);
	core.direction = glm::vec3(0.0f);

	core.rotation_angle = 0.0f;
	core.rotation_axis = glm::vec3(1.0f);

	core.active = true; // core.active means if it will be surrounded by wireframe or not

	shield_radius = 1.5f;
	core_radius = 0.7f;
	core_color = glm::vec3(0.0f, 1.0f, 0.0f);

	CORE_SHAKE_INTENSITY = 0.3f;
	CORE_SHAKE_DECAY_SPEED = 3.95f;

	core_shake_intensity = 0.0f;
	core_offset_one = 0.0f;
	core_offset_two = 0.0f;

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

	BEAT_DISPLAY_DELAY = 0.100f;
	last_beat_status = BEAT_NULL;
	beat_clock_time = 0.0f;


	shield.position = core.position + glm::vec3(0.0f, 0.0f, -1.5f);
	shield.pivot = core.position;
	shield.alignment = W;

	shield.rotation_angle = 0.0f;
	shield.rotation_axis = glm::vec3(-1.0f, 0.0f, 0.0f);

	sun_directions.push_back(glm::vec3(+1.0f, -1.0f, +0.0f));
	sun_directions.push_back(glm::vec3(+1.0f, +1.0f, +0.0f));

	if (ma_engine_init(NULL, &audio_engine) != MA_SUCCESS) 
		std::cout << "[!] Audio engine failed to initialize!" << std::endl;
	
	if (ma_sound_init_from_file(&audio_engine, bg_song_path.c_str(), 0, NULL, NULL, &bgm) != MA_SUCCESS)
		std::cout << "[!] Failed to load audio from source file!" << std::endl;

	is_sound_playing = false;
	sound_volume = 0.1f;
	sound_pitch = 1.0f;

	black_img = new Texture2D("assets/black.png");
	background_image = new Texture2D(bg_image_path);
	background_dim = 0.7f;

	vcr_osd_mono = new character_class("assets/VCR_OSD_MONO_1.001.ttf", text_shader, &font_VAO, &font_VBO, &text_projection);
	rajdhani_regular = new character_class("assets/Rajdhani-Regular.ttf", text_shader, &font_VAO, &font_VBO, &text_projection);
}

inline Game::~Game() {
	ma_sound_uninit(&bgm);
	ma_engine_uninit(&audio_engine);
}

# endif