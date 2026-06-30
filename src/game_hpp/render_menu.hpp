# ifndef GAME_HPP_RENDER_MENU
# define GAME_HPP_RENDER_MENU

# include "../game.hpp"

inline void Game::render_menu() {
	if (
		game_state != GAME_MAIN_MENU and
		game_state != GAME_SELECTING_BEATMAP
	) return;

	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);

	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	view = glm::mat4(1.0f);
	text_projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
	model = glm::mat4(1.0f);

	flat_shader->use();
	glBindVertexArray(rect_VAO);

	flat_shader->set_mat4("projection", glm::mat4(1.0f));
	flat_shader->set_mat4("view", glm::mat4(1.0f));

	int current_backgroud_frame = (int)(glfwGetTime() * menu_video_fps) % total_menu_video_frames;
	
	glActiveTexture(GL_TEXTURE0);
	menu_video_frames[current_backgroud_frame]->bind();

	flat_shader->set_bool("use_texture", true);
	flat_shader->set_int("tex", 0);
	flat_shader->set_float("background_dim", 0.5f);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));

	flat_shader->set_mat4("model", model);
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

			else if (menu_tile == *last_menu_tile and not menu_tile->active)
				model = glm::scale(model, glm::vec3(1.0f + max_menu_scale - menu_scale));

			flat_shader->set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 18);
		}

		for (auto menu_tile : main_menu_tiles) {
			float menu_label_scale = 0.5f;

			if (menu_tile == *current_menu_tile) menu_label_scale = menu_scale - 0.5f;
			else if (menu_tile == *last_menu_tile) menu_label_scale = 1.0f + max_menu_scale - menu_scale - 0.5f;

			vcr_osd_mono->render_text(
				menu_tile->label,
				menu_tile->position.x - 35.0f + menu_tile->label_x_offset, 
				menu_tile->position.y - 10.0f,
				menu_label_scale, glm::vec3(0.0f)
			);
		}
	}
}

# endif