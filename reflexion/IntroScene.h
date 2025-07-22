#pragma once
#include "Scene.h"

class IntroScene : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_font_texture_id;

    // ————— DESTRUCTOR ————— //
    ~IntroScene();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program, ShaderProgram* clone_program, ShaderProgram* map_program) override;
};
