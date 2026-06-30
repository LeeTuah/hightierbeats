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
	float bgm_length = get_audio_length(&bgm);
	float current_time = calc_audio_time();

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
	model = glm::translate(model, core.position + glm::vec3(3.0f, 0.0f, 0.0f) + core_elevation);
	model = glm::scale(model, glm::vec3(1.0f, 9.0f * IMAGE_SCALE_FACTOR, 16.0f * IMAGE_SCALE_FACTOR));
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
	model = glm::rotate(model, glm::radians(-180.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
	flat_shader->set_mat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	main_shader->use();
	glBindVertexArray(core_VAO);
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
	model = glm::translate(model, core.position + glm::vec3(0.0f, core_offset_one, core_offset_two) - core_elevation);
	model = glm::rotate(model, (float)glm::radians(core.rotation_angle), glm::normalize(core.rotation_axis));
	main_shader->set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 60);

	if (core.active) {
		model = glm::scale(model, glm::vec3(1.1f));
		main_shader->set_mat4("model", model);
		glDrawArrays(GL_LINE_LOOP, 0, 60);
	}

	const ImGuiViewport *viewport = ImGui::GetMainViewport(); 
	int flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	float timeline_height = viewport->WorkSize.y * 0.25f;
	float side_panels_width = viewport->WorkSize.x * 0.20f;

	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkSize.y - timeline_height));
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, timeline_height));
	if (ImGui::Begin("Timeline", &mapmaker_timeline_opened, flags)) {
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::SliderFloat("##audio_timeline", &current_time, 0.0f, bgm_length)) {
			set_audio_time(&bgm, current_time);
		}

		if (ImGui::Button("Play")) {
			mapmaker_play_music = not mapmaker_play_music;

			if (mapmaker_play_music) {
				if (bg_song_path != "")
					ma_sound_start(&bgm);
				is_sound_playing = true;
			} else {
				ma_sound_stop(&bgm);
				is_sound_playing = false;
			}
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(ImVec2(side_panels_width, viewport->WorkSize.y - timeline_height));
	if (ImGui::Begin("Properties", &mapmaker_properties_opened, flags)){
	
		if (ImGui::Button("Test")) {

		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x - side_panels_width, viewport->WorkPos.y));
	ImGui::SetNextWindowSize(ImVec2(side_panels_width, viewport->WorkSize.y - timeline_height));
	if (ImGui::Begin("Shards", &mapmaker_timeline_opened, flags)) {

	}
	ImGui::End();
}

# endif