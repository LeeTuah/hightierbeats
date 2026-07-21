# ifndef GAME_HPP_RENDER_GAME
# define GAME_HPP_RENDER_GAME

# include "../game.hpp"

inline void Game::render_game() {
	projection = glm::perspective(camera->zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera->get_lookat_matrix();
	text_projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
	model = glm::mat4(1.0f);

	auto format_int_to_str = [&](int total_size, int number) {
		std::string num = std::to_string(number);
		
		if (num.size() > total_size) return std::string("");
		else if (num.size() == total_size) return num;

		std::string zeroes(total_size - num.size(), '0');
		return zeroes + num;
	};

	std::string beat_timing_msg = "", score_msg = "", combo_msg = "", max_combo_msg = "";
	glm::vec3 beat_text_color = glm::vec3(0.0f);

	if (last_beat_status == BEAT_GOOD) {
		beat_timing_msg = "GOOD";
		beat_text_color = glm::vec3(1.0f, 1.0f, 0.0f);
	} else if (last_beat_status == BEAT_GREAT) {
		beat_timing_msg = "GREAT";
		beat_text_color = glm::vec3(0.0f, 1.0f, 0.0f);
	} else if (last_beat_status == BEAT_PERFECT) {
		beat_timing_msg = "PERFECT";
		beat_text_color = glm::vec3(0.0f, 1.0f, 1.0f);
	}
	score_msg = format_int_to_str(8, score_point);
	combo_msg = "x" + format_int_to_str(3, combo_point);
	max_combo_msg = "x" + format_int_to_str(3, max_combo_reached);

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
	model = glm::translate(model, core.position + glm::vec3(3.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 9.0f * IMAGE_SCALE_FACTOR, 16.0f * IMAGE_SCALE_FACTOR));
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
	model = glm::rotate(model, glm::radians(-180.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
	flat_shader->set_mat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	main_shader->use();

	glBindVertexArray(shard_VAO);
	main_shader->set_mat4("projection", projection);
	main_shader->set_mat4("view", view);
	
	main_shader->set_float3("viewer_pos", camera->position);

	main_shader->set_float3("shard.ambient", 1.0f, 1.0f, 1.0f);
	main_shader->set_float3("shard.diffuse", 1.0f, 1.0f, 1.0f);
	main_shader->set_float3("shard.specular", 1.0f, 1.0f, 1.0f);

	main_shader->set_float("shard.shininess", 32.0f);

	main_shader->set_int("allocated_suns", sun_directions.size());
	for (size_t i = 0; i < sun_directions.size(); i++){
		std::string name = "suns[" + std::to_string(i) + "].";

		main_shader->set_float3(name + "ambient", 0.1f, 0.1f, 0.1f);
		main_shader->set_float3(name + "diffuse", 0.3f, 0.3f, 0.3f);
		main_shader->set_float3(name + "specular", 0.0f, 0.0f, 0.0f);

		main_shader->set_float3(name + "direction", sun_directions[i]);
	}

	main_shader->set_float3("shard.color", glm::vec3(1.0f));
	for (auto &shard : shards) {
		if (not shard.active) continue;

		model = glm::mat4(1.0f);
		model = glm::translate(model, shard.position);
		if (shard.rotation_angle != 0.0f)
			model = glm::rotate(model, glm::radians(shard.rotation_angle), glm::normalize(shard.rotation_axis));

		model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
		main_shader->set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 24);
	}

	float del_change_for_hp = 1.0f - ((float)health_point / 100.0f);
	core_color.x = del_change_for_hp;
	core_color.y = 1.0f - del_change_for_hp;
	main_shader->set_float3("shard.color", core_color);
	
	if (game_state == GAME_RUNNING) {
		glBindVertexArray(core_VAO);
		core.rotation_angle = 25.0f * glfwGetTime();

		model = glm::mat4(1.0f);
		model = glm::translate(model, core.position + glm::vec3(0.0f, core_offset_one, core_offset_two));
		model = glm::rotate(model, (float)glm::radians(core.rotation_angle), glm::normalize(core.rotation_axis));
		main_shader->set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 60);

		if (core.active) {
			model = glm::scale(model, glm::vec3(1.1f));
			main_shader->set_mat4("model", model);
			glDrawArrays(GL_LINE_LOOP, 0, 60);
		}
	} else if (game_state == GAME_ZERO_HP and is_sound_playing) {
		particle_shader->use();

		particle_shader->set_mat4("projection", projection);
		particle_shader->set_mat4("view", view);
		
		particle_shader->set_float3("viewer_pos", camera->position);

		particle_shader->set_float3("shard.ambient", 1.0f, 1.0f, 1.0f);
		particle_shader->set_float3("shard.diffuse", 1.0f, 1.0f, 1.0f);
		particle_shader->set_float3("shard.specular", 1.0f, 1.0f, 1.0f);

		particle_shader->set_float("shard.shininess", 32.0f);

		particle_shader->set_int("allocated_suns", sun_directions.size());
		for (size_t i = 0; i < sun_directions.size(); i++){
			std::string name = "suns[" + std::to_string(i) + "].";

			particle_shader->set_float3(name + "ambient", 0.1f, 0.1f, 0.1f);
			particle_shader->set_float3(name + "diffuse", 0.3f, 0.3f, 0.3f);
			particle_shader->set_float3(name + "specular", 0.0f, 0.0f, 0.0f);

			particle_shader->set_float3(name + "direction", sun_directions[i]);
		}

		particle_shader->set_float3("shard.color", core_color);
		particle_shader->set_float("scale", 0.09f);

		glBindVertexArray(core_particle_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, core_particle_VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * losing_shards.size(), losing_shards.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0));
		glEnableVertexAttribArray(2);
		glVertexAttribDivisor(2, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 24, total_losing_shards);
		
		main_shader->use();
	}

	if (game_state == GAME_RUNNING or game_state == GAME_PAUSED) {
		main_shader->set_float3("shard.color", glm::vec3(1.0f));
		glBindVertexArray(shield_VAO);
		shield.rotation_angle = (float)shield.alignment;

		model = glm::mat4(1.0f);
		model = glm::translate(model, core.position);
		if (shield.rotation_angle != 0)
			model = glm::rotate(model, glm::radians(shield.rotation_angle), glm::normalize(shield.rotation_axis));
		model = glm::translate(model, shield.position - core.position);
		main_shader->set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 24 * shield.segments + 12);

		screen_shader->use(); 
		glBindVertexArray(square_VAO);

		float starting_pos = -0.92f;
		int total_filled_hp_boxes = health_point / 5;
		if (total_filled_hp_boxes == 0 and health_point > 0)
			total_filled_hp_boxes = 1;

		for (int i = 0; i < total_filled_hp_boxes; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(starting_pos, 0.85f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.0f));
			screen_shader->set_mat4("model", model);

			float red_channel = del_change_for_hp;
			float green_channel = std::max(0.65f - (red_channel / 5.0f), 0.0f);
			float blue_channel = std::max(0.83f - red_channel, 0.0f);
			screen_shader->set_float3("frag_color", red_channel, green_channel, blue_channel);

			starting_pos += 0.04f;
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		if (game_state == GAME_PAUSED) {
			flat_shader->use();
			glBindVertexArray(rect_VAO);

			flat_shader->set_mat4("projection", projection);
			flat_shader->set_mat4("view", view);

			flat_shader->set_bool("use_texture", false);
			flat_shader->set_float3("color", glm::vec3(0.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 5.0f, 4.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

			flat_shader->set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisable(GL_DEPTH_TEST);

			flat_shader->set_mat4("projection", text_projection);
			flat_shader->set_mat4("view", glm::mat4(1.0f));
			glBindVertexArray(menu_tile_VAO);

			for (auto pause_tile : pause_menu_tiles) {
				flat_shader->set_float3("color", pause_tile->color);

				model = glm::mat4(1.0f);
				model = glm::translate(model, pause_tile->position);
				
				if (pause_tile == *current_pause_menu_tile and pause_tile->active) 
					model = glm::scale(model, glm::vec3(menu_scale));

				else if (pause_tile == *last_pause_menu_tile and not pause_tile->active)
					model = glm::scale(model, glm::vec3(0.6f + max_pause_scale - menu_scale));
				
				else 
					model = glm::scale(model, pause_tile->scale);

				flat_shader->set_mat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 18);
			}

			vcr_osd_mono->render_text("PAUSED", SCR_WIDTH / 2 - 85.0f, SCR_HEIGHT / 2 + 85.0f, 1.0f, glm::vec3(1.0f));
			
			for (auto pause_tile : pause_menu_tiles) {
				float pause_label_scale = 0.5f; 

				if (pause_tile == *current_pause_menu_tile) 
					pause_label_scale = 0.5f + (menu_scale - 0.6f);
				else if (pause_tile == *last_pause_menu_tile) 
					pause_label_scale = 0.5f + (max_pause_scale - menu_scale);

				vcr_osd_mono->render_text(
					pause_tile->label,
					pause_tile->position.x - 35.0f + pause_tile->label_x_offset, 
					pause_tile->position.y - 10.0f,
					pause_label_scale, glm::vec3(0.0f)
				);
			}
		}

		glDisable(GL_DEPTH_TEST);

		if (fps_counter)
			vcr_osd_mono->render_text(std::to_string((int)fps), 20, 20, 0.5f, glm::vec3(1.0f));

		vcr_osd_mono->render_text(beat_timing_msg, (SCR_WIDTH / 2) - 20, SCR_HEIGHT - 100, 0.5f, beat_text_color);
		vcr_osd_mono->render_text(score_msg, SCR_WIDTH - 147, SCR_HEIGHT - 50, 0.6f, glm::vec3(1.0f));
		vcr_osd_mono->render_text(combo_msg, SCR_WIDTH - 80, SCR_HEIGHT - 80, 0.6f, glm::vec3(1.0f));
	} else if (game_state == GAME_WIN) {
		glDisable(GL_DEPTH_TEST);

		flat_shader->use();
		glBindVertexArray(rect_VAO);

		flat_shader->set_mat4("projection", projection);
		flat_shader->set_mat4("view", view);

		flat_shader->set_bool("use_texture", false);
		flat_shader->set_float3("color", glm::vec3(0.0f));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 9.0f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

		flat_shader->set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		std::string skill_rating = "";
		float skill_rating_x_offset = -65.0f, skill_rating_scale = 5.0f;
		float labels_x_offset = -195.0f, label_y_offset = -100.0f, label_gap = -50.0f;

		if      (total_accuracy <= (float)SKILL_F)   skill_rating = "F";
		else if (total_accuracy <= (float)SKILL_D)   skill_rating = "D";
		else if (total_accuracy <= (float)SKILL_C)   skill_rating = "C";
		else if (total_accuracy <= (float)SKILL_B)   skill_rating = "B";
		else if (total_accuracy <= (float)SKILL_A)   skill_rating = "A";
		else if (total_accuracy <= (float)SKILL_S)   skill_rating = "S";
		else if (total_accuracy <= (float)SKILL_SS)  skill_rating = "SS";
		else if (total_accuracy <= (float)SKILL_SSP) skill_rating = "ZZ";
		
		skill_rating_x_offset *= skill_rating.size();

		if (win_animation_style == SCORES_BUILD_UP) {
			std::string del_score_str = "", del_combo_str = "", del_acc_str = "";

			if (win_screen_animation_completed)
			vcr_osd_mono->render_text(
				skill_rating, 
				SCR_WIDTH / 2 + skill_rating_x_offset, SCR_HEIGHT / 2 + 50.0f, 
				skill_rating_scale, glm::vec3(1.0f)
			);

			del_score_str = format_int_to_str(8, del_score);
			del_combo_str = "x" + format_int_to_str(3, del_combo);
			del_acc_str = std::format("{:.2f}", del_accuracy) + "%";

			vcr_osd_mono->render_text(
				"Score: ", SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset,
				0.6f, glm::vec3(1.0f)
			);
			vcr_osd_mono->render_text(
				del_score_str, SCR_WIDTH / 2 + 60.0f, SCR_HEIGHT / 2 + label_y_offset,
				0.6f, glm::vec3(1.0f)
			);

			vcr_osd_mono->render_text(
				"Accuracy: ", SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset + label_gap,
				0.6f, glm::vec3(1.0f)
			);
			vcr_osd_mono->render_text(
				del_acc_str, SCR_WIDTH / 2 + 94.0f, SCR_HEIGHT / 2 + label_y_offset + label_gap,
				0.6f, glm::vec3(1.0f)
			);
			
			vcr_osd_mono->render_text(
				"Max Combo: ", SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset + 2 * label_gap,
				0.6f, glm::vec3(1.0f)
			);
			vcr_osd_mono->render_text(
				del_combo_str, SCR_WIDTH / 2 + 128.0f, SCR_HEIGHT / 2 + label_y_offset + 2 * label_gap,
				0.6f, glm::vec3(1.0f)
			);

			if (win_screen_animation_completed)
			vcr_osd_mono->render_text(
				"Press ENTER to continue", SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset + 3 * label_gap,
				0.5f, glm::vec3(1.0f)
			);
		}
		else if (win_animation_style == SCORES_PUNCHED_IN) {
			auto draw_win_label = [&](WinLabels* label){
				std::string win_label = "";

				if (label->big_label) {
					win_label = skill_rating;
				} else if (label->label == "Press ENTER to continue") {
					win_label = label->label;
				} else {
					win_label = label->label + ": ";
					if (label->label == "Score") win_label += score_msg;
					else if (label->label == "Max Combo") win_label += max_combo_msg;
					else if (label->label == "Accuracy") win_label += std::format("{:.2f}", total_accuracy) + "%";
				}

				float y_offset = label->scale * 25.0f;
				glm::mat4 original_text_projection = text_projection;

				text_projection = glm::translate(text_projection, glm::vec3(label->position.x + label->x_offset, label->position.y + y_offset, 0.0f));
				text_projection = glm::rotate(text_projection, label->rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));

				text_projection = glm::translate(text_projection, glm::vec3(-label->x_offset, -y_offset, 0.0f));
				text_projection = glm::translate(text_projection, glm::vec3(-label->position.x, -label->position.y, 0.0f));

				vcr_osd_mono->render_text(
					win_label, 
					label->position.x, label->position.y,
					label->scale, glm::vec3(1.0f)
				);

				text_projection = original_text_projection;
			};

			for (auto label : win_label_animation_order) {
				if (not label->animated) continue;

				draw_win_label(label);
			}

			if (current_win_label != win_label_animation_order.end())
				draw_win_label(*current_win_label);
		}
	}
}

# endif