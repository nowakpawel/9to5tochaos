#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <cstdio>
#include <cstdlib>
#include "game/Game.h"

int main(int /*argc*/, char* /*argv*/[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    const int W = 1280, H = 720;
    SDL_Window* window = SDL_CreateWindow(
        "9 TO 5 TO CHAOS",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        W, H,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glCtx = SDL_GL_CreateContext(window);
    if (!glCtx) {
        fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // vsync

    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        fprintf(stderr, "glewInit failed: %s\n", glewGetErrorString(glewErr));
        SDL_GL_DeleteContext(glCtx);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    {
        Game game(window, W, H);

        Uint64 prevTicks = SDL_GetTicks64();
        while (!game.shouldQuit()) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                game.handleEvent(e);
            }

            Uint64 now = SDL_GetTicks64();
            float dt = (float)(now - prevTicks) / 1000.0f;
            if (dt > 0.1f) dt = 0.1f; // cap delta time
            prevTicks = now;

            game.update(dt);
            game.render();
        }
    }

    SDL_GL_DeleteContext(glCtx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
