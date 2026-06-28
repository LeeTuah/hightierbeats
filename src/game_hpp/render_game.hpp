# ifndef GAME_HPP_RENDER_GAME
# define GAME_HPP_RENDER_GAME

# include "../game.hpp"

inline void Game::render_game() {
	// glfwSetInputMode for mouse later
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	std::string beat_timing_msg = "", score_msg = "", combo_msg = "";
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

	if (game_state == GAME_RUNNING) {
		if (fps_counter)
			vcr_osd_mono->render_text(std::to_string((int)fps), 20, 20, 0.5f, glm::vec3(1.0f));

		vcr_osd_mono->render_text(beat_timing_msg, (SCR_WIDTH / 2) - 20, SCR_HEIGHT - 100, 0.5f, beat_text_color);
		vcr_osd_mono->render_text(score_msg, SCR_WIDTH - 147, SCR_HEIGHT - 50, 0.6f, glm::vec3(1.0f));
		vcr_osd_mono->render_text(combo_msg, SCR_WIDTH - 80, SCR_HEIGHT - 80, 0.6f, glm::vec3(1.0f));
	}

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

	if (game_state == GAME_RUNNING) {
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
	} else if (game_state == GAME_WIN) {
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

		// TODO: animate the score gradually increase
		// TODO: add a pause menu
		std::string skill_rating = "";
		float skill_rating_x_offset = -65.0f, skill_rating_scale = 5.0f;

		if      (total_accuracy <= (float)SKILL_F)   skill_rating = "F";
		else if (total_accuracy <= (float)SKILL_D)   skill_rating = "D"; 
		else if (total_accuracy <= (float)SKILL_C)   skill_rating = "C"; 
		else if (total_accuracy <= (float)SKILL_B)   skill_rating = "B"; 
		else if (total_accuracy <= (float)SKILL_A)   skill_rating = "A"; 
		else if (total_accuracy <= (float)SKILL_S)   skill_rating = "S"; 
		else if (total_accuracy <= (float)SKILL_SS)  skill_rating = "SS"; 
		else if (total_accuracy <= (float)SKILL_SSP) skill_rating = "SS+";
		
		skill_rating_x_offset *= skill_rating.size();
		if (skill_rating.size() == 3) skill_rating_scale = 4.5f;

		vcr_osd_mono->render_text(
			skill_rating, 
			SCR_WIDTH / 2 + skill_rating_x_offset, SCR_HEIGHT / 2 + 50.0f, 
			skill_rating_scale, glm::vec3(1.0f)
		);

		float labels_x_offset = -195.0f, label_y_offset = -100.0f, label_gap = -50.0f;
		std::string accuracy_msg = std::format("{:.2f}", total_accuracy) + "%";

		vcr_osd_mono->render_text(
			"Score: ", SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset,
			0.6f, glm::vec3(1.0f)
		);
		vcr_osd_mono->render_text(
			score_msg, SCR_WIDTH / 2 + 60.0f, SCR_HEIGHT / 2 + label_y_offset,
			0.6f, glm::vec3(1.0f)
		);

		vcr_osd_mono->render_text(
			"Accuracy: ", SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset + label_gap,
			0.6f, glm::vec3(1.0f)
		);
		vcr_osd_mono->render_text(
			accuracy_msg, SCR_WIDTH / 2 + 94.0f, SCR_HEIGHT / 2 + label_y_offset + label_gap,
			0.6f, glm::vec3(1.0f)
		);
		
		vcr_osd_mono->render_text(
			"Max Combo: ", SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset + 2 * label_gap,
			0.6f, glm::vec3(1.0f)
		);
		vcr_osd_mono->render_text(
			combo_msg, SCR_WIDTH / 2 + 128.0f, SCR_HEIGHT / 2 + label_y_offset + 2 * label_gap,
			0.6f, glm::vec3(1.0f)
		);

		vcr_osd_mono->render_text(
			"Press ENTER to continue...", SCR_WIDTH / 2 + labels_x_offset, SCR_HEIGHT / 2 + label_y_offset + 3 * label_gap,
			0.5f, glm::vec3(1.0f)
		);
	}
}

# endif