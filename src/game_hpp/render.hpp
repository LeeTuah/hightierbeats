# ifndef GAME_HPP_RENDER
# define GAME_HPP_RENDER

# include "../game.hpp"

inline void Game::render() {
	glBindFramebuffer(GL_FRAMEBUFFER, ms_fbo);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glfwSwapInterval(enable_vsync);
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);

	bool ingame = game_state == GAME_RUNNING or
		game_state == GAME_ZERO_HP or
		game_state == GAME_WIN	   or
		game_state == GAME_PAUSED;

	if (ingame) render_game();

	else if (game_state == GAME_MAPMAKER) render_mapmaker();

	else if (
		game_state == GAME_MAIN_MENU         or
		game_state == GAME_SELECTING_BEATMAP or
		game_state == GAME_SETTINGS
	) render_menu();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, ms_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int original_width, original_height;
	glfwGetFramebufferSize(window, &original_width, &original_height);
	glViewport(0, 0, original_width, original_height);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	postprocess_shader->use();
	glBindVertexArray(rect_VAO);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(SCR_WIDTH, SCR_HEIGHT, 1.0f));

	postprocess_shader->set_mat4("model", model);
	postprocess_shader->set_mat4("view", glm::mat4(1.0f));
	postprocess_shader->set_mat4("projection", text_projection);

	glBindTexture(GL_TEXTURE_2D, fbo_color_buffer);
	glActiveTexture(GL_TEXTURE0);
	postprocess_shader->set_int("tex", 0);

	postprocess_shader->set_bool("hdr", enable_hdr_tonemapping);
	postprocess_shader->set_bool("chromatic", enable_chromatic_aberration and ingame);
	postprocess_shader->set_float2("ca_offset", glm::vec2(ca_offset));

	postprocess_shader->set_bool("vignette", enable_vignette and ingame);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
# endif