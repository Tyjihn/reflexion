/**
* Author: Lucy Zheng
* Assignment: Reflexion
* Date due: 05/02/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "UI.h"

UI::UI() : TOTAL_LIVES(0), m_heart_texture_id(0), hearts(nullptr)
{ }

UI::~UI()
{
	delete[] hearts;
}

void UI::initialise(int player_lives)
{
	m_heart_texture_id = Utility::load_texture("assets/heart.png");
	TOTAL_LIVES = player_lives;

	hearts = new Entity[player_lives];

	for (int i = 0; i < player_lives; i++)
	{
		hearts[i] = Entity(
			m_heart_texture_id,		// texture id
			HEART					// Entity type
		);

		hearts[i].set_position(glm::vec3(5.0f, -12.0f, 0.0f));
		hearts[i].activate();
	}
}

void UI::update(glm::mat4 *view_matrix, glm::mat4 *projection_matrix, int player_lives)
{
	// Deactivate lost hearts
	for (int i = player_lives; i < TOTAL_LIVES; i++) {
		hearts[i].deactivate();
	}

    float screen_width = 2.0f / (*projection_matrix)[0][0];
    float screen_height = 2.0f / (*projection_matrix)[1][1];

	glm::vec3 camera_pos = -glm::vec3((*view_matrix)[3]);

    float x_offset = 1.0f;
	float heart_spacing = 0.25f;
    float y_offset = -0.75f;

    for (int i = 0; i < player_lives; i++)
    {
		float x_pos = (camera_pos.x + screen_width / 2) - x_offset - (i * (heart_spacing + hearts[i].get_width()));
		float y_pos = (camera_pos.y + screen_height / 2) + y_offset;
        hearts[i].set_position(glm::vec3(x_pos, y_pos, 0.0f));

		hearts[i].update(NULL, NULL, NULL, NULL, NULL);
    }
}

void UI::render(ShaderProgram* program) {
	for (int i = 0; i < TOTAL_LIVES; i++)
		hearts[i].render(program);
}