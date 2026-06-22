@echo off
REM ---- Reflexion web build ----
REM Run this from this folder (C:\NYU\2025-2026 Spring\reflexion-remake\reflexion\reflexion).
REM It activates Emscripten and compiles the game to index.html/.js/.wasm/.data.

call "C:\NYU\2025-2026 Spring\reflexion-remake\emsdk\emsdk_env.bat"

call "C:\NYU\2025-2026 Spring\reflexion-remake\emsdk\upstream\emscripten\em++.bat" ^
  main.cpp ControlsScene.cpp Effects.cpp EndGame.cpp Entity.cpp GameStart.cpp ^
  IntroScene.cpp LevelA.cpp LevelB.cpp LevelC.cpp Map.cpp ShaderProgram.cpp ^
  UI.cpp Utility.cpp ^
  -o index.html ^
  -s USE_SDL=2 -s USE_SDL_MIXER=2 ^
  -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2 -s FULL_ES2=1 ^
  -s ALLOW_MEMORY_GROWTH=1 ^
  --preload-file assets --preload-file shaders ^
  --shell-file shell.html

if %errorlevel% neq 0 (
  echo.
  echo BUILD FAILED.
) else (
  echo.
  echo Build OK. Serve with:  python -m http.server 8000
  echo Then open:            http://localhost:8000/index.html
)
