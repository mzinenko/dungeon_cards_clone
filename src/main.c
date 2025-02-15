#include "../inc/header.h"

int main(int argc, const char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--dev") == 0)
        isDev = true;

    printf("isDev: %d\n", isDev);
    printf("Starting initialization...\n");

    // Initialize SDL with video and audio
    if (SDL_Init(SDL_INIT_FLAGS) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    printf("SDL initialized successfully\n");

    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init failed: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    printf("TTF initialized successfully\n");

<<<<<<< HEAD
=======
    // Initialize audio system
    initAudioManager();
    printf("Audio system initialized\n");

    // Create window at initial size, but allow for resizing
>>>>>>> e3a1c07d7ca4a6c44e8e786174d12a8161feb929
    window = SDL_CreateWindow(
        "Dungeon Cards Clone",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    printf("Window created: %p\n", (void *)window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        cleanupAudioManager();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    initRenderContext();

    // Load font
    font = TTF_OpenFont("./resource/fonts/Minecraftia-Regular.ttf", 12);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        cleanupAudioManager();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    initTextures();
    loadTextures();
    initMenu();
    initHubInterface();
    initFactions();
    initSettingsUI();
    createPlayer(&heroTextures[0]);
    turn = 0;
    initGrid(5, 5);
    initGameContext();
    initCardAnimation();
    initAnimationManager();

    // Play startup sound
    playMusic("resource/music/background2.wav", true);

    Uint32 lastFrameTime = SDL_GetTicks();
    running = 1;
    while (running) {
        Uint32 currentFrameTime = SDL_GetTicks();
        float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentFrameTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    updateRenderScale();
                }
            }

            if (isAnyAnimationRunning())
                continue;

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int virtualMouseX, virtualMouseY;
            windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

            switch (gameContext->currentState) {
                case STATE_MAIN_MENU:
                    handleMenuInput();
                    break;
                case STATE_GAMEPLAY:
                    if (!player->isAlive) {
                        gameContext->currentState = STATE_GAME_OVER;
                        gameContext->score = player->gold;
                    }
                    handlePlayerInput(grid->rows, grid->cols);
                    handleQuitInput();
                    break;
                case STATE_GAME_OVER:
                    handleGameOverInput();
                    break;
                case STATE_SAVE_SELECT:
                    if (fileBrowser && fileBrowser->isActive) {
                        handleFileBrowserInput();
                    } else {
                        handleSaveSelectInput();
                    }
                    break;
                case STATE_HUB:
                    handleHubInput();
                    break;
                case STATE_SETTINGS:
                    handleSettingsInput();
                    break;
                case STATE_FRACTION:
                    handleFactionInput();
                    break;
            }
        }

        updatePlayerAnimation(deltaTime);
        updateCardAnimation(deltaTime);

        if (!player->animation.isAnimating && !cardAnim->isAnimating)
            updateAnimations(deltaTime);

        beginRender();
        SDL_Rect backgroundRect = {0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT};
        SDL_RenderCopy(renderer, uiTextures[9].texture, NULL, &backgroundRect);

        switch (gameContext->currentState) {
            case STATE_MAIN_MENU:
                drawMainMenu();
                break;
            case STATE_GAMEPLAY:
                drawGameplayUI();
                break;
            case STATE_GAME_OVER:
                drawGrid();
                drawPlayerStats();
                drawGameOver();
                break;
            case STATE_HUB:
                drawHubInterface();
                break;
            case STATE_SAVE_SELECT:
                drawSaveSelectUI();
                if (fileBrowser && fileBrowser->isActive) {
                    drawFileBrowser();
                }
                break;
            case STATE_SETTINGS:
                drawSettingsUI();
                break;
            case STATE_FRACTION:
                drawFactionUI();
                break;
        }

        endRender();
        SDL_Delay(16);
    }

<<<<<<< HEAD
=======
    // Cleanup
    cleanupAudioManager();
>>>>>>> e3a1c07d7ca4a6c44e8e786174d12a8161feb929
    destroyRenderContext();
    TTF_CloseFont(font);
    destroyPlayer();
    destroyGrid();
    cleanupFileBrowser();
    destroyAllTextures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
