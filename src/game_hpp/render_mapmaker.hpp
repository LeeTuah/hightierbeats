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

	ImGui::Begin("HTB Map Maker");

	ImGui::SliderFloat("Timeline", &audio_time, 0.0f, 180.0f);
	if (ImGui::Button("Save Beatmap")) {

	}

	ImGui::ShowDemoWindow();

	ImGui::End();
}

# endif