#include "../inc/header.h"


void createPlayer(Texture *texture) {
    player = malloc(sizeof(Player));
    
    // Initialize basic properties
    player->x = 2;
    player->y = 2;
    player->prevX = 2;
    player->prevY = 2;
    player->isAlive = true;

    // Set up combat stats
    player->attack.meleeDamage = 1;
    player->attack.damage = 2;
    player->attack.range = 1;
    player->attack.ammo = 2;
    player->attack.critChance = 0.0f;
    player->attack.multiShotChance = 0.0f;

    player->defense.hp = 10;
    player->defense.maxHp = 10;
    player->defense.armor = 0;
    player->defense.armorType = ARMOR_PHYSICAL;
    player->defense.isInvulnerable = false;

    player->gold = 0;

    player->base.damage = 0;
    player->base.armor = 0;

    // Initialize animation properties using virtual coordinates
    int gridX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
    int gridY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;

    player->animation.currentX = gridX + player->x * CARD_VIRTUAL_SIZE;
    player->animation.currentY = gridY + player->y * CARD_VIRTUAL_SIZE;
    player->animation.animationProgress = 1.0f;
    player->animation.animationDuration = 0.3f;
    player->animation.isAnimating = false;
    player->animation.currentMove = MOVE_NONE;

    player->texture = texture;
}

void drawPlayer(void) {
    SDL_Rect playerRect = {
        (int)player->animation.currentX,
        (int)player->animation.currentY,
        CARD_VIRTUAL_SIZE,
        CARD_VIRTUAL_SIZE
    };

    // Draw player cell background
    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    // Only proceed if we have valid texture data
    if (!player->texture || !player->texture->texture)
        return;

    SDL_Rect spriteClip;
    const Texture *playerTexture = player->texture;

    // Calculate animation frame
    static int frame = 0;
    static Uint32 lastFrameTime = 0;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastFrameTime > FRAME_DELAY) {
        frame = (frame + 1) % playerTexture->frameCount;
        lastFrameTime = currentTime;
    }

    // Get correct frame from sprite sheet
    if (playerTexture->alignment) {
        spriteClip.x = playerTexture->clipRect.x;
        spriteClip.y = playerTexture->clipRect.y + frame * 
            (playerTexture->clipRect.h + playerTexture->frameGap);
        spriteClip.w = playerTexture->clipRect.w;
        spriteClip.h = playerTexture->clipRect.h;
    } else {
        spriteClip.x = playerTexture->clipRect.x + frame * 
            (playerTexture->clipRect.w + playerTexture->frameGap);
        spriteClip.y = playerTexture->clipRect.y;
        spriteClip.w = playerTexture->clipRect.w;
        spriteClip.h = playerTexture->clipRect.h;
    }

    // Scale sprite to fit card while maintaining aspect ratio
    float scale = fminf(
        (float)(CARD_VIRTUAL_SIZE - 8) / spriteClip.w,
        (float)(CARD_VIRTUAL_SIZE - 8) / spriteClip.h
    );
    
    int destW = (int)(spriteClip.w * scale);
    int destH = (int)(spriteClip.h * scale);

    SDL_Rect destRect = {
        playerRect.x + (CARD_VIRTUAL_SIZE - destW) / 2,
        playerRect.y + (CARD_VIRTUAL_SIZE - destH) / 2,
        destW,
        destH
    };

    SDL_RenderCopy(renderer, playerTexture->texture, &spriteClip, &destRect);
}

void drawPlayerStats(void) {
    // Draw stats panel in top left corner
    SDL_Rect statsPanel = {
        STATS_TEXT_PADDING, 
        STATS_TEXT_PADDING, 
        STATS_PANEL_WIDTH, 
        STATS_PANEL_HEIGHT
    };

    // Semi-transparent background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &statsPanel);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &statsPanel);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color textColor = {255, 255, 255, 255};

    // Draw HP bar
    SDL_Rect hpBarBg = {
        statsPanel.x + STATS_TEXT_PADDING,
        statsPanel.y + STATS_TEXT_PADDING * 3,
        statsPanel.w - STATS_TEXT_PADDING * 2,
        STATS_BAR_HEIGHT
    };
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarBg);

    SDL_Rect hpBarFg = {
        hpBarBg.x,
        hpBarBg.y,
        (int)(hpBarBg.w * ((float)player->defense.hp / player->defense.maxHp)),
        hpBarBg.h
    };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarFg);

    // Draw stats text
    int textY = statsPanel.y + STATS_BAR_HEIGHT + STATS_TEXT_PADDING * 4;
    char statText[32];

    snprintf(statText, sizeof(statText), "HP: %d/%d", player->defense.hp, player->defense.maxHp);
    renderText(statText, statsPanel.x + STATS_TEXT_PADDING, textY, textColor);
    textY += 12;

    snprintf(statText, sizeof(statText), "Attack: %d", player->attack.damage);
    renderText(statText, statsPanel.x + STATS_TEXT_PADDING, textY, textColor);
    textY += 12;

    snprintf(statText, sizeof(statText), "Range: %d", player->attack.range);
    renderText(statText, statsPanel.x + STATS_TEXT_PADDING, textY, textColor);
    textY += 12;

    snprintf(statText, sizeof(statText), "Ammo: %d", player->attack.ammo);
    renderText(statText, statsPanel.x + STATS_TEXT_PADDING, textY, textColor);
    textY += 12;

    snprintf(statText, sizeof(statText), "Melee: %d", player->attack.meleeDamage);
    renderText(statText, statsPanel.x + STATS_TEXT_PADDING, textY, textColor);
    textY += 12;

    snprintf(statText, sizeof(statText), "Defense: %d", player->defense.armor);
    renderText(statText, statsPanel.x + STATS_TEXT_PADDING, textY, textColor);
    textY += 12;

    snprintf(statText, sizeof(statText), "Gold: %d", player->gold);
    renderText(statText, statsPanel.x + STATS_TEXT_PADDING, textY, textColor);
}

void updatePlayerAnimation(float deltaTime) {
    if (!player->animation.isAnimating)
        return;

    player->animation.animationProgress += deltaTime / player->animation.animationDuration;

    if (player->animation.animationProgress >= 1.0f) {
        player->animation.animationProgress = 1.0f;
        player->animation.isAnimating = false;

        // Snap to final position using virtual coordinates
        int gridX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
        int gridY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;
        player->animation.currentX = gridX + player->x * CARD_VIRTUAL_SIZE;
        player->animation.currentY = gridY + player->y * CARD_VIRTUAL_SIZE;
        return;
    }

    // Calculate current position using smooth easing
    float easedProgress = player->animation.animationProgress * 
        player->animation.animationProgress * 
        (3 - 2 * player->animation.animationProgress);

    int gridX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
    int gridY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;

    float startX = gridX + player->prevX * CARD_VIRTUAL_SIZE;
    float startY = gridY + player->prevY * CARD_VIRTUAL_SIZE;
    float targetX = gridX + player->x * CARD_VIRTUAL_SIZE;
    float targetY = gridY + player->y * CARD_VIRTUAL_SIZE;

    player->animation.currentX = startX + (targetX - startX) * easedProgress;
    player->animation.currentY = startY + (targetY - startY) * easedProgress;
}

void handlePlayerInput(int gridRows, int gridCols)
{
    MoveDirection moveDir = MOVE_NONE;

    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_UP:
            movePlayer(0, -1, gridRows, gridCols, &moveDir);
            break;
        case SDLK_DOWN:
            movePlayer(0, 1, gridRows, gridCols, &moveDir);
            break;
        case SDLK_LEFT:
            movePlayer(-1, 0, gridRows, gridCols, &moveDir);
            break;
        case SDLK_RIGHT:
            movePlayer(1, 0, gridRows, gridCols, &moveDir);
            break;
        }
    }
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            // Translate mouse coordinates to grid coordinates
            int gridX = (WINDOW_WIDTH - (grid->cols * CARD_SIZE)) / 2;
            int gridY = (WINDOW_HEIGHT - (grid->rows * CARD_SIZE)) / 2;

            int mouseX = event.button.x;
            int mouseY = event.button.y;

            int gridCol = (mouseX - gridX) / CARD_SIZE;
            int gridRow = (mouseY - gridY) / CARD_SIZE;

            // Check if clicked card is an enemy
            if (gridRow >= 0 && gridRow < grid->rows &&
                gridCol >= 0 && gridCol < grid->cols &&
                grid->cells[gridRow][gridCol].type == CARD_ENEMY && !isShooting)
            {
                enemyInteraction(gridCol, gridRow);
            }
        }
        if (event.type == SDL_MOUSEMOTION)
        {
            int gridX = (WINDOW_WIDTH - (grid->cols * CARD_SIZE)) / 2;
            int gridY = (WINDOW_HEIGHT - (grid->rows * CARD_SIZE)) / 2;
            int mouseX = event.motion.x;
            int mouseY = event.motion.y;

            int gridCol = (mouseX - gridX) / CARD_SIZE;
            int gridRow = (mouseY - gridY) / CARD_SIZE;

            // Check if mouse is over an enemy card
            if (gridRow >= 0 && gridRow < grid->rows &&
                gridCol >= 0 && gridCol < grid->cols &&
                grid->cells[gridRow][gridCol].type == CARD_ENEMY)
            {
                if (canAttackEnemy(gridCol, gridRow))
                {
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                }
                else
                {
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO));
                }
            }
            else
            {
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            }
        }
    }
}

void movePlayer(int dx, int dy, int gridRows, int gridCols, MoveDirection *outDirection)
{
    *outDirection = MOVE_NONE;

    int newX = player->x + dx;
    int newY = player->y + dy;

    if (newX >= 0 && newX < gridCols && newY >= 0 && newY < gridRows)
    {
        // Store current position as previous
        player->prevX = player->x;
        player->prevY = player->y;

        // Update target position
        player->x = newX;
        player->y = newY;

        // Start animation
        player->animation.isAnimating = 1;
        player->animation.animationProgress = 0.0f;

        // Determine direction
        if (dx > 0)
            *outDirection = MOVE_RIGHT;
        else if (dx < 0)
            *outDirection = MOVE_LEFT;
        else if (dy > 0)
            *outDirection = MOVE_DOWN;
        else if (dy < 0)
            *outDirection = MOVE_UP;

        player->animation.currentMove = *outDirection;
    }

    if (outDirection != MOVE_NONE)
    {
        interaction();

        startCardAnimation(*outDirection);
        shiftCards(player->prevY, player->prevX, *outDirection);
    }
}

bool canAttackEnemy(const int enemyX, int enemyY)
{
    if (player->attack.ammo <= 0)
        return false;

    return isInRange(player->x, player->y, enemyX, enemyY, player->attack.range);
}

void resetPlayerPosition(void) {
    player->x = 2;
    player->y = 2;
    player->prevX = 2;
    player->prevY = 2;

    int gridX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
    int gridY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;
    
    player->animation.currentX = gridX + player->x * CARD_VIRTUAL_SIZE;
    player->animation.currentY = gridY + player->y * CARD_VIRTUAL_SIZE;
    player->animation.animationProgress = 1.0f;
    player->animation.isAnimating = false;
    player->animation.currentMove = MOVE_NONE;
}

void destroyPlayer(void)
{
    if (player->texture)
    {
        SDL_DestroyTexture(player->texture->texture);
        player->texture = NULL; // Prevent dangling pointer
    }
}
