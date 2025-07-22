#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"
#include "Effects.h"


enum SceneType { START, LEVEL, END };

struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map *map = nullptr;
    Entity *player = nullptr;
    Entity *clone = nullptr;
    Entity *enemies = nullptr;
    Entity **blocks = nullptr;
    Entity *pressure_plate = nullptr;
    Entity *background = nullptr;
    
    // ————— AUDIO ————— //
    Mix_Chunk *game_over_sfx = nullptr;
    Mix_Chunk *walk_sfx = nullptr;
    Mix_Chunk *on_pressure_plate_sfx = nullptr;
    Mix_Chunk *off_pressure_plate_sfx = nullptr;
    Mix_Chunk *block_slide_sfx = nullptr;
    Mix_Chunk* door_mechanic_sfx = nullptr;
    
    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;
};


class Scene {
protected:
    GameState m_game_state;
    SceneType m_scene_type;

    // ----- Camera ----- //
    glm::mat4 *m_projection_matrix;
    glm::mat4 *m_view_matrix;

    float m_level_top_edge,
          m_level_bottom_edge;

    // ----- PLAYER LIVES ----- //
    int ENEMY_COUNT = 0;
    int m_player_lives;
    bool m_scene_skip = false;
    bool m_is_enemy_off = false;

    bool is_door_open = false;
    
public:
    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program0, ShaderProgram *program1, ShaderProgram* program2) = 0;
    
    // ————— GETTERS ————— //
    SceneType const get_scene_type() const          { return m_scene_type;          }
    GameState const get_state() const               { return m_game_state;          }
    int       const get_player_lives() const        { return m_player_lives;        }
    bool      const get_scene_skip() const          { return m_scene_skip;          }
    float     const get_top_edge() const            { return m_level_top_edge;      }
    float     const get_bottom_edge() const         { return m_level_bottom_edge;   }
    bool      const get_enemy_off() const           { return m_is_enemy_off;        }

    // ----- SETTER ----- //
    void const set_projection_matrix(glm::mat4 *new_projection_matrix) { m_projection_matrix = new_projection_matrix; }
    void const set_view_matrix(glm::mat4 *new_view_matrix)             { m_view_matrix = new_view_matrix;             }
    void const set_player_lives(int new_player_lives)                  { m_player_lives = new_player_lives;           }
    void const set_scene_skip(bool new_scene_skip)                     { m_scene_skip = new_scene_skip;               }
    void const set_enemy_off(bool new_enemy_off)                       { m_is_enemy_off = new_enemy_off;              }
};
