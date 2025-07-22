/**
* Author: Lucy Zheng
* Assignment: Reflexion
* Date due: 05/02/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

// ------------ DEBUG -----------
#include <windows.h>
// --------------

void Entity::ai_activate(Entity* player)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_walk();
            break;

        case PROJECTILE:
            ai_projectile();
            break;

        default:
            set_ai_state(IDLE);
            break;
    }
}

void Entity::trap_activate(Entity* player, float delta_time)
{
    switch (m_trap_type)
    {
        case SPIKE:
            trap_spike(delta_time);
            break;
        
        default:
            break;
    }
}

void Entity::obstacle_activate()
{
    switch (m_obstacle_type) {
        case BLOCK:
            obstacle_block();
            break;

        case PRESSURE_PLATE:
            break;

        default:
            break;
    }
}

void Entity::ai_walk()
{
    set_ai_state(MOVING);

    if (m_collided_left) set_direction(RIGHT);
    else if (m_collided_right) set_direction(LEFT);
    else if (m_collided_top) set_direction(DOWN);
    else if (m_collided_bottom) set_direction(UP);

    if (m_direction == LEFT)
    {
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        face_left();
    }
    else if (m_direction == RIGHT)
    {
        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        face_right();
    }
    else if (m_direction == UP)
    {
        m_movement = glm::vec3(0.0f, 1.0f, 0.0f);
        face_up();
    }
    else if (m_direction == DOWN)
    {
        m_movement = glm::vec3(0.0f, -1.0f, 0.0f);
        face_down();
    }
}

void Entity::ai_projectile()
{
    set_ai_state(MOVING);
    if (m_direction == LEFT)
    {
        if (m_collided_left) m_position = m_initial_position;
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        face_left();
    }
    else if (m_direction == RIGHT)
    {
        if (m_collided_right) m_position = m_initial_position;
        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        face_right();
    }
    else if (m_direction == DOWN)
    {
        if (m_collided_bottom) m_position = m_initial_position;
        m_movement = glm::vec3(0.0f, -1.0f, 0.0f);
        face_down();
    }
    else if (m_direction == UP)
    {
        if (m_collided_top) m_position = m_initial_position;
        m_movement = glm::vec3(0.0f, 1.0f, 0.0f);
        face_up();
    }
}

void Entity::trap_spike(float delta_time)
{
    m_spike_timer += delta_time;

    const float OFF_TIME = 0.6f;
    const float READY_TIME = 2.25f;
    const float ATTACK_FRAME_TIME = 0.1f;

    switch (m_trap_state) {
        case OFF:
            if (m_spike_timer >= OFF_TIME) {
                m_spike_timer = 0.0f;
                set_trap_state(READY);
            }
            break;

        case READY:
            if (m_spike_timer >= READY_TIME) {
                m_spike_timer = 0.0f;
                set_trap_state(ATTACK);
                m_animation_index = 0;
            }
            break;

        case ATTACK:
            if (m_spike_timer >= ATTACK_FRAME_TIME) {
                m_spike_timer = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames) {
                    m_animation_index = 0;
                    set_trap_state(OFF);
                }
            }
            break;

        default:
            break;
    }
}

void Entity::obstacle_block()
{
    if (m_obstacle_direction == DOWN) move_down(false);
    else if (m_obstacle_direction == UP) move_up(false);
    else if (m_obstacle_direction == LEFT) move_left(false);
    else if (m_obstacle_direction == RIGHT) move_right(false);
}

// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f),
    m_character_type(NOT_CHARACTER), m_character_state(NO_CHARACTER_STATE),
    m_ai_type(NOT_AI), m_ai_state(NO_AI_STATE), m_trap_type(NOT_TRAP), m_trap_state(NO_TRAP_STATE),
    m_obstacle_type(NOT_OBSTACLE), m_obstacle_state(NO_OBSTACLE_STATE), m_direction(NO_DIRECTION)
{
}

// Character constructor
Entity::Entity(std::vector<std::vector<GLuint>> texture_ids, float speed, glm::vec3 acceleration, 
    std::vector<std::vector<std::vector<int>>> animations, float animation_time, int animation_frames, 
    int animation_index, int animation_cols, int animation_rows, float width, float height, 
    EntityType EntityType, CharacterType CharacterType, CharacterState CharacterState, Direction Direction)
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_animations(animations), m_animation_cols(animation_cols), 
    m_animation_frames(animation_frames), m_animation_index(animation_index), m_animation_rows(animation_rows), 
    m_animation_indices(nullptr), m_animation_time(animation_time), m_texture_ids(texture_ids), 
    m_velocity(0.0f), m_width(width), m_height(height), m_entity_type(EntityType), 
    m_character_type(CharacterType),m_character_state(CharacterState), m_direction(Direction)
{
    set_character_state(CharacterState);
    move_entity(Direction);
}

// Simple constructor (Background)
Entity::Entity(GLuint texture_id, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f), m_texture_id(texture_id), 
    m_velocity(0.0f), m_acceleration(0.0f), m_width(1.0f), m_height(1.0f), m_entity_type(EntityType)
{
}

// Enemy constructor
Entity::Entity(std::vector<std::vector<GLuint>> texture_ids, float speed, glm::vec3 acceleration, 
    std::vector<std::vector<std::vector<int>>> animations, float animation_time, int animation_frames, 
    int animation_index, int animation_cols, int animation_rows, float width, float height, 
    EntityType EntityType, AIType AIType, AIState AIState, Direction Direction)
    : m_position(glm::vec3(0.0f)), m_movement(glm::vec3(0.0f)), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f), 
    m_acceleration(acceleration), m_speed(speed), m_animations(animations), m_animation_cols(animation_cols), 
    m_animation_frames(animation_frames), m_animation_index(animation_index), m_animation_rows(animation_rows), 
    m_animation_indices(nullptr), m_animation_time(animation_time), m_texture_ids(texture_ids), m_velocity(glm::vec3(0.0f)),
    m_width(width), m_height(height), m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState), m_direction(Direction)
{
    set_ai_state(AIState);
    move_entity(Direction);
}

// Trap constructor
Entity::Entity(std::vector<GLuint> texture_ids, std::vector<std::vector<int>> animations, float animation_time, 
    int animation_frames, int animation_index, int animation_cols, int animation_rows, float width, float height, 
    EntityType EntityType, TrapType TrapType, TrapState TrapState)
    : m_position(glm::vec3(0.0f)), m_movement(glm::vec3(0.0f)), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f), 
    m_obstacle_texture_ids(texture_ids), m_speed(0.0f), m_acceleration(0.0f), m_obstacle_animations(animations), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_animation_frames(animation_frames), m_animation_index(animation_index), 
    m_animation_cols(animation_cols), m_animation_rows(animation_rows), m_width(width), m_height(height), 
    m_entity_type(EntityType), m_trap_type(TrapType), m_trap_state(TrapState)
{
    set_trap_state(TrapState);
}

// Block constructor
Entity::Entity(GLuint texture_id, float speed, float width, float height, 
    EntityType EntityType, ObstacleType ObstacleType, ObstacleState ObstacleState)
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f), m_texture_id(texture_id),
    m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType), 
    m_obstacle_type(ObstacleType), m_obstacle_state(ObstacleState)
{
}

// Pressure Plate Constructor
Entity::Entity(std::vector<GLuint> texture_id, float speed, float width, float height, EntityType EntityType,
    ObstacleType ObstacleType, ObstacleState ObstacleState)
    : m_position(0.0f), m_movement(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 0.0f)), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f), m_obstacle_texture_ids(texture_id),
    m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType),
    m_obstacle_type(ObstacleType), m_obstacle_state(ObstacleState)
{
}

Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    if (m_entity_type == CHARACTER && other->m_entity_type == TRAP && other->m_trap_type == SPIKE && other->m_trap_state != ATTACK) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity *collidable_entities, int collidable_entity_count, Map *map)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->get_is_active() || this == collidable_entity) continue;

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                // Block Movement
                if (collidable_entity->get_obstacle_type() == BLOCK && m_entity_type == CHARACTER) {
                    collidable_entity->m_obstacle_direction = UP;
                    collidable_entity->m_activate_obstacle = true;
                    if (!(collidable_entity->will_collide(glm::vec3(0.0f, 1.0f, 0.0f), map))) {
                        m_collided_top = false;
                    }
                    else {
                        m_position.y -= y_overlap;
                        m_velocity.y = 0;
                        m_collided_top = true;
                    }
                }
                else {
                    m_position.y -= y_overlap;
                    m_velocity.y = 0;
                    m_collided_top = true;
                }
            }
            else if (m_velocity.y < 0)
            {
                // Block Movement
                if (collidable_entity->get_obstacle_type() == BLOCK && m_entity_type == CHARACTER) {
                    collidable_entity->m_obstacle_direction = DOWN;
                    collidable_entity->m_activate_obstacle = true;
                    if (!(collidable_entity->will_collide(glm::vec3(0.0f, -1.0f, 0.0f), map))) {
                        m_collided_bottom = false;
                    }
                    else {
                        m_position.y += y_overlap;
                        m_velocity.y = 0;
                        m_collided_bottom = true;
                    }
                }
                else {
                    m_position.y += y_overlap;
                    m_velocity.y = 0;
                    m_collided_bottom = true;
                }
            }
        }
    }
}

void const Entity::check_collision_x(Entity *collidable_entities, int collidable_entity_count, Map *map)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->get_is_active() || this == collidable_entity) continue;

        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                // Block Movement
                if (collidable_entity->get_obstacle_type() == BLOCK && m_entity_type == CHARACTER) {
                    collidable_entity->m_obstacle_direction = RIGHT;
                    collidable_entity->m_activate_obstacle = true;
                    if (!(collidable_entity->will_collide(glm::vec3(1.0f, 0.0f, 0.0f), map))) {
                        m_collided_right = false;
                    }
                    else {
                        m_position.x -= x_overlap;
                        m_velocity.x = 0;
                        m_collided_right = true;
                    }
                }
                else {
                    m_position.x -= x_overlap;
                    m_velocity.x = 0;
                    m_collided_right = true;
                }
            }
            else if (m_velocity.x < 0)
            {
                // Block Movement
                if (collidable_entity->get_obstacle_type() == BLOCK && m_entity_type == CHARACTER) {
                    collidable_entity->m_obstacle_direction = LEFT;
                    collidable_entity->m_activate_obstacle = true;
                    if (!(collidable_entity->will_collide(glm::vec3(-1.0f, 0.0f, 0.0f), map))) {
                        m_collided_left = false;
                    }
                    else {
                        m_position.x += x_overlap;
                        m_velocity.x = 0;
                        m_collided_left = true;
                    }
                }
                else {
                    m_position.x += x_overlap;
                    m_velocity.x = 0;
                    m_collided_left = true;
                }
            }
        }
    }
}

void const Entity::check_collision_y(Map* map)
{
    float y_offset = (m_width / 2) * 0.85f;

    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - y_offset, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + y_offset, m_position.y + (m_height / 2), m_position.z);

    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - y_offset, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + y_offset, m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
}

void const Entity::check_collision_x(Map* map)
{
    float x_offset = (m_width / 2) * 0.85f;

    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 left_top = glm::vec3(m_position.x - (m_width / 2), m_position.y + x_offset, m_position.z);
    glm::vec3 left_bottom = glm::vec3(m_position.x - (m_width / 2), m_position.y - x_offset, m_position.z);

    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right_top = glm::vec3(m_position.x + (m_width / 2), m_position.y + x_offset, m_position.z);
    glm::vec3 right_bottom = glm::vec3(m_position.x + (m_width / 2), m_position.y - x_offset, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    else if (map->is_solid(left_top, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    else if (map->is_solid(left_bottom, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }

    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
    else if (map->is_solid(right_top, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
    else if (map->is_solid(right_bottom, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

bool const Entity::will_collide(glm::vec3 movement, Map* map) const {
    glm::vec3 new_position = m_position + movement;

    float x_offset = (m_width / 2) * 0.85f;
    float y_offset = (m_width / 2) * 0.85f;

    glm::vec3 left = glm::vec3(new_position.x - (m_width / 2), new_position.y, new_position.z);
    glm::vec3 left_top = glm::vec3(new_position.x - (m_width / 2), new_position.y + x_offset, new_position.z);
    glm::vec3 left_bottom = glm::vec3(new_position.x - (m_width / 2), new_position.y - x_offset, new_position.z);

    glm::vec3 right = glm::vec3(new_position.x + (m_width / 2), new_position.y, new_position.z);
    glm::vec3 right_top = glm::vec3(new_position.x + (m_width / 2), new_position.y + x_offset, new_position.z);
    glm::vec3 right_bottom = glm::vec3(new_position.x + (m_width / 2), new_position.y - x_offset, new_position.z);

    glm::vec3 top = glm::vec3(new_position.x, new_position.y + (m_height / 2), new_position.z);
    glm::vec3 top_left = glm::vec3(new_position.x - y_offset, new_position.y + (m_height / 2), new_position.z);
    glm::vec3 top_right = glm::vec3(new_position.x + y_offset, new_position.y + (m_height / 2), new_position.z);

    glm::vec3 bottom = glm::vec3(new_position.x, new_position.y - (m_height / 2), new_position.z);
    glm::vec3 bottom_left = glm::vec3(new_position.x - y_offset, new_position.y - (m_height / 2), new_position.z);
    glm::vec3 bottom_right = glm::vec3(new_position.x + y_offset, new_position.y - (m_height / 2), new_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (movement.x < 0) {
        if (map->is_solid(left, &penetration_x, &penetration_y) ||
            map->is_solid(left_top, &penetration_x, &penetration_y) ||
            map->is_solid(left_bottom, &penetration_x, &penetration_y)) {
            return true;
        }
    }
    else if (movement.x > 0) {
        if (map->is_solid(right, &penetration_x, &penetration_y) ||
            map->is_solid(right_top, &penetration_x, &penetration_y) ||
            map->is_solid(right_bottom, &penetration_x, &penetration_y)) {
            return true;
        }
    }

    if (movement.y < 0) {
        if (map->is_solid(bottom, &penetration_x, &penetration_y) ||
            map->is_solid(bottom_left, &penetration_x, &penetration_y) ||
            map->is_solid(bottom_right, &penetration_x, &penetration_y)) {
            return true;
        }
    }
    else if (movement.y > 0) {
        if (map->is_solid(top, &penetration_x, &penetration_y) ||
            map->is_solid(top_left, &penetration_x, &penetration_y) ||
            map->is_solid(top_right, &penetration_x, &penetration_y)) {
            return true;
        }
    }
    return false;
}

void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map)
{
    if (!m_is_active) return;

    if (m_entity_type == BACKGROUND || m_entity_type == HEART)
    {
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
        m_model_matrix = glm::scale(m_model_matrix, m_scale);
        return;
    }
    else if (m_entity_type == ENEMY) {
        ai_activate(player);
    }
    else if (m_entity_type == TRAP) {
        trap_activate(player, delta_time);
    }
    else if (m_obstacle_type == BLOCK && m_activate_obstacle) {
        obstacle_activate();
        m_activate_obstacle = false;
    }
    else if (m_obstacle_type == PRESSURE_PLATE) {
        bool is_pressed = false;
        if (check_collision(player)) is_pressed = true;
        for (int i = 0; i < collidable_entity_count; i++) {
            if (check_collision(&collidable_entities[i])) is_pressed = true;
        }
        if (is_pressed) set_obstacle_state(PRESSED);
        else set_obstacle_state(NOTHING);

        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
        m_model_matrix = glm::scale(m_model_matrix, m_scale);
        return;
    }

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_animation_indices != NULL)
    {
        if (m_animation_frames > 1)
        {
            if (not(m_entity_type == TRAP && m_trap_state == ATTACK))
            {
                m_animation_time += delta_time;
                float frames_per_second = (float)1 / SECONDS_PER_FRAME;

                if (m_animation_time >= frames_per_second)
                {
                    m_animation_time = 0.0f;
                    m_animation_index++;

                    if (m_animation_index >= m_animation_frames)
                    {
                        m_animation_index = 0;
                    }
                }
            }
        }
    }

    m_velocity.y = m_movement.y * m_speed;
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;

    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count, map);
    check_collision_y(map);

    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count, map);
    check_collision_x(map);

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) return;

    program->set_model_matrix(m_model_matrix);

    GLuint current_texture = m_texture_id;

    if (m_entity_type == CHARACTER)
        current_texture = m_texture_ids[m_character_state][m_direction];
    else if (m_entity_type == ENEMY)
        current_texture = m_texture_ids[m_ai_state][m_direction];
    else if (m_entity_type == TRAP)
        current_texture = m_obstacle_texture_ids[m_trap_state];
    else if (m_obstacle_type == PRESSURE_PLATE)
        current_texture = m_obstacle_texture_ids[m_obstacle_state];

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, current_texture, 
            m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, current_texture);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}