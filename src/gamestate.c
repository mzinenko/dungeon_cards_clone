#include "header.h"

void initGameContext(void) {
    gameContext = malloc(sizeof(GameContext));

    gameContext->currentState = STATE_MAIN_MENU;
    gameContext->characterSelected = 0;
    gameContext->score = 0;
}

void initializeButton(MenuButton *button,
                      const char *texturePath, int x, int y,
                      const char *identifier) {
    button->rect = (SDL_Rect){x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    button->state = BUTTON_STATE_NORMAL;
    button->identifier = identifier;

    for (size_t i = 0; i < uiTexturesCount; i++) {
        if (strcmp(uiTextures[i].texturePath, texturePath) == 0) {
            printf("Found matching texture for button: %s\n", texturePath);
            button->texture = &uiTextures[i];
            break;
        }
    }
}

void initMenu(void) {
    int startX = (VIRTUAL_WIDTH - (3 * MENU_BUTTON_WIDTH + 2 * MENU_BUTTON_SPACING)) / 2;
    int buttonY = VIRTUAL_HEIGHT - MENU_BUTTON_HEIGHT - 20; // Bottom aligned with padding

    menuButtons = malloc(3 * sizeof(MenuButton));

    initializeButton(&menuButtons[0], 
        "resource/public/ui/ui_btn_info.bmp",
        startX, 
        buttonY, 
        "info"
    );

    initializeButton(&menuButtons[1], 
        "resource/public/ui/ui_btn_play.bmp",
        startX + MENU_BUTTON_WIDTH + MENU_BUTTON_SPACING, 
        buttonY, 
        "play"
    );

    initializeButton(&menuButtons[2], 
        "resource/public/ui/ui_btn_settings.bmp",
        startX + (MENU_BUTTON_WIDTH + MENU_BUTTON_SPACING) * 2, 
        buttonY, 
        "settings"
    );

    for (int i = 0; i < 3; i++) {
        menuButtons[i].rect.w = MENU_BUTTON_WIDTH;
        menuButtons[i].rect.h = MENU_BUTTON_HEIGHT;
    }
}

// In initHubInterface()
void initHubInterface(void) {
    hubInterface = malloc(sizeof(HubInterface));

    hubInterface->playerPhotoRect = (SDL_Rect){
        HUB_PANEL_PADDING,
        HUB_PANEL_PADDING,
        HUB_PLAYER_PHOTO_SIZE,
        HUB_PLAYER_PHOTO_SIZE
    };

    hubInterface->statsRect = (SDL_Rect){
        HUB_PANEL_PADDING,
        HUB_PANEL_PADDING * 2 + HUB_PLAYER_PHOTO_SIZE,
        HUB_PLAYER_PHOTO_SIZE + HUB_PANEL_PADDING * 2,
        HUB_STATS_HEIGHT
    };

    hubInterface->godsRect = (SDL_Rect){
        (VIRTUAL_WIDTH - 200) / 2,
        HUB_PANEL_PADDING,
        200,
        40
    };

    hubInterface->resourceRect = (SDL_Rect){
        VIRTUAL_WIDTH - 150 - HUB_PANEL_PADDING,
        HUB_PANEL_PADDING,
        150,
        100
    };

    hubInterface->quitButton = (SDL_Rect){
        VIRTUAL_WIDTH - ACTION_BUTTON_WIDTH - HUB_PANEL_PADDING,
        HUB_PANEL_PADDING,
        ACTION_BUTTON_WIDTH,
        ACTION_BUTTON_HEIGHT
    };

    hubInterface->playButton = (SDL_Rect){
        VIRTUAL_WIDTH - ACTION_BUTTON_WIDTH - HUB_PANEL_PADDING,
        VIRTUAL_HEIGHT - ACTION_BUTTON_HEIGHT - HUB_PANEL_PADDING,
        ACTION_BUTTON_WIDTH,
        ACTION_BUTTON_HEIGHT
    };

    int bannersTotalWidth = 2 * HUB_BANNER_WIDTH + HUB_BANNER_SPACING;
    int bannersStartX = (VIRTUAL_WIDTH - bannersTotalWidth) / 2 + HUB_PLAYER_PHOTO_SIZE / 2; 
    int bannersY = (VIRTUAL_HEIGHT - HUB_BANNER_HEIGHT) / 2;


    for (int i = 0; i < 2; i++) {
        hubInterface->banners[i] = (Banner){
            .name = i == 0 ? "Sanctum Vanguard" : "The Crimson Path",
            .relationship = 0,
            .type = i == 0 ? FACTION_VANGUARD : FACTION_CRIMSON_PATH,
            .banner = NULL,
            .bannerRect = {
                bannersStartX + i * (HUB_BANNER_WIDTH + HUB_BANNER_SPACING),
                bannersY,
                HUB_BANNER_WIDTH,
                HUB_BANNER_HEIGHT
            },
            .isHovered = false
        };
    }

    hubInterface->quitHovered = false;
    hubInterface->playHovered = false;
}

void drawMainMenu(void) {
    SDL_Rect backgroundRect = {0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT};
    SDL_RenderCopy(renderer, uiTextures[0].texture, NULL, &backgroundRect);

    for (int i = 0; i < 3; i++) {
        switch (menuButtons[i].state) {
            case BUTTON_STATE_HOVER:
                menuButtons[i].texture->currentFrame = 1;
                break;
            case BUTTON_STATE_CLICKED:
                menuButtons[i].texture->currentFrame = 2;
                break;
            default:
                menuButtons[i].texture->currentFrame = 0;
        }

        SDL_Rect spriteClip;
        const Texture* buttonTexture = menuButtons[i].texture;

        if (buttonTexture->alignment) {
            spriteClip.x = buttonTexture->clipRect.x;
            spriteClip.y = buttonTexture->clipRect.y + 
                buttonTexture->currentFrame * (buttonTexture->clipRect.h + buttonTexture->frameGap);
            spriteClip.w = buttonTexture->clipRect.w;
            spriteClip.h = buttonTexture->clipRect.h;
        } else {
            spriteClip.x = buttonTexture->clipRect.x + 
                buttonTexture->currentFrame * (buttonTexture->clipRect.w + buttonTexture->frameGap);
            spriteClip.y = buttonTexture->clipRect.y;
            spriteClip.w = buttonTexture->clipRect.w;
            spriteClip.h = buttonTexture->clipRect.h;
        }

        SDL_RenderCopy(renderer, 
            buttonTexture->texture, 
            &spriteClip, 
            &menuButtons[i].rect
        );
    }
}

void drawHubInterface(void) {
<<<<<<< HEAD
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, NULL);

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &hubInterface->statsRect);

    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &hubInterface->playerPhotoRect);
=======
    // Draw player stats panel
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &hubInterface->statsRect);

    SDL_Rect frameRect = {
        hubInterface->playerPhotoRect.x - 5,
        hubInterface->playerPhotoRect.y - 5,
        hubInterface->playerPhotoRect.w + 10,
        hubInterface->playerPhotoRect.h + 10
    };
    
    SDL_RenderCopy(renderer, heroTextures[2].texture, NULL, &frameRect);
    SDL_RenderCopy(renderer, heroTextures[1].texture, NULL, &hubInterface->playerPhotoRect);
>>>>>>> e3a1c07d7ca4a6c44e8e786174d12a8161feb929

    SDL_RenderFillRect(renderer, &hubInterface->godsRect);

    SDL_RenderFillRect(renderer, &hubInterface->resourceRect);

    SDL_Color textColor = {255, 255, 255, 255};
    char resourceText[32];
    int resourceY = hubInterface->resourceRect.y + HUB_PANEL_PADDING;
    
    snprintf(resourceText, sizeof(resourceText), "Gold: %d", progress->totalCoins);
    renderText(resourceText, 
        hubInterface->resourceRect.x + HUB_PANEL_PADDING,
        resourceY,
        textColor
    );
    
    for (int i = 0; i < 5; i++) {
        resourceY += 16;
        const char* gemTypes[] = {"Common", "Uncommon", "Rare", "Epic", "Legendary"};
        snprintf(resourceText, sizeof(resourceText), "%s: %d", gemTypes[i], progress->gems[i]);
        renderText(resourceText,
            hubInterface->resourceRect.x + HUB_PANEL_PADDING,
            resourceY,
            textColor
        );
    }

    for (int i = 0; i < 2; i++) {
        Banner *banner = &hubInterface->banners[i];
        SDL_Rect bannerRect = banner->bannerRect;

        Texture *bannerTexture = &uiTextures[4 + i]; 
        if (bannerTexture->texture) {
            SDL_RenderCopy(renderer, bannerTexture->texture, NULL, &bannerRect);
        }

        renderText(banner->name,
            bannerRect.x + HUB_PANEL_PADDING * 1.5,
            bannerRect.y + HUB_PANEL_PADDING * 1.5,
            textColor
        );

        char relationText[32];
        snprintf(relationText, sizeof(relationText), "Standing: %d",
            banner->relationship);
        renderText(relationText,
            bannerRect.x + HUB_PANEL_PADDING * 1.5,
            bannerRect.y + HUB_PANEL_PADDING * 4,
            textColor
        );

        SDL_Rect enterButton = {
            bannerRect.x + (bannerRect.w / 2 - ACTION_BUTTON_WIDTH) / 2,
            bannerRect.y + bannerRect.h - ACTION_BUTTON_HEIGHT - HUB_PANEL_PADDING,
            ACTION_BUTTON_WIDTH,
            ACTION_BUTTON_HEIGHT
        };
        drawButton("Enter", enterButton, banner->isHovered);
    }

    drawButton("Quit", hubInterface->quitButton, hubInterface->quitHovered);
    drawButton("Play", hubInterface->playButton, hubInterface->playHovered);
}

void handleHubInput(void) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

    hubInterface->quitHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        hubInterface->quitButton);
    hubInterface->playHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        hubInterface->playButton);

    for (int i = 0; i < 2; i++) {
        Banner *banner = &hubInterface->banners[i];
         SDL_Rect enterButton = {
                    hubInterface->banners[i].bannerRect.x + 
                        (hubInterface->banners[i].bannerRect.w / 2 - ACTION_BUTTON_WIDTH) / 2,
                    hubInterface->banners[i].bannerRect.y + 
                        hubInterface->banners[i].bannerRect.h - ACTION_BUTTON_HEIGHT - HUB_PANEL_PADDING,
                    ACTION_BUTTON_WIDTH,
                    ACTION_BUTTON_HEIGHT
                };

        banner->isHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
            enterButton);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        for (int i = 0; i < 2; i++) {
            if (hubInterface->banners[i].isHovered) {
                SDL_Rect enterButton = {
                    hubInterface->banners[i].bannerRect.x + 
                        (hubInterface->banners[i].bannerRect.w / 2 - ACTION_BUTTON_WIDTH) / 2,
                    hubInterface->banners[i].bannerRect.y + 
                        hubInterface->banners[i].bannerRect.h - ACTION_BUTTON_HEIGHT - HUB_PANEL_PADDING,
                    ACTION_BUTTON_WIDTH,
                    ACTION_BUTTON_HEIGHT
                };
                
                if (isMouseOverButton(virtualMouseX, virtualMouseY, enterButton)) {
                    gameContext->currentState = STATE_FRACTION;
                    switchToFaction(hubInterface->banners[i].type);
                    return;
                }
            }
        }

        if (hubInterface->quitHovered) {
            gameContext->currentState = STATE_MAIN_MENU;
            return;
        }

        if (hubInterface->playHovered) {
            gameContext->currentState = STATE_GAMEPLAY;
            turn = 0;
            createPlayer(&heroTextures[0]);
            applyStartUpgrades();
            initGrid(5, 5);
            populateGrid();
            return;
        }
    }
}

void drawQuitButton(void) {
    SDL_Rect quitRect = {
        VIRTUAL_WIDTH - ACTION_BUTTON_WIDTH - 10,
        10,
        ACTION_BUTTON_WIDTH,
        ACTION_BUTTON_HEIGHT
    };

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

    bool isHovered = false;

    if (isMouseOverButton(virtualMouseX, virtualMouseY, quitRect)) {
        isHovered = true;
    }

    drawButton("Quit", quitRect, isHovered);
}

void drawGameOver(void) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect fullscreen = {0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT};
    SDL_RenderFillRect(renderer, &fullscreen);

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

    SDL_Color textColor = {255, 255, 255, 255};
    renderText("Game Over",
               VIRTUAL_WIDTH / 2 - 40,
               VIRTUAL_HEIGHT / 3, 
               textColor);

    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", gameContext->score);
    renderText(scoreText,
               VIRTUAL_WIDTH / 2 - 30, 
               VIRTUAL_HEIGHT / 2 - 15, 
               textColor);

    SDL_Rect toTownRect = {
        VIRTUAL_WIDTH / 2 - ACTION_BUTTON_WIDTH / 2,  
        VIRTUAL_HEIGHT * 2 / 3,                       
        ACTION_BUTTON_WIDTH,
        ACTION_BUTTON_HEIGHT
    };

    drawButton("To Town", toTownRect,
               isMouseOverButton(virtualMouseX, virtualMouseY, toTownRect));
}

void handleMenuInput(void) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
        
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);
    
    if (event.type == SDL_MOUSEMOTION) {
        for (int i = 0; i < 3; i++) {
            SDL_Point virtualPoint = {virtualMouseX, virtualMouseY};
            if (SDL_PointInRect(&virtualPoint, &menuButtons[i].rect)) {
                if (menuButtons[i].state != BUTTON_STATE_CLICKED) {
                    menuButtons[i].state = BUTTON_STATE_HOVER;
                }
            } else {
                if (menuButtons[i].state != BUTTON_STATE_CLICKED) {
                    menuButtons[i].state = BUTTON_STATE_NORMAL;
                }
            }
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        for (int i = 0; i < 3; i++) {
            SDL_Point virtualPoint = {virtualMouseX, virtualMouseY};
            if (SDL_PointInRect(&virtualPoint, &menuButtons[i].rect)) {
                menuButtons[i].state = BUTTON_STATE_CLICKED;
                if (strcmp(menuButtons[i].identifier, "play") == 0) {
                    gameContext->currentState = STATE_SAVE_SELECT;
                    initSaveSelectUI();
                } else if (strcmp(menuButtons[i].identifier, "info") == 0) {
                    gameContext->currentState = STATE_MAIN_MENU;
                } else if (strcmp(menuButtons[i].identifier, "settings") == 0) {
                    gameContext->currentState = STATE_SETTINGS;
                }
            }
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP) {
        for (int i = 0; i < 3; i++) {
            if (menuButtons[i].state == BUTTON_STATE_CLICKED) {
                SDL_Point virtualPoint = {virtualMouseX, virtualMouseY};
                menuButtons[i].state = SDL_PointInRect(&virtualPoint, &menuButtons[i].rect) 
                    ? BUTTON_STATE_HOVER 
                    : BUTTON_STATE_NORMAL;
            }
        }
    }
}

void handleQuitInput(void) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        int virtualMouseX, virtualMouseY;
        windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

        SDL_Rect quitRect = {
            VIRTUAL_WIDTH - ACTION_BUTTON_WIDTH - 10,  
            10,                                        
            ACTION_BUTTON_WIDTH,
            ACTION_BUTTON_HEIGHT
        };

        if (isMouseOverButton(virtualMouseX, virtualMouseY, quitRect)) {
            player->isAlive = 0;
        }
    }
}

void handleGameOverInput(void) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        progress->totalCoins += gameContext->score;
        saveProgress(saveSelectUI->selectedFile);

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        int virtualMouseX, virtualMouseY;
        windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

        SDL_Rect toTownRect = {
            VIRTUAL_WIDTH / 2 - GAME_OVER_BUTTON_WIDTH / 2,
            VIRTUAL_HEIGHT * 2 / 3,
            GAME_OVER_BUTTON_WIDTH,
            GAME_OVER_BUTTON_HEIGHT
        };

        if (isMouseOverButton(virtualMouseX, virtualMouseY, toTownRect)) {
            gameContext->currentState = STATE_HUB;
        }
    }
}

void initSaveSelectUI(void) {
    saveSelectUI = malloc(sizeof(SaveSelectUI));
    saveSelectUI->saveButtons = NULL;
    saveSelectUI->saveButtonCount = 0;
    saveSelectUI->selectedFile = NULL;

    int centerX = VIRTUAL_WIDTH / 2;
    
    saveSelectUI->newGameButton = (SDL_Rect){
        centerX - ACTION_BUTTON_WIDTH - SAVE_LIST_PADDING,
        VIRTUAL_HEIGHT - ACTION_BUTTON_HEIGHT - SAVE_LIST_PADDING * 2,
        ACTION_BUTTON_WIDTH,
        ACTION_BUTTON_HEIGHT
    };

    saveSelectUI->importButton = (SDL_Rect){
        centerX + SAVE_LIST_PADDING,
        VIRTUAL_HEIGHT - ACTION_BUTTON_HEIGHT - SAVE_LIST_PADDING * 2,
        ACTION_BUTTON_WIDTH,
        ACTION_BUTTON_HEIGHT
    };

    saveSelectUI->backButton = (SDL_Rect){
        SAVE_LIST_PADDING * 2,
        VIRTUAL_HEIGHT - ACTION_BUTTON_HEIGHT - SAVE_LIST_PADDING * 2,
        ACTION_BUTTON_WIDTH,
        ACTION_BUTTON_HEIGHT
    };

    scanSaveFiles();
    int count;
    const SaveFileInfo *saves = getSaveFiles(&count);

    saveSelectUI->saveButtons = malloc(sizeof(SaveFileButton) * count);
    saveSelectUI->saveButtonCount = count;

    int listWidth = VIRTUAL_WIDTH * 0.6f;  
    int listStartX = (VIRTUAL_WIDTH - listWidth) / 2;
    int listStartY = 40;

    for (int i = 0; i < count; i++) {
        saveSelectUI->saveButtons[i].rect = (SDL_Rect){
            listStartX,
            listStartY + i * (SAVE_BUTTON_HEIGHT + SAVE_BUTTON_SPACING),
            listWidth,
            SAVE_BUTTON_HEIGHT
        };
        saveSelectUI->saveButtons[i].info = saves[i];
        saveSelectUI->saveButtons[i].isHovered = false;
    }
}

void drawSaveSelectUI(void) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &(SDL_Rect){0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT});
    SDL_Color textColor = {255, 255, 255, 255};    
    renderText("Select Save File", 
        (VIRTUAL_WIDTH - 80) / 2,
        12,
        textColor
    );
    
    for (int i = 0; i < saveSelectUI->saveButtonCount; i++) {
        SaveFileButton *btn = &saveSelectUI->saveButtons[i];

        SDL_SetRenderDrawColor(renderer,
            btn->isHovered ? 100 : 70,
            btn->isHovered ? 100 : 70,
            btn->isHovered ? 100 : 70,
            255
        );
        SDL_RenderFillRect(renderer, &btn->rect);
        
        char info[256];
        time_t lastMod = btn->info.lastModified;
        struct tm *tm = localtime(&lastMod);

        snprintf(info, sizeof(info), "%s - Last played: %02d/%02d/%04d %02d:%02d - Coins: %d",
            btn->info.filename,
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
            tm->tm_hour, tm->tm_min,
            btn->info.preview.totalCoins
        );

        renderText(info, 
            btn->rect.x + SAVE_LIST_PADDING,
            btn->rect.y + (SAVE_BUTTON_HEIGHT - 15) / 2, 
            textColor
        );
    }

    drawButton("New Game", saveSelectUI->newGameButton, saveSelectUI->newGameHovered);
    drawButton("Import Save", saveSelectUI->importButton, saveSelectUI->importHovered);
    drawButton("Back", saveSelectUI->backButton, saveSelectUI->backHovered);
}

void handleSaveSelectInput(void) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

    saveSelectUI->newGameHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        saveSelectUI->newGameButton);
    saveSelectUI->importHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        saveSelectUI->importButton);
    saveSelectUI->backHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        saveSelectUI->backButton);

    for (int i = 0; i < saveSelectUI->saveButtonCount; i++) {
        saveSelectUI->saveButtons[i].isHovered = 
            isMouseOverButton(virtualMouseX, virtualMouseY, 
                saveSelectUI->saveButtons[i].rect);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            for (int i = 0; i < saveSelectUI->saveButtonCount; i++) {
                if (saveSelectUI->saveButtons[i].isHovered) {
                    if (loadSave(saveSelectUI->saveButtons[i].info.filename)) {
                        saveSelectUI->selectedFile = saveSelectUI->saveButtons[i].info.filename;
                        gameContext->currentState = STATE_HUB;
                        
                        if (isDev) {
                            progress->totalCoins = 10000;
                            for (int j = 0; j < 5; j++) {
                                progress->gems[j] = 100;
                            }
                        }
                    }
                    break;
                }
            }

            if (saveSelectUI->newGameHovered) {
                char filename[256];
                time_t now = time(NULL);
                strftime(filename, sizeof(filename), "save_%Y%m%d_%H%M%S.sav", 
                    localtime(&now));

                if (createNewSave(filename)) {
                    saveSelectUI->selectedFile = filename;
                    gameContext->currentState = STATE_HUB;
                }
            }
            else if (saveSelectUI->importHovered) {
                initFileBrowser();
                scanDirectory();
            }
            else if (saveSelectUI->backHovered) {
                gameContext->currentState = STATE_MAIN_MENU;
            }
        }
        else if (event.button.button == SDL_BUTTON_RIGHT) {
            for (int i = 0; i < saveSelectUI->saveButtonCount; i++) {
                if (saveSelectUI->saveButtons[i].isHovered) {
                    if (deleteSave(saveSelectUI->saveButtons[i].info.filename)) {
                        cleanupSaveSelectUI();
                        initSaveSelectUI();
                    }
                    break;
                }
            }
        }
    }
}

void cleanupSaveSelectUI(void) {
    if (saveSelectUI) {
        free(saveSelectUI->saveButtons);
        free(saveSelectUI);
        saveSelectUI = NULL;
    }
}

void drawGameplayUI(void) {
<<<<<<< HEAD
    SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
    SDL_RenderClear(renderer);

=======
    // Begin rendering to virtual resolution
>>>>>>> e3a1c07d7ca4a6c44e8e786174d12a8161feb929
    renderEventObserver();
    drawGridWithAnimation();
    drawPlayerStats();
    renderProjectiles();
    drawQuitButton();
}

