#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 19
#define LEVEL_HEIGHT 17

static unsigned int LEVEL_DATA[] =
{
    23, 45, 45, 45, 45, 45, 10, 45, 10, 45, 45, 56, 57, 58, 59, 56, 45, 45, 24,
    44,  3,  3,  3,  3,  5, 50,  5, 50,  3,  3,  3,  3,  3,  3,  3,  3,  5, 43,
    44,  6,  6,  6,  6,  2, 35,  2, 50,  6,  6,  6,  6,  6,  6,  6,  6,  2, 43,
     8, 49, 37,  6,  6,  4,  1,  2, 50,  6,  6,  6,  6,  6,  6,  6,  6,  2, 43,
    44,  3,  1,  6,  6,  6,  6,  2, 50,  6,  6,  6,  6,  6,  6,  6,  6,  2, 43,
    44,  0, 38,  6,  6,  6,  6,  2, 50,  6,  0, 30, 49, 49, 49, 17, 37,  2, 43,
    44,  2, 28, 31,  6,  6,  6,  2, 50,  0, 36, 29,  3,  3,  5, 50,  1,  2, 43,
    44,  4,  5, 28, 37,  0, 36, 49, 16,  4,  3,  1,  6,  6,  2, 15, 49, 49,  9,
    44,  6,  4,  3,  1,  4,  3,  5, 50,  6,  6,  6,  6,  6,  2, 35,  3,  5, 43,
    44,  0, 38,  6,  6,  0, 38,  2, 15, 49, 17, 49, 37,  6,  4,  1,  6,  2, 43,
    44,  2, 50,  6,  6,  2, 35,  2, 50,  5, 50,  3,  1,  6,  6,  6,  6,  2, 43,
    44,  2, 50,  6,  6,  4,  1,  2, 50,  2, 28, 49, 37,  0, 36, 49, 49, 49,  9,
    44,  2, 35,  0, 36, 49, 31,  2, 50,  4,  3,  3,  1,  4,  3,  3,  3,  5, 43,
    44,  4,  1,  4,  3,  5, 50,  2, 50,  6,  6,  6,  6,  6,  6,  6,  6,  2, 43,
    44,  6,  6,  6,  6,  2, 50,  2, 50,  6,  6,  6,  6,  6,  6,  6,  6,  2, 43,
    44,  6,  6,  6,  6,  2, 50,  2, 50,  6,  6,  6,  6,  6,  6,  6,  6,  2, 43,
    21, 42, 42, 42, 42, 42,  7, 42,  7, 42, 42, 42, 42, 42, 42, 42, 42, 42, 22,
};

LevelB::~LevelB()
{
    delete[] m_game_state.enemies;
    delete[] m_game_state.blocks;
    delete   m_game_state.player;
    delete   m_game_state.clone;
    delete   m_game_state.map;
    Mix_FreeChunk(m_game_state.walk_sfx);
    Mix_FreeChunk(m_game_state.block_slide_sfx);
}

void LevelB::initialise()
{
    // ----- Camera ----- //
    float mult = 1.9f;
    *m_projection_matrix = glm::ortho(-5.0f * mult, 5.0f * mult, -3.75f * mult, 3.75f * mult, -1.0f * mult, 1.0f * mult);

    m_level_top_edge = -6.7f;
    m_level_bottom_edge = -9.25f;

    // ----- Game Assets ----- //
    m_scene_type = LEVEL;
    m_game_state.next_scene_id = -1;
    m_scene_skip = false;
    m_is_enemy_off = false;

    m_font_texture_id = Utility::load_texture("assets/font.png");

    GLuint map_texture_id = Utility::load_texture("assets/map_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 7, 9);

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
        0.85f,                     // width
        0.9f,                      // height
        CHARACTER,                 // entity type
        PLAYER,                    // character type
        REST,                      // character state
        UP                         // direction
    );

    m_game_state.player->set_position(glm::vec3(5.0f, -15.1f, 0.0f));

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
        0.85f,                     // width
        0.9f,                      // height
        CHARACTER,                 // entity type
        CLONE,                     // character type
        REST,                      // character state
        UP                         // direction
    );

    m_game_state.clone->set_position(glm::vec3(13.0f, -15.1f, 0.0f));

    // ----- Enemies Initialization ----- //
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

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT - 3; i++)
    {
        m_game_state.enemies[i] = Entity(
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
    }

    for (int i = 0; i < ENEMY_COUNT - 3; i++) {
        if (i % 2 == 0) m_game_state.enemies[i].set_position(glm::vec3(9.0f + i, -2.0f, 0.0f));
        else m_game_state.enemies[i].set_position(glm::vec3(9.0f + i, -3.0f, 0.0f));
        m_game_state.enemies[i].set_scale(glm::vec3(0.8f));
    }

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

    m_game_state.enemies[ENEMY_COUNT - 3] = Entity(
        projectile_texture_ids, // texture ids
        2.5f,                   // speed
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
    m_game_state.enemies[ENEMY_COUNT - 2] = Entity(
        projectile_texture_ids, // texture ids
        4.0f,                   // speed
        glm::vec3(0.0f),        // acceleration
        projectile_animations,  // animation index sets
        0.0f,                   // animation time
        1,                      // animation frame amount
        0,                      // current animation index
        1,                      // animation column amount
        1,                      // animation row amount
        0.15f,                  // width
        0.55f,                  // height
        ENEMY,                  // entity type
        PROJECTILE,             // AI type
        IDLE,                   // AI state
        DOWN                    // direction
    );

    m_game_state.enemies[ENEMY_COUNT - 3].set_position(glm::vec3(17.0f, -14.0f, 0.0f));
    m_game_state.enemies[ENEMY_COUNT - 2].set_position(glm::vec3(9.0f, -1.0f, 0.0f));

    // ----- BLOCK ----- //
    m_game_state.enemies[ENEMY_COUNT - 1] = Entity(
        Utility::load_texture("assets/obstacles/rock.png"),  // texture id
        3.0f,                                                   // speed
        0.7f,                                                   // width
        0.7f,                                                   // height
        OBSTACLE,                                               // entity type
        BLOCK,                                                  // obstacle type
        NOTHING                                                 // obstacle state
    );
    m_game_state.enemies[ENEMY_COUNT - 1].set_position(glm::vec3(15.0f, -12.0f, 0.0f));
    m_game_state.enemies[ENEMY_COUNT - 1].set_scale(glm::vec3(0.9f));

    m_game_state.blocks = new Entity*[BLOCK_COUNT];
    m_game_state.blocks[0] = &m_game_state.enemies[ENEMY_COUNT - 1];

    // ----- BGM and SFX ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.walk_sfx = Mix_LoadWAV("assets/sound/walk.wav");
    m_game_state.block_slide_sfx = Mix_LoadWAV("assets/sound/block_slide.wav");
}

void LevelB::update(float delta_time)
{
    // ----- Enemy Activation ----- //
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

    // Reset Block Movement + Play Sound Effect
    bool is_pushing = false;
    if (m_game_state.blocks[0]->get_movement() != glm::vec3(0.0f)) is_pushing = true;
    m_game_state.blocks[0]->set_movement(glm::vec3(0.0f));
    if (is_pushing && !Mix_Playing(3)) Mix_PlayChannel(3, m_game_state.block_slide_sfx, -1);
    else Mix_HaltChannel(3);

    // ----- Update Entities ----- //
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.clone, nullptr, 0, m_game_state.map);
    m_game_state.clone->update(delta_time, m_game_state.player, m_game_state.clone, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.clone, *m_game_state.blocks, BLOCK_COUNT, m_game_state.map);
    }
    for (int i = 0; i < BLOCK_COUNT; i++) {
        m_game_state.clone->resolve_overlap(m_game_state.blocks[i]);
	}

    // ----- Player Camera ----- //
    *m_view_matrix = glm::mat4(1.0f);

    if (m_game_state.player->get_position().y < m_level_top_edge &&
        m_game_state.player->get_position().y > m_level_bottom_edge) {
        *m_view_matrix = glm::translate(*m_view_matrix,
            glm::vec3(-9, -m_game_state.player->get_position().y, 0));
    }
    else {
        if (m_game_state.player->get_position().y >= m_level_top_edge)
            *m_view_matrix = glm::translate(*m_view_matrix, glm::vec3(-9, 6.65, 0));
        else if (m_game_state.player->get_position().y <= m_level_bottom_edge)
            *m_view_matrix = glm::translate(*m_view_matrix, glm::vec3(-9, 9.35, 0));
    }

    // ----- Switch Scene ----- //
    if (m_game_state.clone->get_position().y >= -0.5f || m_scene_skip) m_game_state.next_scene_id = 5;

    // ----- Player Lives ----- //
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.clone->check_collision(&m_game_state.enemies[i])
            && m_game_state.enemies[i].get_is_active()) {
            if (not(m_game_state.enemies[i].get_entity_type() == TRAP && 
                m_game_state.enemies[i].get_trap_type() == SPIKE && 
                m_game_state.enemies[i].get_trap_state() != ATTACK) &&
                m_game_state.enemies[i].get_entity_type() != OBSTACLE)
                player_death();
            break;
        }
    }
}

void LevelB::render(ShaderProgram* g_shader_program, ShaderProgram* g_clone_shader_program, ShaderProgram* g_map_shader_program)
{
    // ----- Shader Programs ----- //
    g_shader_program->set_projection_matrix(*m_projection_matrix);
    g_shader_program->set_view_matrix(*m_view_matrix);
    g_clone_shader_program->set_projection_matrix(*m_projection_matrix);
    g_clone_shader_program->set_view_matrix(*m_view_matrix);
    g_map_shader_program->set_projection_matrix(*m_projection_matrix);
    g_map_shader_program->set_view_matrix(*m_view_matrix);

    m_game_state.map->render(g_map_shader_program);
    for (int i = 0; i < ENEMY_COUNT - 2; i++) {
        m_game_state.enemies[i].render(g_clone_shader_program);
    }

    m_game_state.player->render(g_shader_program);
    m_game_state.clone->render(g_clone_shader_program);

    m_game_state.enemies[ENEMY_COUNT - 3].render(g_clone_shader_program);
    m_game_state.enemies[ENEMY_COUNT - 2].render(g_clone_shader_program);
    m_game_state.enemies[ENEMY_COUNT - 1].render(g_clone_shader_program);
}

void LevelB::player_death() {
    set_player_lives(get_player_lives() - 1);  // Lose 1 life

    if (get_player_lives() <= 0) {
        m_game_state.next_scene_id = 6;  // Game Over Scene
    }
    else {
        initialise();  // Reset Level
    }
}