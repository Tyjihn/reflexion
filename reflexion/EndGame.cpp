/**
* Author: Lucy Zheng
* Assignment: Reflexion
* Date due: 05/02/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "EndGame.h"
#include "Utility.h"

EndGame::~EndGame()
{
    Mix_FreeChunk(m_game_state.game_over_sfx);
}

void EndGame::initialise()
{
    *m_view_matrix = glm::mat4(1.0f);
    *m_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    m_scene_type = END;
    m_game_state.next_scene_id = -1;

    // ----- Text ----- //
    m_font_texture_id = Utility::load_texture("assets/font2.png");

    // ----- BGM and SFX ----- //
    Mix_HaltMusic(); // Stop BGM
}

void EndGame::update(float delta_time) {
    if (m_scene_skip) m_game_state.next_scene_id = 0;
}

void EndGame::render(ShaderProgram* g_shader_program, ShaderProgram* g_text_shader_program, ShaderProgram* g_map_shader_program)
{
    // ----- Shader Programs ----- //
    g_shader_program->set_projection_matrix(*m_projection_matrix);
    g_shader_program->set_view_matrix(*m_view_matrix);
    g_text_shader_program->set_projection_matrix(*m_projection_matrix);
    g_text_shader_program->set_view_matrix(*m_view_matrix);

    // ----- Sound Effect ----- //
    //if (!m_has_played_sound)
    //{
    //    if (m_player_lives <= 0) {
    //        m_game_state.game_over_sfx = Mix_LoadWAV("assets/music/game_over.wav");
    //        Mix_PlayChannel(0, m_game_state.game_over_sfx, 0);
    //    }
    //    else {
    //        m_game_state.game_over_sfx = Mix_LoadWAV("assets/music/game_win.wav");
    //        Mix_PlayChannel(0, m_game_state.game_over_sfx, 0);
    //    }
    //    m_has_played_sound = true;
    //}

    // ----- Background ----- //
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // ----- Text ----- //
    Utility::draw_text(g_shader_program, m_font_texture_id, "GAME OVER",
        0.6f, 0.0f, glm::vec3(-2.45f, 1.5f, 0.0f));

    if (m_player_lives <= 0) {
        Utility::draw_text(g_shader_program, m_font_texture_id, "YOU LOSE",
            0.5f, 0.0f, glm::vec3(-1.85f, 0.0f, 0.0f));
    }
    else {
        Utility::draw_text(g_shader_program, m_font_texture_id, "YOU WIN",
            0.5f, 0.0f, glm::vec3(-1.6f, 0.0f, 0.0f));
    }

    Utility::draw_text(g_shader_program, m_font_texture_id, "PRESS ENTER TO RETURN TO MAIN MENU", 0.2f, 0.0f,
        glm::vec3(-3.35f, -2.0f, 0.0f));
}