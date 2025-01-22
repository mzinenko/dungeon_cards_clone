#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "gamestate.h"
#include "grid.h"
#include "player.h"
#include "card.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define GRID_SIZE 750  // 3x3 grid, each cell 150x150

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init failed: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Dungeon Cards Clone",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
    );

    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize game state
    Grid grid;
    initGrid(&grid, 5, 5);
    populateGrid(&grid);
    
    Player player;
    initPlayer(&player);

    GameContext gameContext;
    initGameContext(&gameContext);

    CardAnimation cardAnim;
    initCardAnimation(&cardAnim);
    
    Uint32 lastFrameTime = SDL_GetTicks();

    int running = 1;
    SDL_Event event;
    while (running) {
        Uint32 currentFrameTime = SDL_GetTicks();
        float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentFrameTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            switch (gameContext.currentState) {
                case STATE_MAIN_MENU:
                    handleMenuInput(&event, &gameContext, &player, &grid);
                    break;
                    
                case STATE_GAMEPLAY: {
                    MoveDirection moveDir = handlePlayerInput(&player, &event, grid.rows, grid.cols);
                    interaction(&grid, &player);

                    if (moveDir != MOVE_NONE) {
                        startCardAnimation(&grid, &cardAnim, &player, moveDir);
                        shiftCards(&grid, player.prevY, player.prevX, moveDir);
                    }
                    
                    // Check if player died
                    if (player.isAlive == -1) {
                        gameContext.currentState = STATE_GAME_OVER;
                        gameContext.score = player.gold; // Use gold as score
                    }
                    break;
                }
                case STATE_GAME_OVER:
                    handleGameOverInput(&event, &gameContext, &player, &grid);
                    break;
            }
        }

        updateCardAnimation(&cardAnim, deltaTime);
        updatePlayerAnimation(&player, deltaTime);

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
        SDL_RenderClear(renderer);

        // Render current state
        switch (gameContext.currentState) {
            case STATE_MAIN_MENU:
                drawMainMenu(renderer, font);
                break;
                
            case STATE_GAMEPLAY:
                drawGridWithAnimation(renderer, &grid, &cardAnim, &player, font);
                drawPlayerStats(renderer, &player, font);
                break;
                
            case STATE_GAME_OVER:
                // Draw game state in background
                drawGrid(renderer, &grid, &player, font);
                drawPlayerStats(renderer, &player, font);
                // Draw game over screen on top
                drawGameOver(renderer, font, &gameContext);
                break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Cleanup
    TTF_CloseFont(font);
    destroyGrid(&grid);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
