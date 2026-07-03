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
	win_animation_style = SCORES_PUNCHED_IN; // TODO: SCORES_PUNCHED_IN sound effect

	total_accuracy = 0.0f;
	sum_of_total_accuracy = 0.0f;
	total_shards_destroyed = 0.0f;

	FPS_COUNTING_DELAY = 0.250f;
	fps_counter = true;
	fps = 0.0f;
	last_fps_clock_time = 0.0f;

	win_screen_load_time = 2.0f;
	win_screen_animation_completed = false;

	del_score = 0;
	del_accuracy = 0;
	del_combo = 0;

	win_label_animation_time = 0.5f;

	win_label_init_scale = 0.95f;
	win_skill_rating_init_scale = 8.7f;
	win_label_init_angle = glm::radians(75.0f);

	current_win_label = win_label_animation_order.begin();

	float skill_rating_x_offset = -65.0f, skill_rating_scale = 5.0f;
	float labels_x_offset = -195.0f, label_y_offset = -100.0f, label_gap = -50.0f;

	WinLabels base_label;
	base_label.position = glm::vec3(SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset, 0.0f);
	base_label.scale = win_label_init_scale;
	base_label.rotation_angle = win_label_init_angle;
	base_label.animated = false;
	base_label.big_label = false;
	base_label.label = "";
	base_label.x_offset = 0.0f;

	WinLabels* score_label = new WinLabels(base_label);
	score_label->label = "Score";
	score_label->x_offset = 130.0f;

	WinLabels* accuracy_label = new WinLabels(base_label);
	accuracy_label->position.y += label_gap;
	accuracy_label->label = "Accuracy";
	accuracy_label->x_offset = 147.0f;

	WinLabels* max_combo_label = new WinLabels(base_label);
	max_combo_label->position.y += 2 * label_gap;
	max_combo_label->label = "Max Combo";
	max_combo_label->x_offset = 130.0f;

	WinLabels* press_enter_label = new WinLabels(base_label);
	press_enter_label->position.y += 3 * label_gap;
	press_enter_label->label = "Press ENTER to continue";
	press_enter_label->x_offset = 199.0f;

	WinLabels* skill_rating_label = new WinLabels(base_label);
	skill_rating_label->position = glm::vec3(SCR_WIDTH / 2 + skill_rating_x_offset, SCR_HEIGHT / 2 + 50.0f, 0.0f);
	skill_rating_label->scale = win_skill_rating_init_scale;
	skill_rating_label->big_label = true;
	skill_rating_label->x_offset = 145.0f;

	win_label_animation_order.push_back(score_label);
	win_label_animation_order.push_back(accuracy_label);
	win_label_animation_order.push_back(max_combo_label);
	win_label_animation_order.push_back(skill_rating_label);
	win_label_animation_order.push_back(press_enter_label);

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

	mapmaker_tile = base_tile;
	mapmaker_tile.label = "Mapmaker";
	mapmaker_tile.label_x_offset = -20.0f;
	mapmaker_tile.position = base_tile.position - glm::vec3(0.0f, 2 * menu_tiles_gap, 0.0f);

	exit_tile = base_tile;
	exit_tile.label = "Exit";
	exit_tile.label_x_offset = 3.0f;
	exit_tile.position = base_tile.position - glm::vec3(0.0f, 3 * menu_tiles_gap, 0.0f);

	main_menu_tiles.push_back(&play_tile);
	main_menu_tiles.push_back(&settings_tile);
	main_menu_tiles.push_back(&mapmaker_tile);
	main_menu_tiles.push_back(&exit_tile);

	current_menu_tile = main_menu_tiles.begin();
	last_menu_tile = main_menu_tiles.end();

	base_tile.position = glm::vec3(SCR_WIDTH / 2, SCR_HEIGHT / 2 + 10.0f, 1.0f);
	base_tile.scale = glm::vec3(0.6f);
	base_tile.color = glm::vec3(0.9f);

	base_tile.label = "";
	base_tile.label_x_offset = 0.0f;

	base_tile.function_ptr = nullptr;
	base_tile.active = false;

	continue_tile = base_tile;
	continue_tile.label_x_offset = -27.0f;
	continue_tile.label = "Continue";

	exit_level_tile = base_tile;
	exit_level_tile.position = base_tile.position - glm::vec3(0.0f, 70.0f, 0.0f);
	exit_level_tile.label_x_offset = 3.0f;
	exit_level_tile.label = "Exit";

	pause_menu_tiles.push_back(&continue_tile);
	pause_menu_tiles.push_back(&exit_level_tile);

	current_pause_menu_tile = pause_menu_tiles.begin();
	last_pause_menu_tile = pause_menu_tiles.end();

	animating_menu_tile = false;
	max_menu_scale = 1.1f;
	max_pause_scale = 0.7f;
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

	is_sound_playing = false;
	sound_volume = 0.1f;
	sfx_volume = 1.0f;
	sound_pitch = 1.0f;

	if (ma_engine_init(NULL, &audio_engine) != MA_SUCCESS) 
		std::cout << "[!] Audio engine failed to initialize!" << std::endl;

	if (ma_sound_init_from_file(&audio_engine, "assets/move_cursor.mp3", 0, 0, NULL, &move_cursor_sound) != MA_SUCCESS) {
		std::cout << "[!] Move cursor sound failed to load!" << std::endl;
	}

	if (ma_sound_init_from_file(&audio_engine, "assets/click_cursor.mp3", 0, 0, NULL, &click_cursor_sound) != MA_SUCCESS) {
		std::cout << "[!] Click cursor sound failed to load!" << std::endl;
	}

	if (ma_sound_init_from_file(&audio_engine, "assets/close_down.mp3", 0, 0, NULL, &close_cursor_sound) != MA_SUCCESS) {
		std::cout << "[!] Close cursor sound failed to load!" << std::endl;
	}

	if (ma_sound_init_from_file(&audio_engine, "assets/scoreboard_going_up.mp3", 0, 0, NULL, &win_sound_scores_build_up) != MA_SUCCESS) {
		std::cout << "[!] Scoreboard going up sound failed to load!" << std::endl;
	}

	if (ma_sound_init_from_file(&audio_engine, "assets/punch_in_sound.mp3", 0, 0, NULL, &win_sound_punched_in) != MA_SUCCESS) {
		std::cout << "[!] Scoreboard punch in sound failed to load!" << std::endl;
	}

	for (int i = 0; i < TOTAL_HIT_SOUNDS; i++) {
		if (ma_sound_init_from_file(&audio_engine, "assets/glass_hitting_metal.wav", 0, 0, NULL, &hit_sounds[i]) != MA_SUCCESS)
			std::cout << "[!] Hit sound " << i + 1 <<" failed to load!" << std::endl;
	}

	for (int i = 0; i < TOTAL_HIT_SOUNDS; i++) {
		if (ma_sound_init_from_file(&audio_engine, "assets/core_taking_damage.wav", 0, 0, NULL, &dmg_sounds[i]) != MA_SUCCESS)
			std::cout << "[!] Damage sound " << i + 1 <<" failed to load!" << std::endl;
	}

	black_img = new Texture2D("assets/black.png");
	background_dim = 0.7f;
	background_image = nullptr;

	vcr_osd_mono = new character_class("assets/VCR_OSD_MONO_1.001.ttf", text_shader, &font_VAO, &font_VBO, &text_projection);
	rajdhani_regular = new character_class("assets/Rajdhani-Regular.ttf", text_shader, &font_VAO, &font_VBO, &text_projection);

	mapmaker_font_size = 14.0f;

	mapmaker_timeline_opened = true;
	mapmaker_properties_opened = true;
	mapmaker_play_music = false;

	is_folder_loaded = false;
	is_background_loaded = false;
	is_song_loaded = false;

	play_button_label = "►";

	loaded_folder_name = "";

	loaded_bg_path = "";
	loaded_song_path = "";

	base_velocity = 8.0f;
	velocity_multiplier = 1.0f;

	song_name = "";
	artist_name = "";
	creator_name = "";

	beats_per_minute = 120.0f;
	song_offset = 0.0f;
	song_divisor = 1.0f;

	current_alignment_selected_int = 0;
	song_divisor_int = 0;
	current_alignment_selected = WA;

	play_pause_pressed = false;
	snap_fwd_pressed = false;
	snap_bwd_pressed = false;
	next_shard_pressed = false;
	prev_shard_pressed = false;
	place_shard_pressed = false;
	delete_shard_pressed = false;
}

inline Game::~Game() {
	ma_sound_uninit(&bgm);
	ma_engine_uninit(&audio_engine);

	for (auto label : win_label_animation_order)
		delete label;
}

# endif

