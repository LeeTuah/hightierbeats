# ifndef GAME_HPP_LOAD_BEATMAP
# define GAME_HPP_LOAD_BEATMAP

# include "../game.hpp"

inline void Game::load_beatmap_from_file(int index) {
	// temporary beatmap to test stuff (will be changed later)
	Shard test_note;
	test_note.active = false;
	test_note.destroyed = false;
	test_note.velocity = 8.0f; 

	test_note.alignment = W;  test_note.impact_time = 3.0f;  shards.push_back(test_note);
	test_note.alignment = S;  test_note.impact_time = 3.5f;  shards.push_back(test_note);
	test_note.alignment = A;  test_note.impact_time = 4.0f;  shards.push_back(test_note);
	test_note.alignment = D;  test_note.impact_time = 4.5f;  shards.push_back(test_note);

	test_note.alignment = WD; test_note.impact_time = 5.0f;  shards.push_back(test_note);
	test_note.alignment = SA; test_note.impact_time = 5.25f; shards.push_back(test_note);
	test_note.alignment = WA; test_note.impact_time = 5.5f;  shards.push_back(test_note);
	test_note.alignment = DS; test_note.impact_time = 5.75f; shards.push_back(test_note);

	test_note.alignment = W;  test_note.impact_time = 7.0f;  shards.push_back(test_note);
	test_note.alignment = WD; test_note.impact_time = 7.2f;  shards.push_back(test_note);
	test_note.alignment = D;  test_note.impact_time = 7.4f;  shards.push_back(test_note);
	test_note.alignment = DS; test_note.impact_time = 7.6f;  shards.push_back(test_note);
	test_note.alignment = S;  test_note.impact_time = 7.8f;  shards.push_back(test_note);
	test_note.alignment = SA; test_note.impact_time = 8.0f;  shards.push_back(test_note);
	test_note.alignment = A;  test_note.impact_time = 8.2f;  shards.push_back(test_note);
	test_note.alignment = WA; test_note.impact_time = 8.4f;  shards.push_back(test_note);

	test_note.alignment = W;  test_note.impact_time = 9.5f;  shards.push_back(test_note);
	test_note.alignment = S;  test_note.impact_time = 9.7f;  shards.push_back(test_note);
	
	test_note.alignment = A;  test_note.impact_time = 10.2f; shards.push_back(test_note);
	test_note.alignment = D;  test_note.impact_time = 10.4f; shards.push_back(test_note);

	test_note.alignment = WD; test_note.impact_time = 10.9f; shards.push_back(test_note);
	test_note.alignment = SA; test_note.impact_time = 11.1f; shards.push_back(test_note);

	test_note.velocity = 12.0f; 

	test_note.alignment = W;  test_note.impact_time = 12.5f; shards.push_back(test_note);
	test_note.alignment = W;  test_note.impact_time = 12.7f; shards.push_back(test_note);
	test_note.alignment = W;  test_note.impact_time = 12.9f; shards.push_back(test_note);
	
	test_note.alignment = D;  test_note.impact_time = 13.3f; shards.push_back(test_note);
	test_note.alignment = D;  test_note.impact_time = 13.5f; shards.push_back(test_note);
	
	test_note.alignment = A;  test_note.impact_time = 13.9f; shards.push_back(test_note);
	test_note.alignment = D;  test_note.impact_time = 14.1f; shards.push_back(test_note);
}

# endif