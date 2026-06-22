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
	glBindVertexArray(menu_tile_VAO);

	flat_shader->set_mat4("projection", text_projection);
	flat_shader->set_mat4("view", view);

	flat_shader->set_bool("use_texture", false);

	for (auto menu_tile : main_menu_tiles) {
		flat_shader->set_float3("color", menu_tile->color);

		model = glm::mat4(1.0f);
		model = glm::translate(model, menu_tile->position);
		model = glm::scale(model, menu_tile->scale);

		if (menu_tile == *current_menu_tile and menu_tile->active) 
			model = glm::scale(model, glm::vec3(1.1f));

		flat_shader->set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 18);
	}

}

# endif