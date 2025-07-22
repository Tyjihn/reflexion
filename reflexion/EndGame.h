#pragma once
#include "Scene.h"

class EndGame : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_font_texture_id;
    bool m_has_played_sound = false;

    // ————— DESTRUCTOR ————— //
    ~EndGame();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program, ShaderProgram* text_program, ShaderProgram* map_program) override;
};