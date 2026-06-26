# ifndef GAME_HPP_LOAD_BEATMAP
# define GAME_HPP_LOAD_BEATMAP

# include "../game.hpp"

inline void Game::load_all_beatmaps() {
	fs::path beatmap_path = "beatmaps/";

	try {
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

inline void Game::load_beatmap_from_file(int index) {
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

	bg_song_path = beatmap_path.string() + beatmap_name + "/" + beatmap_json["audio"].get<std::string>();
	bg_image_path = beatmap_path.string() + beatmap_name + "/" + beatmap_json["background"].get<std::string>();

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
}

# endif