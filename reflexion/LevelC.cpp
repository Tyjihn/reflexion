/**
* Author: Lucy Zheng
* Assignment: Reflexion
* Date due: 05/02/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 18

unsigned int LEVEL_DATA[] =
{
    99, 59, 59,154, 61, 66, 67, 68, 61,154, 59, 59, 59,154, 59, 59, 59, 59, 59, 56,
    57, 10, 11,140, 10, 10, 10, 10, 11,140, 10, 10, 11,140, 10, 10, 10, 10, 11, 87,
    57, 37, 39,197,129, 37, 37, 37, 39,140, 37, 37, 39,140, 37,169,128, 37, 39, 87,
   157,129, 20, 10,168, 37,169,196,141,185,169,128, 39,126,169,196,198, 37, 39, 87,
    57,168, 37, 37,169,128, 39,126, 11,140, 39,140, 20,168, 39,126,168,169,127,155,
    57, 37,169,127,182,198, 20,168, 39,183,141,198, 37, 37, 20,168, 37, 20, 11, 87,
    57, 37, 20, 11,126, 37, 37, 37, 39,140, 10,168,169,127,182,141,129, 37, 39, 87,
    57, 37, 37, 20,168,169,127,199, 39,140,169,128, 20, 11,126, 10,168, 37, 39, 87,
   157,141,141,129, 37, 20, 11,126, 39,140, 39,126, 37, 20,168, 37, 37, 37, 39, 87,
    57, 10, 10,168, 37, 37, 20,168, 39,140, 20,168, 37, 37, 37,169,128, 37, 39, 87,
    57, 37, 37, 37, 37,169,127,141,141,185, 37, 37, 37, 37,169,127,184,141,141,155,
    57, 37, 37, 37, 37, 20, 10, 10, 11,140,169,127,199, 37, 20, 10, 10, 10, 11, 87,
   157,129, 37,169,127,182,129, 37, 39,140, 20, 11,140, 37, 37, 37, 37, 37, 39, 87,
    57,168, 37, 20, 11,140,168, 37, 39,183,141,141,184,141,129, 37,169,128, 39, 87,
    57, 37, 37, 37, 39,126, 37, 37, 39,140, 10, 10, 10, 10,168, 37, 39,197,141,155,
   157,141,141,129, 20,168, 37, 37, 39,140, 37, 37, 37,169,128, 37, 20, 10, 11, 87,
    57, 10, 10,168, 37, 37,169,128, 39,140, 37, 37, 37, 39,140, 37, 37, 37, 39, 87,
    84, 85, 85, 85, 85, 85, 85,156, 85,156, 85, 85, 85, 85,156, 85, 85, 85, 85,101
};

LevelC::~LevelC()
{
    delete[] m_game_state.enemies;
    delete[] m_game_state.blocks;
    delete   m_game_state.pressure_plate;
    delete   m_game_state.player;
    delete   m_game_state.clone;
    delete   m_game_state.map;
    Mix_FreeChunk(m_game_state.walk_sfx);
    Mix_FreeChunk(m_game_state.block_slide_sfx);
    Mix_FreeChunk(m_game_state.on_pressure_plate_sfx);
    Mix_FreeChunk(m_game_state.door_mechanic_sfx);
}

void LevelC::initialise()
{
    // ----- Camera ----- //
    float mult = 1.99f;
    *m_projection_matrix = glm::ortho(-5.0f * mult, 5.0f * mult, -3.75f * mult, 3.75f * mult, -1.0f * mult, 1.0f * mult);

    m_level_top_edge = -7.0f;
    m_level_bottom_edge = -10.0f;

    // ----- Game Assets ----- //
    m_scene_type = LEVEL;
    m_game_state.next_scene_id = -1;
    m_scene_skip = false;
    m_is_enemy_off = false;

    m_font_texture_id = Utility::load_texture("assets/font2.png");

    m_map_texture_id = Utility::load_texture("assets/library_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, m_map_texture_id, 1.0f, 14, 15);

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

    m_game_state.player->set_position(glm::vec3(5.0f, -16.0f, 0.0f));

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

    m_game_state.clone->set_position(glm::vec3(13.0f, -16.0f, 0.0f));

    // ----- Enemies Initialization ----- //
    m_game_state.enemies = new Entity[ENEMY_COUNT];

    std::vector<std::vector<int>> spike_animations =
    {
        { 0 },          // Idle
        { 0 },          // Ready
        { 0, 1, 2 }     // Attack
    };
    std::vector<GLuint> spike_texture_ids =
    {
        Utility::load_texture("assets/enemies/spike/idle.png"),
        Utility::load_texture("assets/enemies/spike/ready.png"),
        Utility::load_texture("assets/enemies/spike/attack.png")
    };
    m_game_state.enemies[0] = Entity(
        spike_texture_ids,      // texture ids
        spike_animations,       // animation index sets
        0.0f,                   // animation time
        1,                      // animation frame amount
        0,                      // current animation index
        1,                      // animation column amount
        1,                      // animation row amount
        0.5f,                   // width
        0.5f,                   // height
        TRAP,                   // entity type
        SPIKE,                  // obstacle type
        OFF                     // obstacle state
    );
    m_game_state.enemies[0].set_position(glm::vec3(9.0f, -4.0f, 0.0f));
    m_game_state.enemies[0].set_scale(glm::vec3(0.8f));

    // Projectile
    std::vector<std::vector<std::vector<int>>> projectile_animations =
    {
        {               // Idle
            { 0 },          // left
            { 0 },          // right
            { 0 },          // up
            { 0 }           // down
        },
        {               // Move
            { 0 },          // left
            { 0 },          // right
            { 0 },          // up
            { 0 }           // down
        }
    };

    std::vector<std::vector<GLuint>> projectile_texture_ids =
    {
        {
            Utility::load_texture("assets/enemies/arrow/left.png"),
            Utility::load_texture("assets/enemies/arrow/right.png"),
            Utility::load_texture("assets/enemies/arrow/up.png"),
            Utility::load_texture("assets/enemies/arrow/down.png")
        },
        {
            Utility::load_texture("assets/enemies/arrow/left.png"),
            Utility::load_texture("assets/enemies/arrow/right.png"),
            Utility::load_texture("assets/enemies/arrow/up.png"),
            Utility::load_texture("assets/enemies/arrow/down.png")
        }
    };

    m_game_state.enemies[1] = Entity(
        projectile_texture_ids, // texture ids
        6.0f,                   // speed
        glm::vec3(0.0f),        // acceleration
        projectile_animations,  // animation index sets
        0.0f,                   // animation time
        1,                      // animation frame amount
        0,                      // current animation index
        1,                      // animation column amount
        1,                      // animation row amount
        0.55f,                  // width
        0.15f,                  // height
        ENEMY,                  // entity type
        PROJECTILE,             // AI type
        IDLE,                   // AI state
        LEFT                    // direction
    );
    m_game_state.enemies[1].set_position(glm::vec3(18.0f, -11.0f, 0.0f));

    // Slime
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
    m_game_state.enemies[2] = Entity(
        slime_texture_ids,      // texture ids
        0.8f,                   // speed
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
    m_game_state.enemies[2].set_position(glm::vec3(2.0f, -3.0f, 0.0f));
    m_game_state.enemies[3] = Entity(
        slime_texture_ids,      // texture ids
        1.0f,                   // speed
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
        DOWN                    // direction
    );
    m_game_state.enemies[3].set_position(glm::vec3(12.0f, -1.0f, 0.0f));

    // ----- BLOCK ----- //
    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        m_game_state.enemies[ENEMY_COUNT - 2 + i] = Entity(
            Utility::load_texture("assets/obstacles/rock.png"),  // texture id
            3.0f,                                                   // speed
            0.8f,                                                   // width
            0.8f,                                                   // height
            OBSTACLE,                                               // entity type
            BLOCK,                                                  // obstacle type
            NOTHING                                                 // obstacle state
        );
    }
    m_game_state.enemies[ENEMY_COUNT - 2].set_position(glm::vec3(12.0f, -14.0f, 0.0f));
    m_game_state.enemies[ENEMY_COUNT - 2].set_scale(glm::vec3(0.9f));
    m_game_state.enemies[ENEMY_COUNT - 1].set_position(glm::vec3(17.0f, -6.0f, 0.0f));
    m_game_state.enemies[ENEMY_COUNT - 1].set_scale(glm::vec3(0.9f));

    // ----- Obstacles ----- //
    m_game_state.blocks = new Entity*[BLOCK_COUNT];
    m_game_state.blocks[0] = &m_game_state.enemies[ENEMY_COUNT - 2];
    m_game_state.blocks[1] = &m_game_state.enemies[ENEMY_COUNT - 1];

    std::vector<GLuint> pressure_plate_texture_ids =
    {
        Utility::load_texture("assets/obstacles/plate_off.png"),
        Utility::load_texture("assets/obstacles/plate_on.png")
    };
    m_game_state.pressure_plate = new Entity(
        pressure_plate_texture_ids,     // texture ids
        0.0f,                               // speed
        0.65f,                              // width
        0.65f,                              // height
        OBSTACLE,                           // entity type
        PRESSURE_PLATE,                     // obstacle type
        NOTHING                             // obstacle state
    );
    m_game_state.pressure_plate->set_position(glm::vec3(10.0f, -12.0f, 0.0f));

    // ----- BGM and SFX ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.walk_sfx = Mix_LoadWAV("assets/sound/walk.wav");
    m_game_state.block_slide_sfx = Mix_LoadWAV("assets/sound/block_slide.wav");
    m_game_state.on_pressure_plate_sfx = Mix_LoadWAV("assets/sound/on_plate.wav");
    m_game_state.door_mechanic_sfx = Mix_LoadWAV("assets/sound/door_mechanic.wav");
}

void LevelC::update(float delta_time)
{   
    // Reset Block Movement + Play Sound Effect
    bool is_pushing = false;
    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (m_game_state.blocks[i]->get_movement() != glm::vec3(0.0f)) is_pushing = true;
        m_game_state.blocks[i]->set_movement(glm::vec3(0.0f));
    }
    if (is_pushing && !Mix_Playing(3)) Mix_PlayChannel(3, m_game_state.block_slide_sfx, -1);
    else Mix_HaltChannel(3);

    // ----- Update Entities ----- //
    bool was_pressed = (m_game_state.pressure_plate->get_obstacle_state() == PRESSED);
    m_game_state.pressure_plate->update(delta_time, m_game_state.clone, *m_game_state.blocks, BLOCK_COUNT, m_game_state.map);
    if (!was_pressed && m_game_state.pressure_plate->get_obstacle_state() == PRESSED) {
        Mix_PlayChannel(4, m_game_state.on_pressure_plate_sfx, 0);
    }
    check_plate_pressed();

    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    m_game_state.clone->update(delta_time, m_game_state.clone, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].update(delta_time, m_game_state.clone, *m_game_state.blocks, BLOCK_COUNT, m_game_state.map);
    }

    // ----- Enemy Activation ----- //
    if (m_is_enemy_off) {
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (m_game_state.enemies[i].get_entity_type() == ENEMY ||
                m_game_state.enemies[i].get_entity_type() == TRAP)
                m_game_state.enemies[i].deactivate();
        }
    }
    else {
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (i == 0 && m_game_state.pressure_plate->get_obstacle_state() != PRESSED) continue;
            m_game_state.enemies[i].activate();
        }
    }

    // ----- Player Camera ----- //
    *m_view_matrix = glm::mat4(1.0f);

    if (m_game_state.player->get_position().y < m_level_top_edge &&
        m_game_state.player->get_position().y > m_level_bottom_edge) {
        *m_view_matrix = glm::translate(*m_view_matrix,
            glm::vec3(-9.5, -m_game_state.player->get_position().y, 0));
    }
    else {
        if (m_game_state.player->get_position().y >= m_level_top_edge)
            *m_view_matrix = glm::translate(*m_view_matrix, glm::vec3(-9.5, 6.98, 0));
        else if (m_game_state.player->get_position().y <= m_level_bottom_edge)
            *m_view_matrix = glm::translate(*m_view_matrix, glm::vec3(-9.5, 10.0, 0));
    }

    // ----- Switch Scene ----- //
    if (m_game_state.clone->get_position().y >= -0.5f || m_scene_skip) m_game_state.next_scene_id = 6;

    // ----- Player Lives ----- //
    for (int i = 0; i < ENEMY_COUNT; i++) 
    {
        if (m_game_state.clone->check_collision(&m_game_state.enemies[i])
            && m_game_state.enemies[i].get_is_active()) 
        {
            if (not(m_game_state.enemies[i].get_entity_type() == TRAP &&
                m_game_state.enemies[i].get_trap_type() == SPIKE &&
                m_game_state.enemies[i].get_trap_state() != ATTACK) &&
                m_game_state.enemies[i].get_entity_type() != OBSTACLE &&
                m_game_state.enemies[i].get_entity_type() != CHARACTER)
                player_death();
            break;
        }

        if (m_game_state.player->check_collision(&m_game_state.enemies[i])
            && m_game_state.enemies[i].get_is_active()) 
        {
            if (not(m_game_state.enemies[i].get_entity_type() == TRAP &&
                m_game_state.enemies[i].get_trap_type() == SPIKE &&
                m_game_state.enemies[i].get_trap_state() != ATTACK) &&
                m_game_state.enemies[i].get_entity_type() != OBSTACLE &&
                m_game_state.enemies[i].get_entity_type() != CHARACTER)
                player_death();
            break;
        }
    }
}

void LevelC::render(ShaderProgram* g_shader_program, ShaderProgram* g_clone_shader_program, ShaderProgram* g_map_shader_program)
{
    // ----- Shader Programs ----- //
    g_shader_program->set_projection_matrix(*m_projection_matrix);
    g_shader_program->set_view_matrix(*m_view_matrix);
    g_clone_shader_program->set_projection_matrix(*m_projection_matrix);
    g_clone_shader_program->set_view_matrix(*m_view_matrix);
    g_map_shader_program->set_projection_matrix(*m_projection_matrix);
    g_map_shader_program->set_view_matrix(*m_view_matrix);

    m_game_state.map->render(g_map_shader_program);
    m_game_state.pressure_plate->render(g_clone_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].render(g_clone_shader_program);
    }
    m_game_state.player->render(g_shader_program);
    m_game_state.clone->render(g_clone_shader_program);
}

void LevelC::player_death() {
    set_player_lives(get_player_lives() - 1);  // Lose 1 life

    if (get_player_lives() <= 0) {
        m_game_state.next_scene_id = 6;  // Game Over Scene
    }
    else {
        initialise();  // Reset Level
    }
}

void LevelC::check_plate_pressed()
{
    if (m_game_state.pressure_plate->get_obstacle_state() == PRESSED) {
        m_game_state.enemies[0].activate();

        LEVEL_DATA[69] = 198;
        LEVEL_DATA[88] = 10;
        LEVEL_DATA[89] = 168;
        LEVEL_DATA[109] = 196;

        if (!is_door_open) {
            Mix_PlayChannel(-1, m_game_state.door_mechanic_sfx, 0);
            is_door_open = true;
        }
    }
    else {
        m_game_state.enemies[0].deactivate();

        LEVEL_DATA[69] = 185;
        LEVEL_DATA[88] = 11;
        LEVEL_DATA[89] = 140;
        LEVEL_DATA[109] = 183;

        if (is_door_open) is_door_open = false;
    }

    delete m_game_state.map;
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, m_map_texture_id, 1.0f, 14, 15);
}