# ifndef GAME_HPP_LOAD_BEATMAP
# define GAME_HPP_LOAD_BEATMAP

# include "../game.hpp"

inline void Game::load_all_beatmaps() {
	fs::path beatmap_path = "beatmaps/";

	try {
		beatmap_tiles.clear();

		unsigned index = 0;
		for (auto &beatmap : fs::directory_iterator(beatmap_path)) {
			if (not fs::is_directory(beatmap)) continue;

			std::ifstream beatmap_stream;
			std::stringstream beatmap_stringstream;
			std::string beatmap_json_data = "", beatmap_name = "", beatmap_folder_name = "";

			json beatmap_data;
			Texture2D* img_background = nullptr;

			beatmap_name = beatmap.path().filename().string();
			beatmap_folder_name = beatmap_name;
			beatmap_stream.open(("beatmaps/" + beatmap_name + "/" + beatmap_name + ".json").c_str());

			if (beatmap_stream.is_open()) {
				beatmap_stringstream << beatmap_stream.rdbuf();
				beatmap_stream.close();

				beatmap_json_data = beatmap_stringstream.str();
				beatmap_data = json::parse(beatmap_json_data);

				if (beatmap_data.contains("name") and beatmap_data.contains("artist")) {
					if (not beatmap_data["name"].get<std::string>().empty())
						beatmap_name = beatmap_data["name"].get<std::string>();

					if (not beatmap_data["artist"].get<std::string>().empty())
						beatmap_name += " - " + beatmap_data["artist"].get<std::string>();
				}

				if (beatmap_data.contains("background") and (not beatmap_data["background"].get<std::string>().empty()))
					if (fs::exists("beatmaps/" + beatmap_folder_name + "/" + beatmap_data["background"].get<std::string>()))
					img_background = new Texture2D("beatmaps/" + beatmap_folder_name + "/" + beatmap_data["background"].get<std::string>());
			}

			all_beatmaps_backgrounds.push_back(img_background);

			BeatmapTile *bmt = new BeatmapTile();
			bmt->position = glm::vec3(SCR_WIDTH - 250, 0.0f, 0.0f);
			bmt->scale = glm::vec3(1.0f);
			bmt->color = glm::vec3(1.0f);

			bmt->label = beatmap_name;
			bmt->label_x_offset = -50.0f;

			bmt->function_ptr = nullptr;
			bmt->active = false;
		
			bmt->index = index++;

			beatmap_tiles.push_back(bmt);
		}
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}

	float gap_size = 110.0f;
	float center = (float)SCR_HEIGHT / 2.0f;

	for (auto i = 0; i < beatmap_tiles.size(); i++) {
		float distance_from_center = i - current_user_beatmap_index;
		float target_y_pos = center - (distance_from_center * gap_size);

		beatmap_tiles[i]->position.y = target_y_pos;
	}
}

inline void Game::load_beatmap_from_file() {
	int index = current_user_beatmap_index;

	fs::path beatmap_path = "beatmaps/";
	std::string beatmap_name = "";
	int beatmap_index = 0;

	try {
		for (auto &beatmap : fs::directory_iterator(beatmap_path)) {
			if (not fs::is_directory(beatmap)) continue;
			if (index != beatmap_index++) continue;

			beatmap_name = beatmap.path().filename().string();
			break;
		}
	} catch (fs::filesystem_error &e) {
		std::cout << e.what() << std::endl;
	}
	
	std::ifstream beatmap_file;
	std::stringstream beatmap_stringstream;
	std::string beatmap_data = "";

	beatmap_file.open(("beatmaps/" + beatmap_name + "/" + beatmap_name + ".json").c_str());
	if (not beatmap_file.is_open()){
		std::cout << "[!] Failed to open beatmap file!" << std::endl;
		return;
	}

	beatmap_stringstream << beatmap_file.rdbuf();
	beatmap_file.close();
	beatmap_data = beatmap_stringstream.str();

	json beatmap_json = json::parse(beatmap_data);
	if (not bg_song_path.empty()) ma_sound_uninit(&bgm);

	bg_song_path = beatmap_path.string() + beatmap_name + "/" + beatmap_json["audio"].get<std::string>();
	bg_image_path = beatmap_path.string() + beatmap_name + "/" + beatmap_json["background"].get<std::string>();

	if (ma_sound_init_from_file(&audio_engine, bg_song_path.c_str(), 0, 0, NULL, &bgm) != MA_SUCCESS)
		std::cout << "[!] Failed to initialize the audio file!" << std::endl;

	if (background_image != nullptr) delete background_image;
	background_image = new Texture2D(bg_image_path);

	shards.clear();

	Shard base_shard;
	base_shard.active = false;
	base_shard.destroyed = false;
	base_shard.velocity = beatmap_json["base_velocity"];
	float base_velocity = base_shard.velocity;

	for (auto note : beatmap_json["notes"]) {
		base_shard.velocity = base_velocity;
		
		base_shard.impact_time = note["time"];
		std::string alignment = note["alignment"].get<std::string>();

		if (alignment == "W") base_shard.alignment = W;
		else if (alignment == "S") base_shard.alignment = S;
		else if (alignment == "A") base_shard.alignment = A;
		else if (alignment == "D") base_shard.alignment = D;

		else if (alignment == "WD") base_shard.alignment = WD;
		else if (alignment == "DS") base_shard.alignment = DS;
		else if (alignment == "SA") base_shard.alignment = SA;
		else if (alignment == "WA") base_shard.alignment = WA;

		if (note.contains("velocity_multiplier"))
			base_shard.velocity = base_shard.velocity * note["velocity_multiplier"].get<float>();
		
		shards.push_back(base_shard);
	}

	for (auto &shard : shards)
		shard = generate_shard_data(shard);

	health_point = 100;
	score_point = 0;
	combo_point = 0;
	max_combo_reached = 0;

	core.active = true;

	total_accuracy = 0.0f;
	sum_of_total_accuracy = 0.0f;
	total_shards_destroyed = 0.0f;

	sound_pitch = 1.0f;
	losing_shards.clear();
	for (int i = 0; i < total_losing_shards; i++)
		losing_shards.push_back(core.position);

	last_beat_status = BEAT_NULL;
	beat_clock_time = 0.0f;

	losing_shard_velocity = 15.0f;

	win_screen_animation_completed = false;
	del_score = 0;
	del_accuracy = 0;
	del_combo = 0;

	current_win_label = win_label_animation_order.begin();
	for (auto label : win_label_animation_order) {
		if (label->big_label)
			label->scale = win_skill_rating_init_scale;
		else
			label->scale = win_label_init_scale;

		label->rotation_angle = win_label_init_angle;
		label->animated = false;
	}

	visual_time = 0.0f;
}

Shard Game::generate_shard_data(Shard shard, glm::vec3 core_elevation) {
	Shard new_shard = shard;

	float x_level = 10.0f - core_elevation.x, y_level = 8.0f - core_elevation.y, z_level = 14.0f - core_elevation.z;
	switch (new_shard.alignment) {
		case W:
			new_shard.position = glm::vec3(x_level, -y_level, 0.0f);
			break;
		
		case S:
			new_shard.position = glm::vec3(x_level, +y_level, 0.0f);
			break;

		case A:
			new_shard.position = glm::vec3(x_level, 0.0f, +z_level);
			break;

		case D:
			new_shard.position = glm::vec3(x_level, 0.0f, -z_level);
			break;


		case WD:
			new_shard.position = glm::vec3(x_level, -y_level, -z_level);
			break;

		case DS:
			new_shard.position = glm::vec3(x_level, +y_level, -z_level);
			break;

		case SA:
			new_shard.position = glm::vec3(x_level, +y_level, +z_level);
			break;

		case WA:
			new_shard.position = glm::vec3(x_level, -y_level, +z_level);
			break;
	} new_shard.direction = glm::normalize(core.position - new_shard.position);

	if (new_shard.direction.y == 0.0f and new_shard.direction.z != 0.0f) {
		new_shard.rotation_angle = 90.0f;
		new_shard.rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	}
	else if (new_shard.direction.y != 0.0f and new_shard.direction.z == 0.0f) {
		new_shard.rotation_angle = 45.0f;
		new_shard.rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	else if ((new_shard.direction.y > 0.0f and new_shard.direction.z < 0.0f) or (new_shard.direction.y < 0.0f and new_shard.direction.z > 0.0f)) {
		new_shard.rotation_angle = -45.0f;
		new_shard.rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	}
	else if ((new_shard.direction.y < 0.0f and new_shard.direction.z < 0.0f) or (new_shard.direction.y > 0.0f and new_shard.direction.z > 0.0f)) {
		new_shard.rotation_angle = 45.0f;
		new_shard.rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	new_shard.spawn_time = new_shard.impact_time - (glm::distance(core.position, new_shard.position) / new_shard.velocity);
	new_shard.active = false;
	new_shard.destroyed = false;

	return new_shard;
}

inline void Game::load_settings_file() {
	std::ifstream settings_file_in;
	std::stringstream settings_stream;
	std::string settings_data;

	settings_file_in.open(settings_filepath);
	if (not settings_file_in.is_open()) {
		std::cout << "Failed to open the settings file!" << std::endl;
		return;
	}

	settings_stream << settings_file_in.rdbuf();
	settings_file_in.close();
	settings_data = settings_stream.str();

	settings_json = json::parse(settings_data);

	enable_auto_restart_on_loss = settings_json["game"]["auto_restart_on_loss"];
	background_dim = settings_json["game"]["background_dim"];
	win_animation_style_int = settings_json["game"]["win_style_animation"].get<int>();

	if (win_animation_style_int == 0) win_animation_style = SCORES_BUILD_UP;
	else if (win_animation_style_int == 1) win_animation_style = SCORES_PUNCHED_IN;

	sound_volume = settings_json["audio"]["music_volume"];
	sfx_volume = settings_json["audio"]["sfx_volume"];

	fps_counter = settings_json["video"]["show_fps"];
	enable_vsync = settings_json["video"]["vsync"];

	msaa_samples = settings_json["video"]["msaa"];

	if (msaa_samples == 0)       msaa_samples_int = 0;
	else if (msaa_samples == 2)  msaa_samples_int = 1;
	else if (msaa_samples == 4)  msaa_samples_int = 2;
	else if (msaa_samples == 8)  msaa_samples_int = 3;
	else if (msaa_samples == 16) msaa_samples_int = 4;

	enable_bloom = settings_json["video"]["bloom"];
	enable_vignette = settings_json["video"]["vignette"];
	enable_motion_blur = settings_json["video"]["motion_blur"];
	enable_hdr_tonemapping = settings_json["video"]["hdr"];
	enable_deflection_sparks = settings_json["video"]["deflection_sparks"];
	enable_chromatic_aberration = settings_json["video"]["chromatic_aberration"];
}

inline void Game::save_settings_file() {
	settings_file.clear();
	settings_file.open(settings_filepath);
	if (not settings_file.is_open()) {
		std::cout << "Failed to open the settings file!" << std::endl;
		return;
	}

	settings_file << settings_json.dump(4);
	settings_file.close();
}

# endif