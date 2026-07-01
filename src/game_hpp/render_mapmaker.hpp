# ifndef GAME_HPP_RENDER_MAPMAKER
# define GAME_HPP_RENDER_MAPMAKER

# include "../game.hpp"

inline void Game::render_mapmaker() {
	if (game_state != GAME_MAPMAKER) return;

	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	float scale_up_factor = 3.0f; // for save json and place shard btns

	float timeline_height = viewport->WorkSize.y * 0.25f;
	float side_panels_width = viewport->WorkSize.x * 0.20f;

	auto set_bgm_time = [&](){
		current_time = std::clamp(current_time, 0.0f, bgm_length);
		set_audio_time(&bgm, current_time);
	};

	// TODO: add an error popup later

	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkSize.y - timeline_height));
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, timeline_height));
	if (ImGui::Begin("Timeline", &mapmaker_timeline_opened, flags)) {

		ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, 18.0f);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::SliderFloat("##audio_timeline", &current_time, 0.0f, bgm_length, " ")) {
			set_bgm_time();
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
		float btn_gap = 20.0f;

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
	}
	ImGui::End();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(ImVec2(side_panels_width, viewport->WorkSize.y - timeline_height));
	if (ImGui::Begin("Properties", &mapmaker_properties_opened, flags)){
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

		for (int i = 0; i < 20; i++)
			ImGui::Spacing();

		ImVec2 save_btn_size = ImVec2(
			ImGui::CalcTextSize("Save to JSON").x * scale_up_factor, 
			(mapmaker_font_size + (2 * ImGui::GetStyle().FramePadding.y)) * scale_up_factor
		);
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - save_btn_size.x - (2 * ImGui::GetStyle().FramePadding.x));
		if (ImGui::Button("Save to JSON##save_json_btn", save_btn_size)) {
			// TODO: code to save json here
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x - side_panels_width, viewport->WorkPos.y));
	ImGui::SetNextWindowSize(ImVec2(side_panels_width, viewport->WorkSize.y - timeline_height));
	if (ImGui::Begin("Shards", &mapmaker_timeline_opened, flags)) {
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
		if (ImGui::RadioButton("DS", &current_alignment_selected_int, 7)) {
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
		
		ImVec2 shard_btn_size = ImVec2(
			ImGui::CalcTextSize("Place Shard").x * scale_up_factor, 
			(mapmaker_font_size + (2 * ImGui::GetStyle().FramePadding.y)) * scale_up_factor
		);
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - shard_btn_size.x - (2 * ImGui::GetStyle().FramePadding.x));
		if (ImGui::Button("Place Shard##place_shard_btn", shard_btn_size)) {
			if (is_song_loaded) {
				current_shard = generate_shard_data(current_shard);
				current_shard.active = true;

				shards.push_back(current_shard);
			}
		}
	}
	ImGui::End();

	current_shard.alignment = current_alignment_selected;
	current_shard.velocity = base_velocity * velocity_multiplier;
	current_shard.impact_time = current_time;
}

# endif