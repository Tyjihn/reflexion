#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_font_texture_id;
    int ENEMY_COUNT = 12;
    int BLOCK_COUNT = 1;

    // ————— DESTRUCTOR ————— //
    ~LevelB();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program, ShaderProgram* clone_program, ShaderProgram* map_program) override;
    void player_death();
};