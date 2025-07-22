/**
* Author: Lucy Zheng
* Assignment: Reflexion
* Date due: 05/02/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "IntroScene.h"
#include "Utility.h"

IntroScene::~IntroScene()
{
}

void IntroScene::initialise()
{
    m_scene_type = START;
    m_game_state.next_scene_id = -1;
    m_scene_skip = false;
    m_is_enemy_off = false;

    m_font_texture_id = Utility::load_texture("assets/font2.png");
}

void IntroScene::update(float delta_time) {
    if (m_scene_skip) m_game_state.next_scene_id = 2;
}

void IntroScene::render(ShaderProgram* g_shader_program, ShaderProgram* g_text_shader_program, ShaderProgram* g_map_shader_program)
{
    Utility::draw_text(g_shader_program, m_font_texture_id, "Navigate your clone to the", 0.525f, 0.0f,
        glm::vec3(-6.45f, 5.5f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "exit while avoiding enemies", 0.525f, 0.0f,
        glm::vec3(-6.65f, 4.5f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "PS: there is an object", 0.5f, 0.0f,
        glm::vec3(-5.3f, 2.0f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "you will need to move", 0.5f, 0.0f,
        glm::vec3(-5.15f, 1.0f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "and a pressure plate", 0.5f, 0.0f,
        glm::vec3(-4.95f, 0.0f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "PRESS ENTER TO CONTINUE", 0.4f, 0.0f,
        glm::vec3(-4.65f, -3.25f, 0.0f));
}