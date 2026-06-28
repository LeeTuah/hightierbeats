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
	max_combo_reached = 0;

	enable_auto_restart_on_loss = true;

	total_accuracy = 0.0f;
	sum_of_total_accuracy = 0.0f;
	total_shards_destroyed = 0.0f;

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
	base_tile.color = glm::vec3(0.9f);

	base_tile.label = "";
	base_tile.label_x_offset = 0.0f;

	base_tile.function_ptr = nullptr;
	base_tile.active = false;

	play_tile = base_tile;
	play_tile.label_x_offset = 3.0f;
	play_tile.label = "Play";

	settings_tile = base_tile;
	settings_tile.label = "Settings";
	settings_tile.label_x_offset = -24.0f;
	settings_tile.position = base_tile.position - glm::vec3(0.0f, menu_tiles_gap, 0.0f);

	credits_tile = base_tile;
	credits_tile.label = "Credits";
	credits_tile.label_x_offset = -20.0f;
	credits_tile.position = base_tile.position - glm::vec3(0.0f, 2 * menu_tiles_gap, 0.0f);

	exit_tile = base_tile;
	exit_tile.label = "Exit";
	exit_tile.label_x_offset = 3.0f;
	exit_tile.position = base_tile.position - glm::vec3(0.0f, 3 * menu_tiles_gap, 0.0f);

	main_menu_tiles.push_back(&play_tile);
	main_menu_tiles.push_back(&settings_tile);
	main_menu_tiles.push_back(&credits_tile);
	main_menu_tiles.push_back(&exit_tile);

	current_menu_tile = main_menu_tiles.begin();
	last_menu_tile = main_menu_tiles.end();

	animating_menu_tile = false;
	max_menu_scale = 1.1f;
	menu_tile_size_change = 0.8f;
	menu_scale = max_menu_scale;

	max_beatmap_scale = 3.3f;
	beatmap_tile_size_change = 6.6f;
	beatmap_scale = max_beatmap_scale; 

	beatmap_tile_speed = 3.2f;
	beatmap_tile_distance_change = 0.0f;

	total_menu_video_frames = 28;
	menu_video_fps = 30.0f;
	std::string current_frame;
	for (int i = 1; i <= total_menu_video_frames; i++) {
		if (i <= 9) current_frame += "0";
		current_frame += std::to_string(i);

		menu_video_frames.push_back(new Texture2D("assets/menu_background/frame_" + current_frame + ".jpg"));
		current_frame = "";
	} 

	current_user_beatmap_index = 0;

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

	total_losing_shards = 700;
	losing_shard_velocity = 15.0f;

	gravity_for_core_particles = 2.5f;
	is_core_shards_flying = true;
	for (int i = 0; i < total_losing_shards; i++)
		losing_shards_directions.push_back(
			glm::normalize(glm::vec3((float)random_number() / 100.0f, (float)random_number() / 100.0f, (float)random_number() / 100.0f))
		);

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

	is_sound_playing = false;
	sound_volume = 0.1f;
	sound_pitch = 1.0f;

	black_img = new Texture2D("assets/black.png");
	background_dim = 0.7f;
	background_image = nullptr;

	vcr_osd_mono = new character_class("assets/VCR_OSD_MONO_1.001.ttf", text_shader, &font_VAO, &font_VBO, &text_projection);
	rajdhani_regular = new character_class("assets/Rajdhani-Regular.ttf", text_shader, &font_VAO, &font_VBO, &text_projection);
}

inline Game::~Game() {
	ma_sound_uninit(&bgm);
	ma_engine_uninit(&audio_engine);
}

# endif