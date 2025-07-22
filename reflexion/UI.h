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

class UI {
private:
	int TOTAL_LIVES;
	Entity* hearts;
	GLuint m_heart_texture_id;

public:
	UI();
	~UI();

	void initialise(int player_lives);
	void update(glm::mat4 *view_matrix, glm::mat4 *projection_matrix, int player_lives);
	void render(ShaderProgram* program);
};
