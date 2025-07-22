#pragma once
#include "Scene.h"

class LevelC : public Scene {
private:
    bool m_plate_pressed = false;

public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_font_texture_id;
    GLuint m_map_texture_id;
    int ENEMY_COUNT = 6;
    int BLOCK_COUNT = 2;
    int PRESSURE_PLATE_COUNT = 1;

    // ————— DESTRUCTOR ————— //
    ~LevelC();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program, ShaderProgram* clone_program, ShaderProgram* map_program) override;
    void player_death();
    void check_plate_pressed();
};