/**
* Author: Lucy Zheng
* Assignment: Reflexion
* Date due: 05/02/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "ControlsScene.h"
#include "Utility.h"

ControlsScene::~ControlsScene()
{
}

void ControlsScene::initialise()
{
    m_scene_type = START;
    m_game_state.next_scene_id = -1;
    m_scene_skip = false;
    m_is_enemy_off = false;

    m_font_texture_id = Utility::load_texture("assets/font2.png");
}

void ControlsScene::update(float delta_time) {
    if (m_scene_skip) m_game_state.next_scene_id = 3;
}

void ControlsScene::render(ShaderProgram* g_shader_program, ShaderProgram* g_text_shader_program, ShaderProgram* g_map_shader_program)
{
    Utility::draw_text(g_shader_program, m_font_texture_id, "Movement       [W][A][S][D]", 0.5f, 0.0f,
        glm::vec3(-5.1f, 5.2f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "Reset Level          [R]", 0.5f, 0.0f,
        glm::vec3(-5.8f, 4.2f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "Pause             [P]", 0.5f, 0.0f,
        glm::vec3(-4.3f, 3.2f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "Skip Level        [RETURN]", 0.5f, 0.0f,
        glm::vec3(-5.55f, 2.2f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "Specify Level      [1][2][3]", 0.5f, 0.0f,
        glm::vec3(-6.3f, 1.2f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "Remove/Include Enemies   [0]", 0.5f, 0.0f,
        glm::vec3(-7.8f, 0.2f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "PRESS ENTER TO START GAME", 0.4f, 0.0f,
        glm::vec3(-5.15f, -3.25f, 0.0f));
}