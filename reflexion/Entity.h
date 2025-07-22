#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
enum EntityType     { CHARACTER, ENEMY, BACKGROUND, TRAP, HEART, OBSTACLE };
enum CharacterType  { PLAYER, CLONE, NOT_CHARACTER                        };
enum CharacterState { REST, WALK, PUSH, NO_CHARACTER_STATE                };
enum AIType         { WALKER, PROJECTILE, NOT_AI                          };
enum AIState        { IDLE, MOVING, ATTACKING, NO_AI_STATE                };
enum TrapType       { SPIKE, NOT_TRAP                                     };
enum TrapState      { OFF, READY, ATTACK, NO_TRAP_STATE                   };
enum ObstacleType   { BLOCK, PRESSURE_PLATE, NOT_OBSTACLE                 };
enum ObstacleState  { NOTHING, PRESSED, NO_OBSTACLE_STATE                 };

enum Direction { LEFT, RIGHT, UP, DOWN, NO_DIRECTION };


class Entity
{
private:
    bool m_is_active = true;

    std::vector<std::vector<std::vector<int>>> m_animations;
    std::vector<std::vector<int>> m_obstacle_animations;
    
    EntityType m_entity_type;
    CharacterType m_character_type;
    CharacterState m_character_state;
    AIType m_ai_type;
    AIState m_ai_state;
    TrapType m_trap_type;
    TrapState m_trap_state;
    ObstacleType m_obstacle_type;
    ObstacleState m_obstacle_state;
    Direction m_direction;

    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_initial_position;

    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    float     m_speed;

    // ————— TEXTURES ————— //
    GLuint m_texture_id;
    std::vector<std::vector<GLuint>> m_texture_ids;
    std::vector<GLuint> m_obstacle_texture_ids;

    // ————— ANIMATION ————— //
    int m_animation_cols;
    int m_animation_frames,
        m_animation_index,
        m_animation_rows;

    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;
    float m_spike_timer = 0.0f;

    float m_width = 1.0f,
          m_height = 1.0f;

    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;

    // ----- Block ----- //
    bool m_activate_obstacle = false;
    Direction m_obstacle_direction = NO_DIRECTION;

public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 12;

    // ————— METHODS ————— //
    Entity();
    Entity(std::vector<std::vector<GLuint>> texture_ids, float speed, glm::vec3 acceleration, std::vector<std::vector<std::vector<int>>> animations,
            float animation_time, int animation_frames, int animation_index, int animation_cols, int animation_rows, float width, float height,
            EntityType EntityType, CharacterType CharacterType, CharacterState CharacterState, Direction Direction); // Character constructor
    Entity(GLuint texture_id, EntityType EntityType); // Simple constructor
    Entity(std::vector<std::vector<GLuint>> texture_ids, float speed, glm::vec3 acceleration, std::vector<std::vector<std::vector<int>>> animations,
            float animation_time, int animation_frames, int animation_index, int animation_cols, int animation_rows, float width, float height,
            EntityType EntityType, AIType AIType, AIState AIState, Direction Direction); // AI constructor + animation
    Entity(std::vector<GLuint> texture_ids, std::vector<std::vector<int>> animations, float animation_time,
        int animation_frames, int animation_index, int animation_cols, int animation_rows, float width, float height,
        EntityType EntityType, TrapType TrapType, TrapState TrapState); // Trap constructor
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType,
        ObstacleType ObstacleType, ObstacleState ObstacleState); // Block constructor
    Entity(std::vector<GLuint> texture_id, float speed, float width, float height, EntityType EntityType,
        ObstacleType ObstacleType, ObstacleState ObstacleState); // Pressure Plate constructor
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count, Map *map);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count, Map *map);
    
    void const check_collision_y(Map *map);
    void const check_collision_x(Map*map);
    bool const will_collide(glm::vec3 movement, Map* map) const;

    
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map);
    void render(ShaderProgram* program);

    void ai_activate(Entity *player);
    void trap_activate(Entity* player, float delta_time);
    void obstacle_activate();
    void ai_walk();
    void ai_projectile();
    void trap_spike(float delta_time);
    void obstacle_block();
    
    void normalise_movement() { m_movement = glm::normalize(m_movement); }

    void face_left()  { set_direction(LEFT);  }
    void face_right() { set_direction(RIGHT); }
    void face_up()    { set_direction(UP);    }
    void face_down()  { set_direction(DOWN);  }

    void move_left(bool will_collide) {
        if (!will_collide) m_movement.x = -1.0f;
        if (m_entity_type != OBSTACLE) face_left();
        if (m_entity_type == CHARACTER) set_character_state(WALK);
    }

    void move_right(bool will_collide) {
        if (!will_collide) m_movement.x = 1.0f;
        if (m_entity_type != OBSTACLE) face_right();
        if (m_entity_type == CHARACTER) set_character_state(WALK);
    }

    void move_up(bool will_collide) {
        if (!will_collide) m_movement.y = 1.0f;
        if (m_entity_type != OBSTACLE) face_up();
        if (m_entity_type == CHARACTER) set_character_state(WALK);
    }

    void move_down(bool will_collide) {
        if (!will_collide) m_movement.y = -1.0f;
        if (m_entity_type != OBSTACLE) face_down();
        if (m_entity_type == CHARACTER) set_character_state(WALK);
    }

    void resting() { set_character_state(REST); }

    void move_entity(Direction direction)
    {
        if (direction == UP) move_up(false);
        else if (direction == DOWN) move_down(false);
        else if (direction == LEFT) move_left(false);
        else if (direction == RIGHT) move_right(false);
    }

    // ————— GETTERS ————— //
    EntityType          const get_entity_type()      const { return m_entity_type;    };
    AIType              const get_ai_type()          const { return m_ai_type;        };
    AIState             const get_ai_state()         const { return m_ai_state;       };
    TrapType            const get_trap_type()        const { return m_trap_type;      }
    TrapState           const get_trap_state()       const { return m_trap_state;     }
    ObstacleType        const get_obstacle_type()    const { return m_obstacle_type;  }
    ObstacleState       const get_obstacle_state()   const { return m_obstacle_state; }
    Direction           const get_direction()        const { return m_direction;      };
    CharacterState      const get_character_state()  const { return m_character_state; }
    glm::vec3           const get_position()         const { return m_position;       }
    glm::vec3           const get_velocity()         const { return m_velocity;       }
    glm::vec3           const get_acceleration()     const { return m_acceleration;   }
    glm::vec3           const get_movement()         const { return m_movement;       }
    glm::vec3           const get_scale()            const { return m_scale;          }
    GLuint              const get_texture_id()       const { return m_texture_id;     }
    float               const get_speed()            const { return m_speed;          }
    bool                const get_collided_top()     const { return m_collided_top;   }
    bool                const get_collided_bottom()  const { return m_collided_bottom;}
    bool                const get_collided_right()   const { return m_collided_right; }
    bool                const get_collided_left()    const { return m_collided_left;  }
    float               const get_width()            const { return m_width;          }
    float               const get_height()           const { return m_height;         }
    bool                const get_is_active()        const { return m_is_active;      }
    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };

    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type) { m_ai_type = new_ai_type;};
    void const set_position(glm::vec3 new_position) { m_position = new_position; m_initial_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }
    void const set_animations(std::vector<std::vector<std::vector<int>>> new_animations) { m_animations = new_animations; }
    void const set_direction(Direction new_direction) { m_direction = new_direction; };
    void const set_obstacle_state(ObstacleState new_obsteacle_state) { m_obstacle_state = new_obsteacle_state; }
    void const set_character_state(CharacterState new_character_state)
    {
        if (m_character_state == new_character_state) return;

        m_character_state = new_character_state;

        if (!m_animations[new_character_state].empty() && m_animations.size() > new_character_state)
        {
            m_animation_indices = m_animations[new_character_state][m_direction].data();
        }
        else
        {
            m_animation_indices = nullptr;
            return;
        }

        m_animation_cols = (int)m_animations[new_character_state][m_direction].size();
        m_animation_frames = (int)m_animations[new_character_state][m_direction].size();
    }

    void const set_ai_state(AIState new_ai_state)
    {
        if (m_ai_state == new_ai_state) return;

        m_ai_state = new_ai_state;

        if (!m_animations[new_ai_state].empty() && m_animations.size() > new_ai_state)
        {
            m_animation_indices = m_animations[new_ai_state][m_direction].data();
        }
        else
        {
            m_animation_indices = nullptr;
            return;
        }

        m_animation_cols = (int)m_animations[new_ai_state][m_direction].size();
        m_animation_frames = (int)m_animations[new_ai_state][m_direction].size();
    }

    void const set_trap_state(TrapState new_obstacle_state)
    {
        if (m_trap_state == new_obstacle_state) return;

        m_trap_state = new_obstacle_state;

        if (!m_obstacle_animations.empty() && m_obstacle_animations.size() > new_obstacle_state)
        {
            m_animation_indices = m_obstacle_animations[new_obstacle_state].data();
        }
        else
        {
            m_animation_indices = nullptr;
            return;
        }

        m_animation_cols = (int)m_obstacle_animations[new_obstacle_state].size();
        m_animation_frames = (int)m_obstacle_animations[new_obstacle_state].size();
    }
};

#endif // ENTITY_H
