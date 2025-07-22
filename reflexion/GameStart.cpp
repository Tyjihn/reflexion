/**
* Author: Lucy Zheng
* Assignment: Reflexion
* Date due: 05/02/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "GameStart.h"
#include "Utility.h"

GameStart::~GameStart()
{
    delete m_game_state.background;
}

void GameStart::initialise()
{
    // ----- Camera ----- //
    float mult = 1.8f;
    *m_view_matrix = glm::translate(*m_view_matrix, glm::vec3(0.05f, -1.24f, 1.0f));
    *m_projection_matrix = glm::ortho(-5.0f * mult, 5.0f * mult, -3.75f * mult, 3.75f * mult, -1.0f * mult, 1.0f * mult);

    // ----- Background ----- //
    m_scene_type = START;
    m_game_state.next_scene_id = -1;
    m_scene_skip = false;
    m_is_enemy_off = false;

    constexpr float bg_mult = 15.0f;
    GLuint background_texture_id = Utility::load_texture("assets/background.png");

    m_game_state.background = new Entity(background_texture_id, BACKGROUND);

    m_game_state.background->set_position(glm::vec3(0.0f, 1.25f, 0.0f));
    m_game_state.background->set_scale(glm::vec3(1.2f * bg_mult, 0.9f * bg_mult, 1.0f));

    // ----- Text ----- //
    m_font_texture_id = Utility::load_texture("assets/font2.png");
}

void GameStart::update(float delta_time)
{
    m_game_state.background->update(delta_time, NULL, NULL, NULL, NULL);

    // ----- Switch Scene ----- //
    if (m_scene_skip) m_game_state.next_scene_id = 1;
}

void GameStart::render(ShaderProgram* g_shader_program, ShaderProgram* g_text_shader_program, ShaderProgram* g_map_shader_program)
{
    // ----- Shader Programs ----- //
    g_text_shader_program->set_projection_matrix(*m_projection_matrix);
    g_text_shader_program->set_view_matrix(*m_view_matrix);
    g_shader_program->set_view_matrix(*m_view_matrix);
    g_shader_program->set_projection_matrix(*m_projection_matrix);
    g_map_shader_program->set_projection_matrix(*m_projection_matrix);
    g_map_shader_program->set_view_matrix(*m_view_matrix);

    // ----- Background ----- //
    m_game_state.background->render(g_map_shader_program);

    // ----- Text ----- //
    Utility::draw_text(g_shader_program, m_font_texture_id, "REFLEXION", 1.25f, 0.0f,
        glm::vec3(-5.05f, 2.75f, 0.0f));
    Utility::draw_text(g_text_shader_program, m_font_texture_id, "PRESS ENTER TO START GAME", 0.4f, 0.0f,
        glm::vec3(-5.0f, -2.25f, 0.0f));
}