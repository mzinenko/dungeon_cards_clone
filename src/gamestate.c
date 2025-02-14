#include "header.h"

void initGameContext(void)
{
    gameContext = malloc(sizeof(GameContext));

    gameContext->currentState = STATE_MAIN_MENU;
    gameContext->characterSelected = 0;
    gameContext->score = 0;
}

void initializeButton(MenuButton *button,
                      const char *texturePath, int x, int y,
                      const char *identifier)
{
    button->rect = (SDL_Rect){x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    button->state = BUTTON_STATE_NORMAL;
    button->identifier = identifier;

    for (size_t i = 0; i < uiTexturesCount; i++)
    {
        if (strcmp(uiTextures[i].texturePath, texturePath) == 0)
        {
            printf("Found matching texture for button: %s\n", texturePath);
            button->texture = &uiTextures[i];
            break;
        }
    }
}

void initMenu(void)
{
    // Load background
    Texture *backgroundTexture = &uiTextures[0];
    if (!backgroundTexture)
        return;

    // Calculate button positions (centered horizontally)
    int startX = (WINDOW_WIDTH - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
    int buttonY = WINDOW_HEIGHT - BUTTON_HEIGHT - 50; // Bottom aligned

    menuButtons = malloc(3 * sizeof(MenuButton));

    // Initialize the three main buttons
    initializeButton(&menuButtons[0], "resource/public/ui/ui_btn_info.bmp",
                     startX, buttonY, "info");
    initializeButton(&menuButtons[1], "resource/public/ui/ui_btn_play.bmp",
                     startX + BUTTON_WIDTH + BUTTON_SPACING, buttonY, "play");
    initializeButton(&menuButtons[2], "resource/public/ui/ui_btn_settings.bmp",
                     startX + (BUTTON_WIDTH + BUTTON_SPACING) * 2, buttonY, "settings");
}

// In initHubInterface()
void initHubInterface(void)
{
    hubInterface = malloc(sizeof(HubInterface));

    // Initialize player photo area (top-left corner)
    hubInterface->playerPhotoRect = (SDL_Rect){
        20,  // x
        20,  // y
        200, // width
        200  // height
    };

    // Initialize stats area (below photo)
    hubInterface->statsRect = (SDL_Rect){
        20,  // x
        240, // y
        200, // width
        300  // height
    };

    // Initialize gods meter (top center)
    hubInterface->godsRect = (SDL_Rect){
        240, // x
        20,  // y
        400, // width
        100  // height
    };

    // Initialize resource display (top right)
    hubInterface->resourceRect = (SDL_Rect){
        660, // x
        20,  // y
        300, // width
        100  // height
    };

    // Initialize quit button (top right corner)
    hubInterface->quitButton = (SDL_Rect){
        WINDOW_WIDTH - 120, // x
        20,                 // y
        100,                // width
        40                  // height
    };

    // Initialize play button (bottom right corner)
    hubInterface->playButton = (SDL_Rect){
        WINDOW_WIDTH - 120, // x
        WINDOW_HEIGHT - 60, // y
        100,                // width
        40                  // height
    };

    // Update banner initialization
    int bannerWidth = 400;  // Make banners wider
    int bannerHeight = 400; // And taller
    int spacing = 40;
    int startX = (WINDOW_WIDTH - (2 * bannerWidth + spacing)) / 2;

    // Now only initialize 2 factions instead of 4
    for (int i = 0; i < 2; i++)
    {
        hubInterface->banners[i] = (Banner){
            .name = i == 0 ? "Sanctum Vanguard" : "The Crimson Path",
            .relationship = 0,
            .type = i == 0 ? FACTION_VANGUARD : FACTION_CRIMSON_PATH,
            .banner = NULL,
            .bannerRect = {
                startX + i * (bannerWidth + spacing),
                300, // Positioned lower
                bannerWidth,
                bannerHeight},
            .isHovered = false};
    }

    // Initialize hover states
    hubInterface->quitHovered = false;
    hubInterface->playHovered = false;
}

void drawMainMenu(void)
{
    // Draw background
    SDL_RenderCopy(renderer, uiTextures[0].texture, NULL, &(SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});

    for (int i = 0; i < 3; i++)
    {
        switch (menuButtons[i].state)
        {
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

        if (menuButtons[i].texture->alignment)
        { // Vertical alignment
            spriteClip.x = menuButtons[i].texture->clipRect.x;
            spriteClip.y = menuButtons[i].texture->clipRect.y + menuButtons[i].texture->currentFrame * (menuButtons[i].texture->clipRect.h + menuButtons[i].texture->frameGap);
            spriteClip.w = menuButtons[i].texture->clipRect.w;
            spriteClip.h = menuButtons[i].texture->clipRect.h;
        }
        else
        { // Horizontal alignment
            spriteClip.x = menuButtons[i].texture->clipRect.x + menuButtons[i].texture->currentFrame * (menuButtons[i].texture->clipRect.w + menuButtons[i].texture->frameGap);
            spriteClip.y = menuButtons[i].texture->clipRect.y;
            spriteClip.w = menuButtons[i].texture->clipRect.w;
            spriteClip.h = menuButtons[i].texture->clipRect.h;
        }

        SDL_RenderCopy(renderer, menuButtons[i].texture->texture, &spriteClip, &menuButtons[i].rect);
    }
}

void drawHubInterface(void)
{
    // Draw player photo
    if (hubInterface->playerPhoto)
    {
        SDL_RenderCopy(renderer, hubInterface->playerPhoto, NULL, &hubInterface->playerPhotoRect);
    }

    // Draw stats
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &hubInterface->statsRect);

    // Draw gods meter
    SDL_RenderFillRect(renderer, &hubInterface->godsRect);

    // Draw resource display
    SDL_RenderFillRect(renderer, &hubInterface->resourceRect);

    // Draw gems in resource display
    SDL_Color textColor = {255, 255, 255, 255};
    char gemText[64];
    int gemY = hubInterface->resourceRect.y + 10;
    for (int i = 0; i < 5; i++)
    {
        snprintf(gemText, sizeof(gemText), "Gem %d: %d", i + 1, progress->gems[i]);
        renderText(gemText, hubInterface->resourceRect.x + 10, gemY, textColor);
        gemY += 20;
    }

    for (int i = 0; i < 2; i++)
    {
        Banner *banner = &hubInterface->banners[i];
        SDL_Rect bannerRect = banner->bannerRect;

        // Draw banner background with hover effect
        if (banner->isHovered)
        {
            SDL_SetRenderDrawColor(renderer, 70, 70, 100, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
        }
        SDL_RenderFillRect(renderer, &bannerRect);

        // Draw banner name
        renderText(banner->name,
                   bannerRect.x + (bannerRect.w - strlen(banner->name) * 10) / 2,
                   bannerRect.y + 20,
                   textColor);

        // Draw relationship status
        char relationText[32];
        snprintf(relationText, sizeof(relationText), "Relationship: %d",
                 banner->relationship);
        renderText(relationText,
                   bannerRect.x + (bannerRect.w - strlen(relationText) * 10) / 2,
                   bannerRect.y + 50,
                   textColor);

        // Draw faction-specific info
        if (banner->type == FACTION_VANGUARD)
        {
            renderText("Discipline. Order. Protection.",
                       bannerRect.x + 20,
                       bannerRect.y + 90,
                       textColor);
            renderText("Military precision meets divine purpose",
                       bannerRect.x + 20,
                       bannerRect.y + 120,
                       textColor);
        }
        else
        {
            renderText("Evolution. Power. Freedom.",
                       bannerRect.x + 20,
                       bannerRect.y + 90,
                       textColor);
            renderText("Embrace the forest's gifts",
                       bannerRect.x + 20,
                       bannerRect.y + 120,
                       textColor);
        }

        // Draw "Enter" button at bottom of banner
        SDL_Rect enterButton = {
            bannerRect.x + (bannerRect.w - 100) / 2,
            bannerRect.y + bannerRect.h - 60,
            100,
            40};
        drawButton("Enter", enterButton, banner->isHovered);
    }

    // Draw quit button
    drawButton("Quit", hubInterface->quitButton, hubInterface->quitHovered);

    // Draw play button
    drawButton("Play", hubInterface->playButton, hubInterface->playHovered);
}

// Add handleHubInput function
void handleHubInput(void)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for (int i = 0; i < 2; i++)
    {
        Banner *banner = &hubInterface->banners[i];
        banner->isHovered = isMouseOverButton(mouseX, mouseY, banner->bannerRect);
    }

    // Update button hover states
    hubInterface->quitHovered = isMouseOverButton(mouseX, mouseY, hubInterface->quitButton);
    hubInterface->playHovered = isMouseOverButton(mouseX, mouseY, hubInterface->playButton);

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        // Check fraction banner clicks
        for (int i = 0; i < 2; i++)
        {
            if (hubInterface->banners[i].isHovered)
            {
                gameContext->currentState = STATE_FRACTION;
                switchToFaction(hubInterface->banners[i].type);
                break;
            }
        }

        if (hubInterface->quitHovered)
        {
            gameContext->currentState = STATE_MAIN_MENU;
        }
        else if (hubInterface->playHovered)
        {
            gameContext->currentState = STATE_GAMEPLAY;
            turn = 0;
            createPlayer(&heroTextures[0]);
            initGrid(5, 5);
            populateGrid();
        }
    }
}

void drawQuitButton(void)
{
    // Draw quit button in the right top corner
    SDL_Rect quitRect = {WINDOW_WIDTH - 100, 20, 80, 50};
    SDL_Color quitColor = {255, 255, 255, 255};
    SDL_Color hoverColor = {255, 255, 0, 255};
    SDL_Color color = quitColor;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (isMouseOverButton(mouseX, mouseY, quitRect))
    {
        color = hoverColor;
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &quitRect);

    renderText("Quit", WINDOW_WIDTH - 90, 25, (SDL_Color){0, 0, 0, 255});
}

void drawGameOver(void)
{
    // Create blur effect by drawing semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect fullscreen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &fullscreen);

    // Get mouse position for hover effects
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Game Over text
    SDL_Color textColor = {255, 255, 255, 255};
    renderText("Game Over",
               WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 3, textColor);

    // Score
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", gameContext->score);
    renderText(scoreText,
               WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT / 2 - 30, textColor);

    // Single button to return to town
    SDL_Rect toTownRect = {WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT * 2 / 3, 120, 50};
    drawButton("To Town", toTownRect,
               isMouseOverButton(mouseX, mouseY, toTownRect));
}

void handleMenuInput(void)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Handle mouse movement for hover states
    if (event.type == SDL_MOUSEMOTION)
    {
        for (int i = 0; i < 3; i++)
        {
            if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuButtons[i].rect))
            {
                if (menuButtons[i].state != BUTTON_STATE_CLICKED)
                {
                    menuButtons[i].state = BUTTON_STATE_HOVER;
                }
            }
            else
            {
                if (menuButtons[i].state != BUTTON_STATE_CLICKED)
                {
                    menuButtons[i].state = BUTTON_STATE_NORMAL;
                }
            }
        }
    }

    // Handle mouse clicks
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        for (int i = 0; i < 3; i++)
        {
            if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuButtons[i].rect))
            {
                menuButtons[i].state = BUTTON_STATE_CLICKED;

                // Handle button actions
                if (strcmp(menuButtons[i].identifier, "play") == 0)
                {
                    gameContext->currentState = STATE_SAVE_SELECT;
                    initSaveSelectUI();
                    // turn = 0;
                    // createPlayer(&heroTextures[0]);
                    // printf("Player melee damage: %d\n", player->attack.meleeDamage);
                    // //applyUpgrades(player, progress);
                    // printf("Player melee damage: %d\n", player->attack.meleeDamage);
                    // initGrid(5, 5);
                    // populateGrid(grid, player);
                }
                else if (strcmp(menuButtons[i].identifier, "info") == 0)
                {
                    gameContext->currentState = STATE_MAIN_MENU;
                }
                else if (strcmp(menuButtons[i].identifier, "settings") == 0)
                {
                    gameContext->currentState = STATE_MAIN_MENU;
                }
            }
        }
    }

    // Handle mouse button release
    if (event.type == SDL_MOUSEBUTTONUP)
    {
        for (int i = 0; i < 3; i++)
        {
            if (menuButtons[i].state == BUTTON_STATE_CLICKED)
            {
                menuButtons[i].state = SDL_PointInRect(&(SDL_Point){mouseX, mouseY},
                                                       &menuButtons[i].rect)
                                           ? BUTTON_STATE_HOVER
                                           : BUTTON_STATE_NORMAL;
            }
        }
    }
}

void handleQuitInput(void)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Rect quitRect = {WINDOW_WIDTH - 100, 20, 80, 50};
        if (isMouseOverButton(mouseX, mouseY, quitRect))
        {
            player->isAlive = 0;
        }
    }
}

void handleGameOverInput(void)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        progress->totalCoins += gameContext->score;
        saveProgress(saveSelectUI->selectedFile);

        int mouseX = event.button.x;
        int mouseY = event.button.y;

        // To Town button
        SDL_Rect toTownRect = {WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT * 2 / 3, 120, 50};
        if (isMouseOverButton(mouseX, mouseY, toTownRect))
        {
            gameContext->currentState = STATE_HUB;
        }
    }
}

void initSaveSelectUI(void)
{
    saveSelectUI = malloc(sizeof(SaveSelectUI));
    saveSelectUI->saveButtons = NULL;
    saveSelectUI->saveButtonCount = 0;
    saveSelectUI->selectedFile = NULL;

    // Initialize buttons
    saveSelectUI->newGameButton = (SDL_Rect){
        WINDOW_WIDTH / 2 - 100,
        WINDOW_HEIGHT - 150,
        200,
        40};

    saveSelectUI->importButton = (SDL_Rect){
        WINDOW_WIDTH / 2 - 100,
        WINDOW_HEIGHT - 100,
        200,
        40};

    saveSelectUI->backButton = (SDL_Rect){
        50,
        WINDOW_HEIGHT - 100,
        120,
        40};

    // Scan for save files
    scanSaveFiles();
    int count;
    const SaveFileInfo *saves = getSaveFiles(&count);

    // Create buttons for each save file
    saveSelectUI->saveButtons = malloc(sizeof(SaveFileButton) * count);
    saveSelectUI->saveButtonCount = count;

    for (int i = 0; i < count; i++)
    {
        saveSelectUI->saveButtons[i].rect = (SDL_Rect){
            WINDOW_WIDTH / 2 - 200,
            100 + i * 60,
            400,
            50};
        saveSelectUI->saveButtons[i].info = saves[i];
        saveSelectUI->saveButtons[i].isHovered = false;
    }
}

void drawSaveSelectUI(void)
{
    // Draw semi-transparent background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &(SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});

    // Draw title
    SDL_Color textColor = {255, 255, 255, 255};
    renderText("Select Save File", WINDOW_WIDTH / 2 - 100, 50, textColor);

    // Draw save file buttons
    for (int i = 0; i < saveSelectUI->saveButtonCount; i++)
    {
        SaveFileButton *btn = &saveSelectUI->saveButtons[i];

        // Draw button background
        SDL_SetRenderDrawColor(renderer,
                               btn->isHovered ? 100 : 70,
                               btn->isHovered ? 100 : 70,
                               btn->isHovered ? 100 : 70,
                               255);
        SDL_RenderFillRect(renderer, &btn->rect);

        // Draw save file info
        char info[256];
        time_t lastMod = btn->info.lastModified;
        struct tm *tm = localtime(&lastMod);

        snprintf(info, sizeof(info), "%s - Last played: %02d/%02d/%04d %02d:%02d - Coins: %d",
                 btn->info.filename,
                 tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
                 tm->tm_hour, tm->tm_min,
                 btn->info.preview.totalCoins);

        renderText(info, btn->rect.x + 10, btn->rect.y + 15, textColor);
    }

    // Draw action buttons
    drawButton("New Game", saveSelectUI->newGameButton, saveSelectUI->newGameHovered);
    drawButton("Import Save", saveSelectUI->importButton, saveSelectUI->importHovered);
    drawButton("Back", saveSelectUI->backButton, saveSelectUI->backHovered);
}

// Modify handleSaveSelectInput to handle delete functionality
void handleSaveSelectInput(void)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Update hover states
    saveSelectUI->newGameHovered = isMouseOverButton(mouseX, mouseY, saveSelectUI->newGameButton);
    saveSelectUI->importHovered = isMouseOverButton(mouseX, mouseY, saveSelectUI->importButton);
    saveSelectUI->backHovered = isMouseOverButton(mouseX, mouseY, saveSelectUI->backButton);

    for (int i = 0; i < saveSelectUI->saveButtonCount; i++)
    {
        saveSelectUI->saveButtons[i].isHovered =
            isMouseOverButton(mouseX, mouseY, saveSelectUI->saveButtons[i].rect);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            // Handle save file selection
            for (int i = 0; i < saveSelectUI->saveButtonCount; i++)
            {
                if (saveSelectUI->saveButtons[i].isHovered)
                {
                    if (loadSave(saveSelectUI->saveButtons[i].info.filename))
                    {
                        saveSelectUI->selectedFile = saveSelectUI->saveButtons[i].info.filename;
                        gameContext->currentState = STATE_HUB;

                        if (isDev)
                        {
                            progress->totalCoins = 10000;
                            progress->gems[0] = 100;
                            progress->gems[1] = 100;
                            progress->gems[2] = 100;
                            progress->gems[3] = 100;
                            progress->gems[4] = 100;
                        }
                    }
                    break;
                }
            }

            // Handle button clicks
            if (saveSelectUI->newGameHovered)
            {
                char filename[256];
                time_t now = time(NULL);
                strftime(filename, sizeof(filename), "save_%Y%m%d_%H%M%S.sav", localtime(&now));

                if (createNewSave(filename))
                {
                    saveSelectUI->selectedFile = filename;
                    gameContext->currentState = STATE_HUB;
                }
            }
            else if (saveSelectUI->importHovered)
            {
                initFileBrowser();
                scanDirectory();
            }
            else if (saveSelectUI->backHovered)
            {
                gameContext->currentState = STATE_MAIN_MENU;
            }
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            // Right-click for delete
            for (int i = 0; i < saveSelectUI->saveButtonCount; i++)
            {
                if (saveSelectUI->saveButtons[i].isHovered)
                {
                    if (deleteSave(saveSelectUI->saveButtons[i].info.filename))
                    {
                        // Refresh the save selection UI
                        cleanupSaveSelectUI();
                        initSaveSelectUI();
                    }
                    break;
                }
            }
        }
    }
}

void cleanupSaveSelectUI(void)
{
    if (saveSelectUI)
    {
        free(saveSelectUI->saveButtons);
        free(saveSelectUI);
        saveSelectUI = NULL;
    }
}
