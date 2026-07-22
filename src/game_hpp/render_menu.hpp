# ifndef GAME_HPP_RENDER_MENU
# define GAME_HPP_RENDER_MENU

# include "../game.hpp"

inline void Game::render_menu() {
	glDisable(GL_DEPTH_TEST);

	view = glm::mat4(1.0f);
	text_projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
	model = glm::mat4(1.0f);

	flat_shader->use();
	glBindVertexArray(rect_VAO);

	flat_shader->set_mat4("projection", glm::mat4(1.0f));
	flat_shader->set_mat4("view", glm::mat4(1.0f));
	glActiveTexture(GL_TEXTURE0);

	flat_shader->set_bool("use_texture", true);
	flat_shader->set_int("tex", 0);
	flat_shader->set_float("background_dim", 0.5f);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
	flat_shader->set_mat4("model", model);

	if (game_state == GAME_MAIN_MENU or game_state == GAME_SETTINGS) {
		int current_backgroud_frame = (int)(glfwGetTime() * menu_video_fps) % total_menu_video_frames;
		menu_video_frames[current_backgroud_frame]->bind();
	}
	else if (game_state == GAME_SELECTING_BEATMAP) {
		if (
			not all_beatmaps_backgrounds.empty() and
			current_user_beatmap_index < all_beatmaps_backgrounds.size() and
			all_beatmaps_backgrounds[current_user_beatmap_index] != nullptr
		)
			all_beatmaps_backgrounds[current_user_beatmap_index]->bind();
		else
			black_img->bind();
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(menu_tile_VAO);
	flat_shader->set_bool("use_texture", false);

	flat_shader->set_mat4("projection", text_projection);
	flat_shader->set_mat4("view", view);

	if (game_state == GAME_SELECTING_BEATMAP) {
		for (auto beatmap_tile : beatmap_tiles) {
			if (beatmap_tile->position.y > SCR_HEIGHT + 100.0f or beatmap_tile->position.y < -100.0f)
				continue;

			flat_shader->set_float3("color", beatmap_tile->color);

			model = glm::mat4(1.0f);
			model = glm::translate(model, beatmap_tile->position);
			model = glm::scale(model, beatmap_tile->scale);

			if (current_user_beatmap_index == beatmap_tile->index)
				model = glm::scale(model, glm::vec3(beatmap_scale, 1.0f, 1.0f));

			if (last_user_beatmap_index == beatmap_tile->index)
				model = glm::scale(model, glm::vec3(1.0f + max_beatmap_scale - beatmap_scale, 1.0f, 1.0f));

			flat_shader->set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 18);
		}

		for (auto beatmap_tile : beatmap_tiles) {
			float beatmap_label_scale = 0.4f;
			float beatmap_selected_label_x_offet = 0.0f;

			std::string beatmap_label = beatmap_tile->label;
			if (beatmap_label.size() > 15 and current_user_beatmap_index != beatmap_tile->index)
				beatmap_label = beatmap_label.substr(0, 12) + "...";

			if (current_user_beatmap_index == beatmap_tile->index)
				beatmap_selected_label_x_offet = -180.0f;

			vcr_osd_mono->render_text(
				beatmap_label,
				beatmap_tile->position.x - 35.0f + beatmap_tile->label_x_offset + beatmap_selected_label_x_offet,
				beatmap_tile->position.y - 10.0f,
				beatmap_label_scale, glm::vec3(0.0f)
			);
		}
	}

	else if (game_state == GAME_MAIN_MENU) {
		for (auto menu_tile : main_menu_tiles) {
			flat_shader->set_float3("color", menu_tile->color);

			model = glm::mat4(1.0f);
			model = glm::translate(model, menu_tile->position);
			model = glm::scale(model, menu_tile->scale);

			if (menu_tile == *current_menu_tile and menu_tile->active) 
				model = glm::scale(model, glm::vec3(menu_scale));

			else if (last_menu_tile != main_menu_tiles.end() and menu_tile == *last_menu_tile and not menu_tile->active)
				model = glm::scale(model, glm::vec3(1.0f + max_menu_scale - menu_scale));

			flat_shader->set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 18);
		}

		for (auto menu_tile : main_menu_tiles) {
			float menu_label_scale = 0.5f;

			if (menu_tile == *current_menu_tile) menu_label_scale = menu_scale - 0.5f;
			else if (last_menu_tile != main_menu_tiles.end() and menu_tile == *last_menu_tile) menu_label_scale = 1.0f + max_menu_scale - menu_scale - 0.5f;

			vcr_osd_mono->render_text(
				menu_tile->label,
				menu_tile->position.x - 35.0f + menu_tile->label_x_offset, 
				menu_tile->position.y - 10.0f,
				menu_label_scale, glm::vec3(0.0f)
			);
		}
	}

	else if (game_state == GAME_SETTINGS) {
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		int flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		int child_flags = ImGuiChildFlags_Borders;

		ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x * 0.20f, viewport->WorkSize.y * 0.20f));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x * 0.60f, viewport->WorkSize.y * 0.60f));
		if (ImGui::Begin("Settings", &mapmaker_timeline_opened, flags)) {
			if (ImGui::BeginChild("##sidebar", ImVec2(viewport->WorkSize.x * 0.09f, 0.0f), child_flags, flags)) {
				if (ImGui::Selectable("Game", current_settings_menu_item == 0, 0, ImVec2(0, 30))) {
					current_settings_menu_item = 0;
					play_sfx(&click_cursor_sound);
				}
				ImGui::Spacing();
				if (ImGui::Selectable("Audio", current_settings_menu_item == 1, 0, ImVec2(0, 30))) {
					current_settings_menu_item = 1;
					play_sfx(&click_cursor_sound);
				}
				ImGui::Spacing();
				if (ImGui::Selectable("Video", current_settings_menu_item == 2, 0, ImVec2(0, 30))) {
					current_settings_menu_item = 2;
					play_sfx(&click_cursor_sound);
				}
			} ImGui::EndChild();

			ImGui::SameLine();
			int total_spacing_between_settings_items = 3;

			if (ImGui::BeginChild("##content", ImVec2(viewport->WorkSize.x * 0.497f, 0.0f), child_flags, flags)) {
				if (current_settings_menu_item == 0) {
					ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2.0f - ImGui::CalcTextSize("General").x);
					ImGui::Text("General");

					ImGui::Spacing();
					ImGui::Separator();
					for (int i = 0; i < total_spacing_between_settings_items + 2; i++) ImGui::Spacing();

					ImGui::Text("Auto Restart on Loss ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##enable_auto_restart_on_loss", &enable_auto_restart_on_loss)) {
						settings_json["game"]["auto_restart_on_loss"] = enable_auto_restart_on_loss;
						save_settings_file();
					}
					ImGui::TextDisabled("Automatically restart a level if you were to lose.");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("Background Dim");
					if (ImGui::SliderFloat("##background_dim_slider", &background_dim, 0.0f, 1.0f)) {
						background_dim = std::clamp(background_dim, 0.0f, 1.0f);
					}

					if (ImGui::IsItemDeactivatedAfterEdit()) {
						settings_json["game"]["background_dim"] = background_dim;
						save_settings_file();
					}

					ImGui::SameLine();
					if (ImGui::Button("▼##bg_dim_down_btn")) {
						background_dim -= 0.1f;
						background_dim = std::clamp(background_dim, 0.0f, 1.0f);

						settings_json["game"]["background_dim"] = background_dim;
						save_settings_file();
					}
					ImGui::SameLine();
					if (ImGui::Button("▲##bg_dim_up_btn")) {
						background_dim += 0.1f;
						background_dim = std::clamp(background_dim, 0.0f, 1.0f);

						settings_json["game"]["background_dim"] = background_dim;
						save_settings_file();
					}
					ImGui::TextDisabled("Change the dim of the background in beatmaps.");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("Win Style Animation");
					ImGui::SameLine();

					const char *win_styles[] = {"Build Up", "Punched In"};
					if (ImGui::Combo("##win_style_animation_combo", &win_animation_style_int, win_styles, IM_ARRAYSIZE(win_styles))) {
						if (win_animation_style_int == 0) win_animation_style = SCORES_BUILD_UP;
						if (win_animation_style_int == 1) win_animation_style = SCORES_PUNCHED_IN;

						settings_json["game"]["win_style_animation"] = win_animation_style_int;
						save_settings_file();
					}
					ImGui::TextDisabled("Win Animation Style to be rendered when you beat a map.");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();
				}

				if (current_settings_menu_item == 1) {
					ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2.0f - ImGui::CalcTextSize("Audio").x);
					ImGui::Text("Audio");

					ImGui::Spacing();
					ImGui::Separator();
					for (int i = 0; i < total_spacing_between_settings_items + 2; i++) ImGui::Spacing();

					ImGui::Text("Music Volume");
					if (ImGui::SliderFloat("##sond_volume_slider", &sound_volume, 0.0f, 1.0f)) {
						sound_volume = std::clamp(sound_volume, 0.0f, 1.0f);
					}

					if (ImGui::IsItemDeactivatedAfterEdit()) {
						settings_json["audio"]["music_volume"] = sound_volume;
						save_settings_file();
					}

					ImGui::SameLine();
					if (ImGui::Button("▼##sond_volume_slider_down_btn")) {
						sound_volume -= 0.1f;
						sound_volume = std::clamp(sound_volume, 0.0f, 1.0f);

						settings_json["audio"]["music_volume"] = sound_volume;
						save_settings_file();
					}
					ImGui::SameLine();
					if (ImGui::Button("▲##sond_volume_slider_up_btn")) {
						sound_volume += 0.1f;
						sound_volume = std::clamp(sound_volume, 0.0f, 1.0f);

						settings_json["audio"]["music_volume"] = sound_volume;
						save_settings_file();
					}
					ImGui::TextDisabled("Volume of the music.");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("SFX Volume");
					if (ImGui::SliderFloat("##sfx_volume_slider", &sfx_volume, 0.0f, 1.0f)) {
						sfx_volume = std::clamp(sfx_volume, 0.0f, 1.0f);
					}

					if (ImGui::IsItemDeactivatedAfterEdit()) {
						settings_json["audio"]["sfx_volume"] = sfx_volume;
						save_settings_file();
					}

					ImGui::SameLine();
					if (ImGui::Button("▼##sfx_volume_slider_down_btn")) {
						sfx_volume -= 0.1f;
						sfx_volume = std::clamp(sfx_volume, 0.0f, 1.0f);

						settings_json["audio"]["sfx_volume"] = sfx_volume;
						save_settings_file();
					}
					ImGui::SameLine();
					if (ImGui::Button("▲##sfx_volume_slider_up_btn")) {
						sfx_volume += 0.1f;
						sfx_volume = std::clamp(sfx_volume, 0.0f, 1.0f);

						settings_json["audio"]["sfx_volume"] = sfx_volume;
						save_settings_file();
					}
					ImGui::TextDisabled("Volume of the Sound Effects.");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();
				}

				if (current_settings_menu_item == 2) {
					ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2.0f - ImGui::CalcTextSize("Video").x);
					ImGui::Text("Video");
					
					ImGui::Spacing();
					ImGui::Separator();
					for (int i = 0; i < total_spacing_between_settings_items + 2; i++) ImGui::Spacing();

					ImGui::Text("Show FPS ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##fps_counter", &fps_counter)) {
						settings_json["video"]["show_fps"] = fps_counter;
						save_settings_file();
					}
					ImGui::TextDisabled("Show/Hide FPS");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing(); 

					ImGui::Text("VSync ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##vsync_checkbox", &enable_vsync)) {
						settings_json["video"]["vsync"] = enable_vsync;
						save_settings_file();
					}
					ImGui::TextDisabled("Enable/Disable Vertical Sync");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("Vignette ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##vignette", &enable_vignette)) {
						settings_json["video"]["vignette"] = enable_vignette;
						save_settings_file();
					}
					ImGui::TextDisabled("Darken the edges of the screen");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("Anti-aliasing ");
					ImGui::SameLine();

					const char *msaa_samples_list[] = {"Disabled", "2x MSAA", "4x MSAA", "8x MSAA", "16x MSAA", "32x MSAA"};
					if (ImGui::Combo("##msaa_samples_combo", &msaa_samples_int, msaa_samples_list, IM_ARRAYSIZE(msaa_samples_list))) {
						if (msaa_samples_int == 0) msaa_samples =  0;
						if (msaa_samples_int == 1) msaa_samples =  2;
						if (msaa_samples_int == 2) msaa_samples =  4;
						if (msaa_samples_int == 3) msaa_samples =  8;
						if (msaa_samples_int == 4) msaa_samples = 16;
						if (msaa_samples_int == 5) msaa_samples = 32;

						settings_json["video"]["msaa"] = msaa_samples;
						save_settings_file();

						resize_fbo(SCR_WIDTH, SCR_HEIGHT);
					}
					ImGui::TextDisabled("Multisampling amount to be used (the higher the prettier)");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("Chromatic Aberration ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##chromatic_aberration", &enable_chromatic_aberration)) {
						settings_json["video"]["chromatic_aberration"] = enable_chromatic_aberration;
						save_settings_file();
					}
					ImGui::TextDisabled("Adds color shifting to the screen");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("HDR Tonemapping (WIP) ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##hdr_tonemapping", &enable_hdr_tonemapping)) {
						settings_json["video"]["hdr"] = enable_hdr_tonemapping;
						save_settings_file();
					}
					ImGui::TextDisabled("Adapting the game brightness to the display");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::BeginDisabled();

					ImGui::Text("Bloom (WIP) ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##bloom", &enable_bloom)) {
						settings_json["video"]["bloom"] = enable_bloom;
						save_settings_file();
					}
					ImGui::TextDisabled("Glowing Effect for the shards and core");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("Motion Blur (WIP) ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##motion_blur", &enable_motion_blur)) {
						settings_json["video"]["motion_blur"] = enable_motion_blur;
						save_settings_file();
					}
					ImGui::TextDisabled("Blurs the shards");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::Text("Deflection Sparks (WIP) ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##deflection_sparks", &enable_deflection_sparks)) {
						settings_json["video"]["deflection_sparks"] = enable_deflection_sparks;
						save_settings_file();
					}
					ImGui::TextDisabled("Sparks appear when a shard is successfully blocked");
					for (int i = 0; i < total_spacing_between_settings_items; i++) ImGui::Spacing();

					ImGui::EndDisabled();
				}
			} ImGui::EndChild();
		} ImGui::End();
	}
}

# endif