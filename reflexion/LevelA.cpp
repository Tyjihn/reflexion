/**
* Author: Lucy Zheng
* Assignment: Reflexion
* Date due: 05/02/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 23
#define LEVEL_HEIGHT 18

static unsigned int LEVEL_DATA[] =
{
    99, 59, 59, 59, 59, 59, 59, 59,154, 59,154, 59, 59, 59, 66, 67, 68, 61, 59, 59, 59, 59, 56,
    57, 10, 10, 10, 10, 10, 10, 11,140, 11,140, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 87,
    57, 37, 37, 37, 37, 37, 37, 39,126, 39,140, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57,169,128, 37, 37, 37, 37, 20,168, 39,140, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57, 39,197,141,141,141,141,141,129, 39,140, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57, 20, 10, 10, 10, 10, 10, 10,168, 39,140, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
   157,141,141,141,141,129, 37, 37, 37, 39,140,169,196,141,141,141,182,141,141,129, 37, 39, 87,
    57, 10, 10, 10, 10,168, 37,169,128, 39,140, 39,126, 10, 10, 11,140, 10, 10,168, 37, 39, 87,
    57, 37,169,196,129,169,127,141,198, 39,140, 20,168,169,128, 39,140,169,127,141,182,141,155,
    57, 37, 39,140,168, 20, 10, 10,168, 39,183,141,141,182,198, 39,140, 20, 10, 11,140, 11, 87, 
    57, 37, 39,126, 37,169,196,141,141,141,185, 10, 11,140,168, 39,140, 37,128, 39,140, 39, 87,
    57, 37, 20,168, 37, 39,140, 10, 10, 11,140,169,127,184,129, 39,197,141,198, 39,126, 39, 87,
   157,141,129, 37, 37, 39,140, 37, 37, 39,140, 20, 10, 10,168, 20, 10, 10,168, 20,168, 39, 87,
    57, 10,168, 37, 37, 39,140, 37, 37, 39,183,141,141,141,141,129,169,127,141,141,141,141,155,
    57, 37, 37, 37,169,127,198, 37, 37, 39,140, 10, 10, 10, 10,168, 20, 10, 10, 10, 10, 11, 87,
    57, 37, 37, 37, 20, 10,168, 37, 37, 39,140, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    57, 37, 37, 37, 37, 37, 37, 37, 37, 39,140, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 39, 87,
    84, 85, 85, 85, 85, 85, 85, 85, 85, 85,156, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,101,
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.clone;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.walk_sfx);
}

void LevelA::initialise()
{
    // ----- Camera ----- //
    float mult = 2.3f;
    *m_projection_matrix = glm::ortho(-5.0f * mult, 5.0f * mult, -3.75f * mult, 3.75f * mult, -1.0f * mult, 1.0f * mult);
    
    m_level_top_edge = -8.1f;
    m_level_bottom_edge = -8.45f;

    // ----- Game Assets ----- //
    m_scene_type = LEVEL;
    m_game_state.next_scene_id = -1;
    m_scene_skip = false;
    m_is_enemy_off = false;

    m_font_texture_id = Utility::load_texture("assets/font2.png");
    
    GLuint map_texture_id = Utility::load_texture("assets/library_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 14, 15);

    // ----- Character ----- //
    std::vector<std::vector<std::vector<int>>> character_animations =
    {
        {                               // Idle
            { 0, 1, 2, 3 },                 // left
            { 0, 1, 2, 3 },                 // right
            { 0, 1, 2, 3 },                 // up
            { 0, 1, 2, 3 },                 // down
        },
        {                               // Walk
            { 0, 1, 2, 3, 4, 5, 6, 7 },     // left
            { 0, 1, 2, 3, 4, 5, 6, 7 },     // right
            { 0, 1, 2, 3, 4, 5, 6, 7 },     // up
            { 0, 1, 2, 3, 4, 5, 6, 7 }      // down
        },
        {                               // Hurt
            { 0,  1,  2,  3 },              // left
            { 0,  1,  2,  3 },              // right
            { 0,  1,  2,  3 },              // up
            { 0,  1,  2,  3 }               // down
        }
    };

    std::vector<std::vector<GLuint>> character_texture_ids =
    {
        {
            Utility::load_texture("assets/character/idle/left.png"),
            Utility::load_texture("assets/character/idle/right.png"),
            Utility::load_texture("assets/character/idle/up.png"),
            Utility::load_texture("assets/character/idle/down.png")
        },
        {
            Utility::load_texture("assets/character/walk/left.png"),
            Utility::load_texture("assets/character/walk/right.png"),
            Utility::load_texture("assets/character/walk/up.png"),
            Utility::load_texture("assets/character/walk/down.png")
        },
        {
            Utility::load_texture("assets/character/hurt/left.png"),
            Utility::load_texture("assets/character/hurt/right.png"),
            Utility::load_texture("assets/character/hurt/up.png"),
            Utility::load_texture("assets/character/hurt/down.png")
        }
    };

    // ----- Player Initialization ----- //
    m_game_state.player = new Entity(
        character_texture_ids,     // texture id
        3.0f,                      // speed
        glm::vec3(0.0f),           // acceleration
        character_animations,      // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        1,                         // animation row amount
        0.9f,                      // width
        0.9f,                      // height
        CHARACTER,                 // entity type
        PLAYER,                    // character type
        REST,                      // character state
        UP                         // direction
    );

    m_game_state.player->set_position(glm::vec3(6.0f, -16.0f, 0.0f));
    
    // ----- Clone Initialization ----- //
    m_game_state.clone = new Entity(
        character_texture_ids,     // texture id
        3.0f,                      // speed
        glm::vec3(0.0f),           // acceleration
        character_animations,      // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        1,                         // animation row amount
        0.9f,                      // width
        0.9f,                      // height
        CHARACTER,                 // entity type
        CLONE,                     // character type
        REST,                      // character state
        UP                         // direction
    );

    m_game_state.clone->set_position(glm::vec3(15.0f, -16.0f, 0.0f));

    // ----- Enemies Initialization ----- //
    std::vector<std::vector<std::vector<int>>> slime_animations =
    {
        {                               // Idle
            { 0, 1, 2, 3 },                 // left
            { 0, 1, 2, 3 },                 // right
            { 0, 1, 2, 3 },                 // up
            { 0, 1, 2, 3 }                  // down
        },
        {                               // Move
            { 0, 1, 2, 3, 4, 5, 6 },        // left
            { 0, 1, 2, 3, 4, 5, 6 },        // right
            { 0, 1, 2, 3, 4, 5, 6 },        // up
            { 0, 1, 2, 3, 4, 5, 6 }         // down
        }
    };

    std::vector<std::vector<GLuint>> slime_texture_ids =
    {
        {
            Utility::load_texture("assets/enemies/slime/idle_left.png"),
            Utility::load_texture("assets/enemies/slime/idle_right.png"),
            Utility::load_texture("assets/enemies/slime/idle_right.png"),
            Utility::load_texture("assets/enemies/slime/idle_left.png")
        },
        {
            Utility::load_texture("assets/enemies/slime/move_left.png"),
            Utility::load_texture("assets/enemies/slime/move_right.png"),
            Utility::load_texture("assets/enemies/slime/move_right.png"),
            Utility::load_texture("assets/enemies/slime/move_left.png")
        }
    };

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT - 1; i++)
    {
        m_game_state.enemies[i] = Entity(
            slime_texture_ids,      // texture ids
            2.0f,                   // speed
            glm::vec3(0.0f),        // acceleration
            slime_animations,       // animation index sets
            0.0f,                   // animation time
            6,                      // animation frame amount
            0,                      // current animation index
            6,                      // animation column amount
            1,                      // animation row amount
            0.8f,                   // width
            0.6f,                   // height
            ENEMY,                  // entity type
            WALKER,                 // AI type
            IDLE,                   // AI state
            RIGHT                   // direction
        );
    }

    if (ENEMY_COUNT >= 1) m_game_state.enemies[0].set_position(glm::vec3(12.0f, -3.0f, 0.0f));
    if (ENEMY_COUNT >= 2) m_game_state.enemies[1].set_position(glm::vec3(16.0f, -2.0f, 0.0f));
    if (ENEMY_COUNT >= 3) m_game_state.enemies[2].set_position(glm::vec3(20.0f, -4.0f, 0.0f));

    // ----- Projectile Initialization ----- //
    std::vector<std::vector<std::vector<int>>> projectile_animations =
    {
        {                                   // Idle
            { 0, 1, 2, 3, 4, 5, 6, 7 },         // left
            { 0, 1, 2, 3, 4, 5, 6, 7 },         // right 
        },                               
        {                                   // Move
            { 0, 1, 2, 3, 4, 5, 6, 7 },         // left
            { 0, 1, 2, 3, 4, 5, 6, 7 },         // right
        },
        {  }                                // Attack
    };

    std::vector<std::vector<GLuint>> projectile_texture_ids =
    {
        {
            Utility::load_texture("assets/enemies/axe/axe_left.png"),
            Utility::load_texture("assets/enemies/axe/axe_right.png"), 
        },
        {
            Utility::load_texture("assets/enemies/axe/axe_left.png"),
            Utility::load_texture("assets/enemies/axe/axe_right.png"),
        }
    };

    m_game_state.enemies[ENEMY_COUNT - 1] = Entity(
        projectile_texture_ids, // texture ids
        5.0f,                   // speed
        glm::vec3(0.0f),        // acceleration
        projectile_animations,  // animation index sets
        0.0f,                   // animation time
        8,                      // animation frame amount
        0,                      // current animation index
        8,                      // animation column amount
        1,                      // animation row amount
        0.75f,                  // width
        0.75f,                  // height
        ENEMY,                  // entity type
        PROJECTILE,             // AI type
        IDLE,                   // AI state
        LEFT                    // direction
    );

    if (ENEMY_COUNT >= 4)
    {
        m_game_state.enemies[3].set_position(glm::vec3(21.0f, -15.0f, 0.0f));
        m_game_state.enemies[3].set_scale(glm::vec3(0.8f));
    }

    // ----- BGM and SFX ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.walk_sfx = Mix_LoadWAV("assets/sound/walk.wav");
}

void LevelA::update(float delta_time)
{
    if (m_is_enemy_off) {
        for (int i = 0; i < ENEMY_COUNT; i++) {
            m_game_state.enemies[i].deactivate();
        }
    }
    else {
        if (!m_game_state.enemies[0].get_is_active()) {
            for (int i = 0; i < ENEMY_COUNT; i++) {
                m_game_state.enemies[i].activate();
            }
        }
    }

    m_game_state.player->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    m_game_state.clone->update(delta_time, m_game_state.clone, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].update(delta_time, m_game_state.clone, NULL, NULL, m_game_state.map);
    }

    // ----- Player Camera ----- //
    *m_view_matrix = glm::mat4(1.0f);

    if (m_game_state.player->get_position().y < m_level_top_edge &&
        m_game_state.player->get_position().y > m_level_bottom_edge) {
        *m_view_matrix = glm::translate(*m_view_matrix,
            glm::vec3(-11, -m_game_state.player->get_position().y, 0));
    }
    else {
        if (m_game_state.player->get_position().y >= m_level_top_edge)
            *m_view_matrix = glm::translate(*m_view_matrix, glm::vec3(-11, 8.05, 0));
        else if (m_game_state.player->get_position().y <= m_level_bottom_edge)
            *m_view_matrix = glm::translate(*m_view_matrix, glm::vec3(-11, 8.91, 0));
    }

    // ----- Switch Scene ----- //
    if (m_game_state.clone->get_position().y >= -0.5f || m_scene_skip) m_game_state.next_scene_id = 4;

    // ----- Player Lives ----- //
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.clone->check_collision(&m_game_state.enemies[i]) 
            && m_game_state.enemies[i].get_is_active()) {
            player_death();
            break;
        }
    }
}

void LevelA::render(ShaderProgram* g_shader_program, ShaderProgram* g_clone_shader_program, ShaderProgram* g_map_shader_program)
{
    // ----- Shader Programs ----- //
    g_shader_program->set_projection_matrix(*m_projection_matrix);
    g_shader_program->set_view_matrix(*m_view_matrix);
    g_clone_shader_program->set_projection_matrix(*m_projection_matrix);
    g_clone_shader_program->set_view_matrix(*m_view_matrix);
    g_map_shader_program->set_projection_matrix(*m_projection_matrix);
    g_map_shader_program->set_view_matrix(*m_view_matrix);

    m_game_state.map->render(g_map_shader_program);
    m_game_state.player->render(g_shader_program);
    m_game_state.clone->render(g_clone_shader_program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].render(g_clone_shader_program);
    }
}

void LevelA::player_death() {
    set_player_lives(get_player_lives() - 1);  // Lose 1 life

    if (get_player_lives() <= 0) {
        m_game_state.next_scene_id = 6;  // Game Over Scene
    }
    else {
        initialise();  // Reset Level
    }
}