# ifndef GAME_HPP_RENDER_MAPMAKER
# define GAME_HPP_RENDER_MAPMAKER

# include "../game.hpp"

inline void Game::render_mapmaker() {
	projection = glm::perspective(camera->zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera->get_lookat_matrix();
	text_projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
	model = glm::mat4(1.0f);

	glm::vec3 core_elevation(0.0f, 2.5f, 0.0f);

	float bgm_length = 0.0f, current_time = 0.0f;
	if (is_song_loaded) {
		bgm_length = get_audio_length(&bgm);
		current_time = calc_audio_time();
	}

	if (is_background_loaded) {
		flat_shader->use();
		glBindVertexArray(rect_VAO);

		flat_shader->set_mat4("projection", projection);
		flat_shader->set_mat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		background_image->bind();

		flat_shader->set_bool("use_texture", true);
		flat_shader->set_int("tex", 0);
		flat_shader->set_float("background_dim", 1.0f - background_dim);

		const float IMAGE_SCALE_FACTOR = 2.85f;
		model = glm::mat4(1.0f);
		model = glm::translate(model, core.position + glm::vec3(3.0f, 0.0f, 0.0f) - core_elevation);
		model = glm::scale(model, glm::vec3(1.0f, 9.0f * IMAGE_SCALE_FACTOR, 16.0f * IMAGE_SCALE_FACTOR));
		model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
		model = glm::rotate(model, glm::radians(-180.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
		flat_shader->set_mat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	main_shader->use();
	main_shader->set_mat4("projection", projection);
	main_shader->set_mat4("view", view);
	
	main_shader->set_float3("viewer_pos", camera->position);

	main_shader->set_float3("shard.ambient", 1.0f, 1.0f, 1.0f);
	main_shader->set_float3("shard.diffuse", 1.0f, 1.0f, 1.0f);
	main_shader->set_float3("shard.specular", 1.0f, 1.0f, 1.0f);

	main_shader->set_float("shard.shininess", 32.0f);
	main_shader->set_float3("shard.color", glm::vec3(1.0f));

	main_shader->set_int("allocated_suns", sun_directions.size());
	for (size_t i = 0; i < sun_directions.size(); i++){
		std::string name = "suns[" + std::to_string(i) + "].";

		main_shader->set_float3(name + "ambient", 0.1f, 0.1f, 0.1f);
		main_shader->set_float3(name + "diffuse", 0.3f, 0.3f, 0.3f);
		main_shader->set_float3(name + "specular", 0.0f, 0.0f, 0.0f);

		main_shader->set_float3(name + "direction", sun_directions[i]);
	}

	glBindVertexArray(core_VAO);
	core.rotation_angle = 25.0f * glfwGetTime();

	model = glm::mat4(1.0f);
	model = glm::translate(model, core.position - core_elevation);
	model = glm::rotate(model, (float)glm::radians(core.rotation_angle), glm::normalize(core.rotation_axis));
	main_shader->set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 60);

	if (core.active) {
		model = glm::scale(model, glm::vec3(1.1f));
		main_shader->set_mat4("model", model);
		glDrawArrays(GL_LINE_LOOP, 0, 60);
	}

	glBindVertexArray(shard_VAO);

	for (auto &shard : shards) {
		if (not shard.active) continue;

		model = glm::mat4(1.0f);
		model = glm::translate(model, shard.position - core_elevation);
		if (shard.rotation_angle != 0.0f)
			model = glm::rotate(model, glm::radians(shard.rotation_angle), glm::normalize(shard.rotation_axis));

		model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
		main_shader->set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 24);
	}

	const ImGuiViewport *viewport = ImGui::GetMainViewport(); 
	int flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	float scale_up_factor = 3.0f;

	float timeline_height = viewport->WorkSize.y * 0.25f;
	float side_panels_width = viewport->WorkSize.x * 0.20f;

	auto set_bgm_time = [&](){
		current_time = std::clamp(current_time, 0.0f, bgm_length);
		set_audio_time(&bgm, current_time);
	};

	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkSize.y - timeline_height));
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, timeline_height));
	if (ImGui::Begin("Timeline", &mapmaker_timeline_opened, flags)) {

		ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, 18.0f);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::SliderFloat("##audio_timeline", &current_time, 0.0f, bgm_length, " ")) {
			set_bgm_time();
		}

		ImVec2 min_pos = ImGui::GetItemRectMin();
		ImVec2 max_pos = ImGui::GetItemRectMax();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 keyframe_color = IM_COL32(0, 255, 255, 255);
		float keyframe_thickness = 2.0f;

		ImGuiStyle &style = ImGui::GetStyle();
		float track_cutoff = style.FramePadding.x + (style.GrabMinSize / 2.0f);
		float start_pos = min_pos.x + track_cutoff, end_pos = max_pos.x - track_cutoff;
		
		for (auto &shard : shards) {
			float keyframe_x_coord = start_pos + ((shard.impact_time / bgm_length) * (end_pos - start_pos));

			ImVec2 keyframe_start(keyframe_x_coord, min_pos.y);
			ImVec2 keyframe_end(keyframe_x_coord, max_pos.y);

			draw_list->AddLine(keyframe_start, keyframe_end, keyframe_color, keyframe_thickness);
		}
		ImGui::PopStyleVar();

		for (int i = 0; i < 3; i++)
			ImGui::Spacing();

		float avaliable_content_region = ImGui::GetContentRegionAvail().x;
		float width_of_media_block = ImGui::CalcTextSize("<<<").x + ImGui::CalcTextSize("<").x + ImGui::CalcTextSize("<").x + 
									 ImGui::CalcTextSize(play_button_label.c_str()).x + ImGui::CalcTextSize(">").x + ImGui::CalcTextSize(">>").x + 
									 ImGui::CalcTextSize(">>>").x + (14 * ImGui::GetStyle().FramePadding.x)
									 + (6 * ImGui::GetStyle().ItemSpacing.x);
		float starting_x_pos = (avaliable_content_region - width_of_media_block) / 2.0f;

		std::string timeline_text = std::to_string(current_time) + " / " + std::to_string(bgm_length);
		ImGui::Text("%s", timeline_text.c_str());

		ImGui::SameLine();
		ImGui::SetCursorPosX(starting_x_pos);
		if (ImGui::Button("<<<")) {
			current_time -= 1.0f;
			set_bgm_time();
		}

		ImGui::SameLine();
		if (ImGui::Button("<<")) {
			current_time -= 0.2f;
			set_bgm_time();
		}

		ImGui::SameLine();
		if (ImGui::Button("<")) {
			current_time -= 0.0167f;
			set_bgm_time();
		}

		ImGui::SameLine();
		std::string new_play_label = play_button_label + "###play_btn_timeline";
		if (ImGui::Button(new_play_label.c_str())) {
			play_pause_pressed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button(">")) {
			current_time += 0.0167f;
			set_bgm_time();
		}

		ImGui::SameLine();
		if (ImGui::Button(">>")) {
			current_time += 0.2;
			set_bgm_time();
		}

		ImGui::SameLine();
		if (ImGui::Button(">>>")) {
			current_time += 1.0;
			set_bgm_time();
		}

		ImGui::Spacing();
		ImGui::Spacing();

		avaliable_content_region = ImGui::GetContentRegionAvail().x;
		width_of_media_block = ImGui::CalcTextSize("< Snap").x + ImGui::CalcTextSize("<-").x + ImGui::CalcTextSize("->").x
							 + ImGui::CalcTextSize("Snap >").x+ (8 * ImGui::GetStyle().FramePadding.x) 
							 + (3 * ImGui::GetStyle().ItemSpacing.x);
		starting_x_pos = (avaliable_content_region - width_of_media_block) / 2.0f;

		ImGui::SetNextItemWidth(ImGui::CalcTextSize("8.888888").x + 4.0f);
		if (ImGui::InputFloat("##sound_pitch_input", &sound_pitch)) {
			sound_pitch = std::clamp(sound_pitch, 0.1f, 4.0f);
		}

		ImGui::SameLine();
		if (ImGui::Button("▼##sound_pitch_down_btn")) {
			sound_pitch -= 0.1f;
			sound_pitch = std::clamp(sound_pitch, 0.1f, 4.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("▲##sound_pitch_up_btn")) {
			sound_pitch += 0.1f;
			sound_pitch = std::clamp(sound_pitch, 0.1f, 4.0f);
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(starting_x_pos);
		if (ImGui::Button("< Snap")) {
			snap_bwd_pressed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("<-")) {
			prev_shard_pressed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("->")) {
			next_shard_pressed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Snap >")) {
			snap_fwd_pressed = true;
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(ImVec2(side_panels_width, viewport->WorkSize.y - timeline_height));
	if (ImGui::Begin("Properties", &mapmaker_properties_opened, flags)) {
		ImGui::Text("Folder Name");
		if (ImGui::InputText("##folder_name_label", &loaded_folder_name)) {

		}
		ImGui::SameLine();

		if (ImGui::Button("Load##folder_load_btn")) {
			fs::path beatmap_path = "beatmaps/";
			is_folder_loaded = false;

			try {
				for (auto &beatmap : fs::directory_iterator(beatmap_path)) {
					if (not fs::is_directory(beatmap)) continue;
					if (beatmap.path().filename().string() != loaded_folder_name) continue;

					loaded_bg_path = beatmap_path.string() + loaded_folder_name + "/";
					loaded_song_path = beatmap_path.string() + loaded_folder_name + "/";
					is_folder_loaded = true;
					break;
				}
			} catch (fs::filesystem_error &e) {
				std::cout << e.what() << std::endl;
			}
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Background Path");
		if (ImGui::InputText("##background_label", &loaded_bg_path)) {

		}
		ImGui::SameLine();

		if (ImGui::Button("Load##bg_load_btn")) {
			if (background_image != nullptr) delete background_image;
			background_image = new Texture2D(loaded_bg_path);

			if (background_image->loaded_without_errors) {
				bg_image_path = loaded_bg_path;
				is_background_loaded = true;
			} else {
				bg_image_path = "";
				is_background_loaded = false;
			}
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Song Path");
		if (ImGui::InputText("##song_label", &loaded_song_path)) {
			
		}
		ImGui::SameLine();

		if (ImGui::Button("Load##song_load_btn")) {
			if (not bg_song_path.empty()) ma_sound_uninit(&bgm);
			if (ma_sound_init_from_file(&audio_engine, loaded_song_path.c_str(), 0, 0, NULL, &bgm) != MA_SUCCESS) {
				std::cout << "[!] Failed to load the audio!" << std::endl;
				bg_song_path = "";
				is_song_loaded = false;
			} else {
				bg_song_path = loaded_song_path;
				is_song_loaded = true;
			}
		}

		float old_base_velocity = base_velocity;

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Base Velocity");
		if (ImGui::InputFloat("##base_velocity_input", &base_velocity)) {
			base_velocity = std::clamp(base_velocity, 1.0f, 50.0f);
		}
		if (ImGui::SliderFloat("##base_velocity_slider", &base_velocity, 1.0f, 50.0f)) {
			base_velocity = std::clamp(base_velocity, 1.0f, 50.0f);
		}

		ImGui::SameLine();
		if (ImGui::Button("▼##base_velocity_down_btn")) {
			base_velocity -= 1.0f;
			base_velocity = std::clamp(base_velocity, 1.0f, 50.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("▲##base_velocity_up_btn")) {
			base_velocity += 1.0f;
			base_velocity = std::clamp(base_velocity, 1.0f, 50.0f);
		}

		if (old_base_velocity != base_velocity) {
			for (auto &shard : shards) {
				float velocity_multiplier = shard.velocity / old_base_velocity;
				shard.velocity = velocity_multiplier * base_velocity;

				shard = generate_shard_data(shard);
			}
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Beats Per Minute (BPM)");
		if (ImGui::InputFloat("##bpm_input", &beats_per_minute)) {
			beats_per_minute = std::clamp(beats_per_minute, 1.0f, 1000.0f);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Song Offset");
		if (ImGui::InputFloat("##song_offset_input", &song_offset)) {
			song_offset = std::clamp(song_offset, 0.0f, bgm_length);
		}

		const char *divisors[] = {"1/1", "1/2", "1/4", "1/8"};
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Beat Divisor");
		if (ImGui::Combo("##beat_divisor_combo", &song_divisor_int, divisors, IM_ARRAYSIZE(divisors))) {
			if (song_divisor_int == 0) song_divisor = 1.0f;
			else if (song_divisor_int == 1) song_divisor = 1.0f / 2.0f;
			else if (song_divisor_int == 2) song_divisor = 1.0f / 4.0f;
			else if (song_divisor_int == 3) song_divisor = 1.0f / 8.0f;
		}

		for (int i = 0; i < 3; i++)
			ImGui::Spacing();
			
		ImGui::Text("Song Name");
		if (ImGui::InputText("##song_name_label", &song_name)) {

		}

		ImGui::Spacing();
		ImGui::Text("Artist Name");
		if (ImGui::InputText("##artist_name_label", &artist_name)) {

		}

		ImGui::Spacing();
		ImGui::Text("Creator Name");
		if (ImGui::InputText("##creator_name_label", &creator_name)) {

		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Background Dim");
		if (ImGui::SliderFloat("##background_dim_slider", &background_dim, 0.0f, 1.0f)) {
			background_dim = std::clamp(background_dim, 0.0f, 1.0f);
		}

		ImGui::SameLine();
		if (ImGui::Button("▼##bg_dim_down_btn")) {
			background_dim -= 0.1f;
			background_dim = std::clamp(background_dim, 0.0f, 1.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("▲##bg_dim_up_btn")) {
			background_dim += 0.1f;
			background_dim = std::clamp(background_dim, 0.0f, 1.0f);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Volume");
		if (ImGui::SliderFloat("##volume_slider", &sound_volume, 0.0f, 1.0f)) {
			ma_sound_set_volume(&bgm, sound_volume);
		}

		for (int i = 0; i < 20; i++)
			ImGui::Spacing();

		ImVec2 save_btn_size = ImVec2(
			ImGui::GetContentRegionAvail().x, 
			(mapmaker_font_size + (2 * ImGui::GetStyle().FramePadding.y)) * scale_up_factor
		);
		if (ImGui::Button("Save to JSON##save_json_btn", save_btn_size)) {
			ImGui::OpenPopup("Are you sure?##save_popup");
		}

		ImGui::SetNextWindowPos(
			ImVec2(viewport->WorkPos.x + (0.5 * viewport->WorkSize.x), viewport->WorkPos.y + (0.5 * viewport->WorkSize.y)),
			ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)
		);
		if (ImGui::BeginPopupModal("Are you sure?##save_popup")) {
			ImGui::Text("Saving will overwrite previous beatmap data.");

			for (int i = 0; i < 5; i++)
				ImGui::Spacing();

			if (ImGui::Button("Ok")) {
				json beatmap_data;
				beatmap_data["name"] = song_name;
				beatmap_data["artist"] = artist_name;
				beatmap_data["creator"] = creator_name;

				auto bg_path = fs::path(loaded_bg_path);
				auto song_path = fs::path(loaded_song_path);

				beatmap_data["audio"] = song_path.filename();
				beatmap_data["background"] = bg_path.filename();

				beatmap_data["base_velocity"] = base_velocity;
				beatmap_data["notes"] = json::array();

				for (auto shard : shards) {
					json current_shard;

					current_shard["time"] = shard.impact_time;
					if (std::abs(shard.velocity - base_velocity) > 0.01f)
						current_shard["velocity_multiplier"] = shard.velocity / base_velocity;

					if (shard.alignment == W)
						current_shard["alignment"] = "W";
					else if (shard.alignment == A)
						current_shard["alignment"] = "A";
					else if (shard.alignment == S)
						current_shard["alignment"] = "S";
					else if (shard.alignment == D)
						current_shard["alignment"] = "D";

					else if (shard.alignment == WA)
						current_shard["alignment"] = "WA";
					else if (shard.alignment == SA)
						current_shard["alignment"] = "SA";
					else if (shard.alignment == DS)
						current_shard["alignment"] = "DS";
					else if (shard.alignment == WD)
						current_shard["alignment"] = "WD";

					beatmap_data["notes"].push_back(current_shard);
				}

				std::ofstream file("beatmaps/" + loaded_folder_name + "/" + loaded_folder_name + ".json");
				file << beatmap_data.dump(4);
				file.close();

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::Button("Load from JSON##load_from_json_btn", save_btn_size)) {
			ImGui::OpenPopup("Are you sure?##load_popup");
		}

		ImGui::SetNextWindowPos(
			ImVec2(viewport->WorkPos.x + (0.5 * viewport->WorkSize.x), viewport->WorkPos.y + (0.5 * viewport->WorkSize.y)),
			ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)
		);
		if (ImGui::BeginPopupModal("Are you sure?##load_popup")) {
			ImGui::Text("Loading will overwrite unsaved progress.");

			for (int i = 0; i < 5; i++)
				ImGui::Spacing();
			
			if (ImGui::Button("Ok")) {
				fs::path beatmap_path = "beatmaps/";
				std::string json_data = "";

				try {
					for (auto &beatmap : fs::directory_iterator(beatmap_path)) {
						if (not fs::is_directory(beatmap)) continue;
						if (loaded_folder_name != beatmap.path().filename().string()) continue;

						std::ifstream json_file_stream;
						std::stringstream json_file_stringstream;

						json_file_stream.open(("beatmaps/" + loaded_folder_name + "/" + loaded_folder_name + ".json").c_str());
						if (not json_file_stream.is_open()) {
							std::cout << "[!] Failed to open the file!" << std::endl;
							break;
						}

						json_file_stringstream << json_file_stream.rdbuf();
						json_file_stream.close();
						json_data = json_file_stringstream.str();
						break;
					}
				} catch (fs::filesystem_error &e) {
					std::cout << e.what() << std::endl;
				}
				if (json_data != "") {
					json file_json = json::parse(json_data);

					artist_name = file_json["artist"];
					creator_name = file_json["creator"];
					song_name = file_json["name"];
					base_velocity = file_json["base_velocity"];

					loaded_song_path = "beatmaps/" + loaded_folder_name + "/" + file_json["audio"].get<std::string>();
					loaded_bg_path = "beatmaps/" + loaded_folder_name + "/" + file_json["background"].get<std::string>();

					Shard base_shard;
					base_shard.active = false;
					base_shard.destroyed = false;
					base_shard.velocity = base_velocity;
					float base_velocity = base_shard.velocity;

					for (auto shard_data : file_json["notes"]) {
						base_shard.velocity = base_velocity;

						base_shard.impact_time = shard_data["time"];
						std::string alignment = shard_data["alignment"].get<std::string>();

						if (alignment == "W") base_shard.alignment = W;
						else if (alignment == "S") base_shard.alignment = S;
						else if (alignment == "A") base_shard.alignment = A;
						else if (alignment == "D") base_shard.alignment = D;

						else if (alignment == "WD") base_shard.alignment = WD;
						else if (alignment == "DS") base_shard.alignment = DS;
						else if (alignment == "SA") base_shard.alignment = SA;
						else if (alignment == "WA") base_shard.alignment = WA;

						if (shard_data.contains("velocity_multiplier"))
							base_shard.velocity = base_shard.velocity * shard_data["velocity_multiplier"].get<float>();
						
						shards.push_back(base_shard);
					}

					for (auto &shard : shards)
						shard = generate_shard_data(shard);

					if (background_image != nullptr) delete background_image;
					background_image = new Texture2D(loaded_bg_path);

					if (background_image->loaded_without_errors) {
						bg_image_path = loaded_bg_path;
						is_background_loaded = true;
					} else {
						bg_image_path = "";
						is_background_loaded = false;
					}

					if (not bg_song_path.empty()) ma_sound_uninit(&bgm);
					if (ma_sound_init_from_file(&audio_engine, loaded_song_path.c_str(), 0, 0, NULL, &bgm) != MA_SUCCESS) {
						std::cout << "[!] Failed to load the audio!" << std::endl;
						bg_song_path = "";
						is_song_loaded = false;
					} else {
						bg_song_path = loaded_song_path;
						is_song_loaded = true;
					}
				}

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	ImGui::End();

	if (is_song_loaded) {
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x - side_panels_width, viewport->WorkPos.y));
		ImGui::SetNextWindowSize(ImVec2(side_panels_width, viewport->WorkSize.y - timeline_height));
		ImGui::Begin("Shards", &mapmaker_timeline_opened, flags);

		ImGui::Text("Shard Alignment");
		ImGui::Spacing();

		if (ImGui::RadioButton("WA", &current_alignment_selected_int, 0)) {
			current_alignment_selected = WA;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("W", &current_alignment_selected_int, 1)) {
			current_alignment_selected = W;
		}
		ImGui::SameLine();
		float third_element_x = ImGui::GetCursorPosX();
		if (ImGui::RadioButton("WD", &current_alignment_selected_int, 2)) {
			current_alignment_selected = WD;
		}

		if (ImGui::RadioButton("A", &current_alignment_selected_int, 3)) {
			current_alignment_selected = A;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(third_element_x);
		if (ImGui::RadioButton("D", &current_alignment_selected_int, 4)) {
			current_alignment_selected = D;
		}

		if (ImGui::RadioButton("SA", &current_alignment_selected_int, 5)) {
			current_alignment_selected = SA;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("S", &current_alignment_selected_int, 6)) {
			current_alignment_selected = S;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("SD", &current_alignment_selected_int, 7)) {
			current_alignment_selected = DS;
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Velocity Multiplier");
		if (ImGui::InputFloat("##vel_mul_input", &velocity_multiplier)) {
			velocity_multiplier = std::clamp(velocity_multiplier, 1.0f, 5.0f);
		}

		if (ImGui::SliderFloat("##velocity_mul_slider", &velocity_multiplier, 1.0f, 5.0f)) {
			velocity_multiplier = std::clamp(velocity_multiplier, 1.0f, 5.0f);
		}

		ImGui::SameLine();
		if (ImGui::Button("▼##vel_mul_down_btn")) {
			velocity_multiplier -= 0.25f;
			velocity_multiplier = std::clamp(velocity_multiplier, 1.0f, 5.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("▲##vel_mul_up_btn")) {
			velocity_multiplier += 0.25f;
			velocity_multiplier = std::clamp(velocity_multiplier, 1.0f, 5.0f);
		}
		for (int i = 0; i < 20; i++)
			ImGui::Spacing();
		
		scale_up_factor = 2.4f;
		ImVec2 shard_btn_size = ImVec2(
			ImGui::GetContentRegionAvail().x, 
			(mapmaker_font_size + (2 * ImGui::GetStyle().FramePadding.y)) * scale_up_factor
		);
		if (ImGui::Button("Place Shard##place_shard_btn", shard_btn_size)) {
			place_shard_pressed = true;
		}

		if (ImGui::Button("Delete Shard##delete_shard_btn", shard_btn_size)) {
			delete_shard_pressed = true;
		}
		ImGui::End();
	}

	current_shard.alignment = current_alignment_selected;
	current_shard.velocity = base_velocity * velocity_multiplier;
	current_shard.impact_time = current_time + (shard_radius / current_shard.velocity);

	if (play_pause_pressed) {
		play_pause_pressed = false;

		mapmaker_play_music = not mapmaker_play_music;

		if (is_song_loaded) {
			if (mapmaker_play_music) {
				if (bg_song_path != "")
					ma_sound_start(&bgm);
				is_sound_playing = true;
				play_button_label = "||";
			} else {
				ma_sound_stop(&bgm);
				is_sound_playing = false;
				play_button_label = "►";
			}
		}
		ma_sound_set_volume(&bgm, sound_volume);
	}
	if (snap_fwd_pressed) {
		snap_fwd_pressed = false;

		float snap_interval = (60.0f / beats_per_minute) * song_divisor;
		float current_index = (current_time - song_offset) / snap_interval;

		float next_index = std::floor(current_index + 0.005f) + 1;
		current_time = song_offset + (next_index * snap_interval);
		set_bgm_time();
	}
	if (snap_bwd_pressed) {
		snap_bwd_pressed = false;
		float snap_interval = (60.0f / beats_per_minute) * song_divisor;
		float current_index = (current_time - song_offset) / snap_interval;

		float prev_index = std::ceil(current_index - 0.0005f) - 1;
		current_time = song_offset + (prev_index * snap_interval);
		set_bgm_time();
	}
	if (next_shard_pressed) {
		next_shard_pressed = false;
		for (auto shard_iter = shards.begin(); shard_iter != shards.end(); shard_iter++) {
			if (shard_iter->impact_time <= current_time + 0.005f) continue;

			current_time = shard_iter->impact_time;
			set_bgm_time();
			break;
		}
	}
	if (prev_shard_pressed) {
		prev_shard_pressed = false;
		for (auto shard_iter = shards.rbegin(); shard_iter != shards.rend(); shard_iter++) {
			if (shard_iter->impact_time >= current_time - 0.005f) continue;

			current_time = shard_iter->impact_time;
			set_bgm_time();
			break;
		}
	}
	if (place_shard_pressed) {
		place_shard_pressed = false;
		current_shard = generate_shard_data(current_shard);
		current_shard.active = true;

		shards.push_back(current_shard);
		std::sort(shards.begin(), shards.end(), [](const Shard &a, const Shard &b) {
			return a.impact_time < b.impact_time;
		});
	}
	if (delete_shard_pressed) {
		delete_shard_pressed = false;
		if (not shards.empty()) {
			std::vector<Shard>::iterator shard_to_delete = shards.end();
			for (auto shard = shards.begin(); shard != shards.end(); shard++) {
				if (abs(shard->impact_time - current_time) > 0.050f) continue;

				shard_to_delete = shard;
				break;
			}
			if (shard_to_delete != shards.end())
			shards.erase(shard_to_delete);
		}
	}
	if (esc_mapmaker_key_pressed) {
		ImGui::OpenPopup("Are you sure?##exit_popup");

		ImGui::SetNextWindowPos(
			ImVec2(viewport->WorkPos.x + (0.5 * viewport->WorkSize.x), viewport->WorkPos.y + (0.5 * viewport->WorkSize.y)),
			ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)
		);
		if (ImGui::BeginPopupModal("Are you sure?##exit_popup")) {
			ImGui::Text("Exiting now will wipe any unsaved progress.");

			for (int i = 0; i < 5; i++)
				ImGui::Spacing();

			if (ImGui::Button("Ok")) {
				esc_mapmaker_key_confirmed = true;
				esc_mapmaker_key_pressed = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				esc_mapmaker_key_confirmed = false;
				esc_mapmaker_key_pressed = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}

# endif
