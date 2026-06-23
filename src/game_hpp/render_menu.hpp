# ifndef GAME_HPP_RENDER_MENU
# define GAME_HPP_RENDER_MENU

# include "../game.hpp"

inline void Game::render_menu() {
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);

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
	// model = glm::translate(model, glm::vec3(SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));

	flat_shader->set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(menu_tile_VAO);
	flat_shader->set_bool("use_texture", false);

	flat_shader->set_mat4("projection", text_projection);
	flat_shader->set_mat4("view", view);

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

}

# endif