# ifndef GAME_HPP_RENDER
# define GAME_HPP_RENDER

# include "../game.hpp"

inline void Game::render() {
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
		model = glm::translate(model, core.position);
		model = glm::rotate(model, (float)glm::radians(core.rotation_angle), glm::normalize(core.rotation_axis));
		main_shader->set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 60);

		if (core.active) {
			model = glm::scale(model, glm::vec3(1.1f));
			main_shader->set_mat4("model", model);
			glDrawArrays(GL_LINE_LOOP, 0, 60);
		}
	} else if (game_state == GAME_ZERO_HP) {
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

	for (int i = 0; i < 20; i++) {
		if (total_filled_hp_boxes <= 0) continue;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(starting_pos, 0.85f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.0f));
		screen_shader->set_mat4("model", model);

		float red_channel = del_change_for_hp;
		float green_channel = std::max(0.65f - (red_channel / 5.0f), 0.0f);
		float blue_channel = std::max(0.83f - red_channel, 0.0f);
		screen_shader->set_float3("frag_color", red_channel, green_channel, blue_channel);

		starting_pos += 0.04f;
		total_filled_hp_boxes--;
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

# endif