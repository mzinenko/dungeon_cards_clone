#include "header.h"

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

    window = SDL_CreateWindow(
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load texture
    initTextures();
    loadTextures();

    initMenu();
    initHubInterface();
    initFactions();

    printf("heroTextures[0].texture: %p\n", (void*)heroTextures[0].texture);

    createPlayer(&heroTextures[0]);
    turn = 0;

    initGrid(5, 5);
    
    initGameContext();

    initStore();

    //loadFromDisk();

    initCardAnimation();
    
    initAnimationManager();

    Uint32 lastFrameTime = SDL_GetTicks();

    int running = 1;
    event = (SDL_Event){0};
    while (running) {
        Uint32 currentFrameTime = SDL_GetTicks();
        float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentFrameTime;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            if (isAnyAnimationRunning()) continue;

            switch (gameContext->currentState) {
                case STATE_MAIN_MENU:
                    handleMenuInput();
                    break;
                    
                case STATE_GAMEPLAY: {
                    // Check if player died
                    if (!player->isAlive) {
                        gameContext->currentState = STATE_GAME_OVER;
                        gameContext->score = player->gold; // Use gold as score
                    }

                    handlePlayerInput(grid->rows, grid->cols);
                    handleQuitInput();
                    break;
                }
                case STATE_STORE:
                    handleStoreInput();
                    break;
                case STATE_GAME_OVER:
                    handleGameOverInput();
                    break;
                case STATE_SAVE_SELECT:
                    handleSaveSelectInput();
                    if (fileBrowser && fileBrowser->isActive) {
                        handleFileBrowserInput();
                    }
                    break;
                case STATE_HUB:
                    handleHubInput();  // Add this line
                    break;
                case STATE_FRACTION:
                    handleFactionInput();
                    break;
            }
        }

        //updateEventObserver(deltaTime);
        updatePlayerAnimation(deltaTime);
        updateCardAnimation(deltaTime);
        
        if (!player->animation.isAnimating && !cardAnim->isAnimating)
            updateAnimations(deltaTime);

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
        SDL_RenderClear(renderer);

        // Render current state
        switch (gameContext->currentState) {
            case STATE_MAIN_MENU:
                drawMainMenu();
                break;
                
            case STATE_GAMEPLAY:
                renderEventObserver();
                drawGridWithAnimation();
                drawPlayerStats();
                renderProjectiles();
                drawQuitButton();
                break;
                
            case STATE_STORE:
                drawStore();
                break;

            case STATE_GAME_OVER:
                // Draw game state in background
                drawGrid();
                drawPlayerStats();
                // Draw game over screen on top
                drawGameOver();
                break;

            case STATE_HUB:
                drawHubInterface();
                break;
            case STATE_SAVE_SELECT: {
                drawSaveSelectUI();
                if (fileBrowser && fileBrowser->isActive) {
                    drawFileBrowser();
                }
            }
                break;
            case STATE_FRACTION:
                drawFactionUI();
                break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Cleanup
    TTF_CloseFont(font);
    destroyPlayer();
    destroyGrid();
    destroyStore();
    cleanupFileBrowser();
    destroyAllTextures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
