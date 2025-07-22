#include "Scene.h"

class GameStart : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_font_texture_id;

    // ————— DESTRUCTOR ————— //
    ~GameStart();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program, ShaderProgram* text_program, ShaderProgram* map_program) override;
};