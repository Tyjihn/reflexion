#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define SCENE_COUNT 7

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

// Read the live browser window size / DPI from JS.
EM_JS(int,    js_window_width,  (), { return window.innerWidth; });
EM_JS(int,    js_window_height, (), { return window.innerHeight; });
EM_JS(double, js_dpr,           (), { return window.devicePixelRatio || 1.0; });
#endif

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Effects.h"

#include "UI.h"
#include "GameStart.h"
#include "IntroScene.h"
#include "ControlsScene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "EndGame.h"

// ————— CONSTANTS ————— //
constexpr float WINDOW_MULT = 1.5f;
constexpr float WINDOW_WIDTH  = 640 * WINDOW_MULT,
                WINDOW_HEIGHT = 480 * WINDOW_MULT;

constexpr float BG_RED     = 0.0f,
                BG_BLUE    = 0.0f,
                BG_GREEN   = 0.0f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F0_SHADER_PATH[] = "shaders/fragment_textured.glsl",
               F1_SHADER_PATH[] = "shaders/clone_fragment_textured.glsl",
               F2_SHADER_PATH[] = "shaders/text_fragment_textured.glsl",
               F3_SHADER_PATH[] = "shaders/map_fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
int g_window_width = WINDOW_WIDTH;
int g_window_height = WINDOW_HEIGHT;

UI* g_ui_scene;
constexpr char HEART_FILEPATH[] = "assets/heart.png";

Scene* g_current_scene;
GameStart* g_start_scene;
IntroScene* g_intro_scene;
ControlsScene* g_controls_scene;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
EndGame* g_end_game;
Scene* g_scenes[SCENE_COUNT];

int g_player_lives = 5;

SDL_Window* g_display_window;

bool is_paused = false;
bool is_game_over = false;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program,
              g_clone_shader_program,
              g_text_shader_program,
              g_map_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;
float g_pause_ticks = 0.0f;

// ----- EFFECTS ----- //
Effects* g_effects;

// ----- MUSIC ----- //
constexpr int CD_QUAL_FREQ = 44100,
              AUDIO_CHAN_AMT = 2,
              AUDIO_BUFF_SIZE = 4096;

constexpr char BGM_FILEPATH[] = "assets/sound/bgm.ogg",
               END_BGM_FILEPATH[] = "assets/sound/end_bgm.ogg";
constexpr int LOOP_FOREVER = -1;
Mix_Music* g_bgm;
Mix_Music* g_end_bgm;

bool music_started = false;


void switch_to_scene(Scene *scene)
{
    SceneType current = g_current_scene->get_scene_type();

    if (current == END)
        g_player_lives = 5;
    else if (current != START)
        g_player_lives = g_current_scene->get_player_lives();

    g_current_scene = scene;
    g_current_scene->initialise(); 
    g_current_scene->set_player_lives(g_player_lives);

    current = g_current_scene->get_scene_type();

    if (current == LEVEL)
        g_effects->start(FADEIN, 1.0f);
    else if (current == START)
    {
        if (!music_started)
        {
            Mix_HaltMusic();
            Mix_PlayMusic(g_bgm, LOOP_FOREVER);
            music_started = true;
        }
    }
    else if (current == END)
    {
        Mix_HaltMusic();
        Mix_PlayMusic(g_end_bgm, LOOP_FOREVER);
	}
}

// Set an aspect-preserving (letterboxed) viewport inside a draw_w x draw_h
// surface. The game uses a fixed 4:3 orthographic projection, so we render it
// centred with black bars instead of stretching it to the window's shape.
void set_letterbox_viewport(int draw_w, int draw_h)
{
    if (draw_w <= 0 || draw_h <= 0) return;

    const float target_aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT; // 4:3

    int vp_w = draw_w;
    int vp_h = draw_h;
    if ((float)draw_w / (float)draw_h > target_aspect)
        vp_w = (int)(draw_h * target_aspect);   // too wide -> bars left/right
    else
        vp_h = (int)(draw_w / target_aspect);   // too tall -> bars top/bottom

    int vp_x = (draw_w - vp_w) / 2;
    int vp_y = (draw_h - vp_h) / 2;

    glViewport(vp_x, vp_y, vp_w, vp_h);
}

void window_resize(int w, int h)
{
    if (w <= 0 || h <= 0) return;

    // Use the real drawable (pixel) size, not the logical window size.
    int draw_w = w, draw_h = h;
    SDL_GL_GetDrawableSize(g_display_window, &draw_w, &draw_h);
    if (draw_w <= 0 || draw_h <= 0) { draw_w = w; draw_h = h; }

    g_window_width = draw_w;
    g_window_height = draw_h;

    set_letterbox_viewport(draw_w, draw_h);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_clone_shader_program.set_projection_matrix(g_projection_matrix);
    g_text_shader_program.set_projection_matrix(g_projection_matrix);
    g_map_shader_program.set_projection_matrix(g_projection_matrix);
}

#ifdef __EMSCRIPTEN__
// Make the canvas back-buffer match the browser window (times the DPI for
// crispness), then letterbox the 4:3 game inside it. Driven from C++ so it
// keeps working no matter how index.html is regenerated.
void fit_canvas_to_window()
{
    int win_w = js_window_width();
    int win_h = js_window_height();
    if (win_w <= 0 || win_h <= 0) return;

    double dpr = js_dpr();
    int buf_w = (int)(win_w * dpr);
    int buf_h = (int)(win_h * dpr);

    emscripten_set_canvas_element_size("#canvas", buf_w, buf_h);

    g_window_width  = buf_w;
    g_window_height = buf_h;
    set_letterbox_viewport(buf_w, buf_h);
}

EM_BOOL on_web_resize(int /*event_type*/, const EmscriptenUiEvent* /*e*/, void* /*user*/)
{
    fit_canvas_to_window();
    return EM_TRUE;
}
#endif

void initialise();
void process_input();
void player_movement();
void update();
void render();
void shutdown();

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Start Audio
    Mix_OpenAudio(
        CD_QUAL_FREQ,       // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT, // audio format
        AUDIO_CHAN_AMT,     // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE     // audio buffer size in sample FRAMES (total samples divided by channel count)
    );

    // ----- MUSIC INITIALIZATION ----- //
    g_bgm = Mix_LoadMUS(BGM_FILEPATH);
    g_end_bgm = Mix_LoadMUS(END_BGM_FILEPATH);

    // ----- WINDOW ----- //
    g_display_window = SDL_CreateWindow("Reflexion",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    int w, h;
    SDL_GetWindowSize(g_display_window, &w, &h);
    window_resize(w, h);
    
    g_shader_program.load(V_SHADER_PATH, F0_SHADER_PATH);
    g_clone_shader_program.load(V_SHADER_PATH, F1_SHADER_PATH);
    g_text_shader_program.load(V_SHADER_PATH, F2_SHADER_PATH);
    g_map_shader_program.load(V_SHADER_PATH, F3_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    // ----- SHDAER PROGRAM SETUP ----- //
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(g_shader_program.get_program_id());

    g_clone_shader_program.set_projection_matrix(g_projection_matrix);
    g_clone_shader_program.set_view_matrix(g_view_matrix);

    g_text_shader_program.set_projection_matrix(g_projection_matrix);
    g_text_shader_program.set_view_matrix(g_view_matrix);

    g_map_shader_program.set_projection_matrix(g_projection_matrix);
    g_map_shader_program.set_view_matrix(g_view_matrix);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ----- SCENES SETUP ----- //
    g_start_scene = new GameStart();
    g_intro_scene = new IntroScene();
    g_controls_scene = new ControlsScene();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    g_end_game = new EndGame();

    g_scenes[0] = g_start_scene;
    g_scenes[1] = g_intro_scene;
    g_scenes[2] = g_controls_scene;
    g_scenes[3] = g_level_a;
    g_scenes[4] = g_level_b;
    g_scenes[5] = g_level_c;
    g_scenes[6] = g_end_game;

    for (int i = 0; i < SCENE_COUNT; ++i) {
        g_scenes[i]->set_projection_matrix(&g_projection_matrix);
        g_scenes[i]->set_view_matrix(&g_view_matrix);
    }

    // ----- EFFECTS ----- //
    g_effects = new Effects(g_projection_matrix, g_view_matrix);

    switch_to_scene(g_scenes[0]);

    // ----- Player Lives ----- //
    GLuint g_heart_texture_id = Utility::load_texture(HEART_FILEPATH);
    g_current_scene->set_player_lives(g_player_lives);

    g_ui_scene = new UI();
    g_ui_scene->initialise(g_player_lives);

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef __EMSCRIPTEN__
    // Size the canvas to the browser window now, and keep it in sync on resize.
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, on_web_resize);
    fit_canvas_to_window();
#endif
}

void process_input()
{
    // Reset player movement
    if (g_current_scene->get_scene_type() == LEVEL) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
        g_current_scene->get_state().clone->set_movement(glm::vec3(0.0f));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            case SDL_WINDOWEVENT_RESIZED:
            {
                int w = event.window.data1;
                int h = event.window.data2;

                if (w > 0 && h > 0)
                    window_resize(w, h);
                break;
            }
            case SDL_WINDOWEVENT_FOCUS_LOST:
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            case SDL_WINDOWEVENT_SHOWN:
            {
                SDL_GL_MakeCurrent(g_display_window, SDL_GL_GetCurrentContext());
                break;
            }
            default:
				break;
            }
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                g_app_status = TERMINATED;
                break;

            // Restart Scene
            case SDLK_r:
                if (g_current_scene->get_scene_type() == LEVEL)
                    switch_to_scene(g_current_scene);
                break;

            // Press Enter at Switch Scene
            case SDLK_RETURN:
                if (g_current_scene == g_end_game) {
                    switch_to_scene(g_start_scene);
                    g_ui_scene->initialise(g_player_lives);
                }
                else {
                    g_current_scene->set_scene_skip(true);
                }
                break;

            // Pause Screen
            case SDLK_p:
                if (g_current_scene->get_scene_type() == LEVEL)
                {
                    is_paused = !is_paused;
                    if (!is_paused)
                        g_previous_ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
                }
                break;

            case SDLK_0:
                if (g_current_scene->get_scene_type() == LEVEL)
                    g_current_scene->set_enemy_off(!g_current_scene->get_enemy_off());
                break;

            case SDLK_1:
                switch_to_scene(g_level_a);
                break;

            case SDLK_2:
                switch_to_scene(g_level_b);
                break;

            case SDLK_3:
                switch_to_scene(g_level_c);
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    // Player movement
    if (g_current_scene->get_scene_type() == LEVEL) {
        player_movement();

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
        {
            g_current_scene->get_state().player->normalise_movement();
            g_current_scene->get_state().clone->normalise_movement();
        }
    }
}

void update()
{
    if (is_paused) return;

    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        if (g_current_scene->get_scene_type() == LEVEL) 
            g_ui_scene->update(&g_view_matrix, &g_projection_matrix, g_current_scene->get_player_lives());
        g_effects->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;

    // ----- SCENE SWITCH ----- //
    if (g_current_scene->get_state().next_scene_id > 0)
        switch_to_scene(g_scenes[g_current_scene->get_state().next_scene_id]);
}

void render()
{
    if (is_paused) return;

    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    if (g_current_scene->get_scene_type() == LEVEL) {
        g_current_scene->render(&g_shader_program, &g_clone_shader_program, &g_map_shader_program);
        g_ui_scene->render(&g_clone_shader_program);
    }
    else {
        g_current_scene->render(&g_shader_program, &g_text_shader_program, &g_map_shader_program);
    }
    g_effects->render();
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    Mix_FreeMusic(g_bgm);
    Mix_FreeMusic(g_end_bgm);
    SDL_Quit();

    delete g_ui_scene;
    
    delete g_start_scene;
    delete g_intro_scene;
    delete g_controls_scene;
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
    delete g_end_game;

    delete g_effects;
}

void game_loop()
{
    process_input();
    update();
    render();
}

int main(int argc, char* argv[])
{
    initialise();
    
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(game_loop, 0, true);
#else
    while (g_app_status == RUNNING)
        game_loop();
    
    shutdown();
#endif
    return 0;
}


void player_movement()
{
    // ————— KEY HOLD ————— //
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    float move_amount = g_current_scene->get_state().player->get_speed() * FIXED_TIMESTEP;

    if (key_state[SDL_SCANCODE_A])
    {
        g_current_scene->get_state().player->move_left();
        g_current_scene->get_state().clone->move_right();

        if (!Mix_Playing(2)) {
            Mix_PlayChannel(2, g_current_scene->get_state().walk_sfx, -1);
        }
    }
    else if (key_state[SDL_SCANCODE_D])
    {
        g_current_scene->get_state().player->move_right();
        g_current_scene->get_state().clone->move_left();

        if (!Mix_Playing(2)) {
            Mix_PlayChannel(2, g_current_scene->get_state().walk_sfx, -1);
        }
    }
    else if (key_state[SDL_SCANCODE_W])
    {
        g_current_scene->get_state().player->move_up();
        g_current_scene->get_state().clone->move_up();

        if (!Mix_Playing(2)) {
            Mix_PlayChannel(2, g_current_scene->get_state().walk_sfx, -1);
        }
    }
    else if (key_state[SDL_SCANCODE_S])
    {
        g_current_scene->get_state().player->move_down();
        g_current_scene->get_state().clone->move_down();

        if (!Mix_Playing(2)) {
            Mix_PlayChannel(2, g_current_scene->get_state().walk_sfx, -1);
        }
    }
    else
    {
        g_current_scene->get_state().player->resting();
        g_current_scene->get_state().clone->resting();

        if (Mix_Playing(2)) { Mix_HaltChannel(2); }
    }
}