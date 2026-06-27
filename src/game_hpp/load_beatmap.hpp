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

			BeatmapTile *bmt = new BeatmapTile();
			bmt->position = glm::vec3(SCR_WIDTH - 250, 0.0f, 0.0f);
			bmt->scale = glm::vec3(1.0f);
			bmt->color = glm::vec3(1.0f);

			bmt->label = beatmap.path().filename().string();
			bmt->label_x_offset = -50.0f;

			bmt->function_ptr = nullptr;
			bmt->active = false;
		
			bmt->index = index++;

			beatmap_tiles.push_back(bmt);
		}
	} catch (fs::filesystem_error &e) {
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

	health_point = 100;
	score_point = 0;
	combo_point = 0;
	max_combo_reached = 0;

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
}

# endif