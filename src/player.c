#include "header.h"

void createPlayer(Texture *texture) {
    player = malloc(sizeof(Player));
    // Initialize basic properties
    player->x = 2;
    player->y = 2;
    player->prevX = 2;
    player->prevY = 2;
    player->isAlive = true;

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

    player->animation.currentX = player->x * CARD_SIZE + (WINDOW_WIDTH - 5 * CARD_SIZE) / 2;
    player->animation.currentY = player->y * CARD_SIZE + (WINDOW_HEIGHT - 5 * CARD_SIZE) / 2;
    player->animation.animationProgress = 1.0f;
    player->animation.animationDuration = 0.3f;
    player->animation.isAnimating = false;
    player->animation.currentMove = MOVE_NONE;

    // Load texture
    if (texture) {
        player->texture = texture;
    } else {
        player->texture = NULL;
    }
}

void drawPlayer() {
    SDL_Rect playerRect = {
        (int)player->animation.currentX,
        (int)player->animation.currentY,
        CARD_SIZE,
        CARD_SIZE
    };

    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);  // Player red
    SDL_RenderFillRect(renderer, &playerRect);

        if (!player || !player->texture || !player->texture->texture) return;

    SDL_Rect spriteClip;
    const Texture *playerTexture = player->texture;

    // Use the same logic as in `updatePlayerAnimation` to calculate the current frame
    static int frame = 0;
    static Uint32 lastFrameTime = 0;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastFrameTime > FRAME_DELAY) {
        frame = (frame + 1) % playerTexture->frameCount;
        lastFrameTime = currentTime;
    }

    if (playerTexture->alignment) { // Vertical alignment
        spriteClip.x = playerTexture->clipRect.x;
        spriteClip.y = playerTexture->clipRect.y + frame * (playerTexture->clipRect.h + playerTexture->frameGap);
        spriteClip.w = playerTexture->clipRect.w;
        spriteClip.h = playerTexture->clipRect.h;
    } else { // Horizontal alignment
        spriteClip.x = playerTexture->clipRect.x + frame * playerTexture->clipRect.w;
        spriteClip.y = playerTexture->clipRect.y;
        spriteClip.w = playerTexture->clipRect.w;
        spriteClip.h = playerTexture->clipRect.h;
    }

    float destW = spriteClip.w * 2;
    float destH = spriteClip.h * 2;

    SDL_Rect destRect = {
        player->animation.currentX + (CARD_SIZE - 10 - destW) / 2,
        player->animation.currentY + (CARD_SIZE - 10 - destH) / 2,
        destW, destH 
    };

    SDL_RenderCopy(renderer, playerTexture->texture, &spriteClip, &destRect);
}

void drawPlayerStats() {
    // Draw stats panel in top left corner
    SDL_Rect statsPanel = {20, 20, 200, 200};
    
    // Semi-transparent background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &statsPanel);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &statsPanel);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color textColor = {255, 255, 255, 255};
    char statText[32];

    // Draw HP bar
    SDL_Rect hpBarBg = {30, 40, 180, 20};
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarBg);
    
    SDL_Rect hpBarFg = {30, 40, (int)(180.0f * player->defense.hp / player->defense.maxHp), 20};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarFg);

    // Draw stats as text
    snprintf(statText, sizeof(statText), "HP: %d/%d", player->defense.hp, player->defense.maxHp);
    renderText(statText, 30, 70, textColor);

    snprintf(statText, sizeof(statText), "Attack: %d", player->attack.damage);
    renderText(statText, 30, 90, textColor);

    snprintf(statText, sizeof(statText), "Range: %d", player->attack.range);
    renderText(statText, 30, 110, textColor);

    snprintf(statText, sizeof(statText), "Ammo: %d", player->attack.ammo);
    renderText(statText, 30, 130, textColor);

    snprintf(statText, sizeof(statText), "Melee: %d", player->attack.meleeDamage);
    renderText(statText, 30, 150, textColor);

    snprintf(statText, sizeof(statText), "Defense: %d", player->defense.armor);
    renderText(statText, 30, 170, textColor);

    snprintf(statText, sizeof(statText), "Gold: %d", player->gold);
    renderText(statText, 30, 190, textColor);
}

void updatePlayerAnimation(float deltaTime) {
    if (!player->animation.isAnimating) return;
    
    player->animation.animationProgress += deltaTime / player->animation.animationDuration;
    
    if (player->animation.animationProgress >= 1.0f) {
        player->animation.animationProgress = 1.0f;
        player->animation.isAnimating = 0;
        // Snap to final position
        player->animation.currentX = player->x * CARD_SIZE + (WINDOW_WIDTH - 5 * CARD_SIZE) / 2;
        player->animation.currentY = player->y * CARD_SIZE + (WINDOW_HEIGHT - 5 * CARD_SIZE) / 2;
        return;
    }
    
    // Calculate current position using smooth easing
    float easedProgress = player->animation.animationProgress * player->animation.animationProgress * (3 - 2 * player->animation.animationProgress);
    
    float startX = player->prevX * CARD_SIZE + (WINDOW_WIDTH - 5 * CARD_SIZE) / 2;
    float startY = player->prevY * CARD_SIZE + (WINDOW_HEIGHT - 5 * CARD_SIZE) / 2;
    float targetX = player->x * CARD_SIZE + (WINDOW_WIDTH - 5 * CARD_SIZE) / 2;
    float targetY = player->y * CARD_SIZE + (WINDOW_HEIGHT - 5 * CARD_SIZE) / 2;
    
    player->animation.currentX = startX + (targetX - startX) * easedProgress;
    player->animation.currentY = startY + (targetY - startY) * easedProgress;
}

void handlePlayerInput(int gridRows, int gridCols) {
    MoveDirection moveDir = MOVE_NONE;

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
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
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
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
                grid->cells[gridRow][gridCol].type == CARD_ENEMY && !isShooting) {
                enemyInteraction(gridCol, gridRow);
            }
        }
        if (event.type == SDL_MOUSEMOTION) {
            int gridX = (WINDOW_WIDTH - (grid->cols * CARD_SIZE)) / 2;
            int gridY = (WINDOW_HEIGHT - (grid->rows * CARD_SIZE)) / 2;
            int mouseX = event.motion.x;
            int mouseY = event.motion.y;

            int gridCol = (mouseX - gridX) / CARD_SIZE;
            int gridRow = (mouseY - gridY) / CARD_SIZE;
    
            // Check if mouse is over an enemy card
            if (gridRow >= 0 && gridRow < grid->rows &&
                gridCol >= 0 && gridCol < grid->cols &&
                grid->cells[gridRow][gridCol].type == CARD_ENEMY) {
                if (canAttackEnemy(gridCol, gridRow)) {
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                } else {
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO));
                }
            } else {
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            }
        }
    }
}

void movePlayer(int dx, int dy, int gridRows, int gridCols, MoveDirection *outDirection) {
    *outDirection = MOVE_NONE;
    
    int newX = player->x + dx;
    int newY = player->y + dy;
    
    if (newX >= 0 && newX < gridCols && newY >= 0 && newY < gridRows) {
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
        if (dx > 0) *outDirection = MOVE_RIGHT;
        else if (dx < 0) *outDirection = MOVE_LEFT;
        else if (dy > 0) *outDirection = MOVE_DOWN;
        else if (dy < 0) *outDirection = MOVE_UP;
        
        player->animation.currentMove = *outDirection;
    }

    if (outDirection != MOVE_NONE) {
        interaction();
        
        startCardAnimation(*outDirection);
        shiftCards(player->prevY, player->prevX, *outDirection);
    }
}

bool canAttackEnemy(const int enemyX, int enemyY) {
    if (player->attack.ammo <= 0) return false;
    
    return isInRange(player->x, player->y, enemyX, enemyY, player->attack.range);
}

void resetPlayerPosition() {
    player->x = 2;
    player->y = 2;
    player->prevX = 2;
    player->prevY = 2;
    player->animation.currentX = player->x * CARD_SIZE + (WINDOW_WIDTH - 5 * CARD_SIZE) / 2;
    player->animation.currentY = player->y * CARD_SIZE + (WINDOW_HEIGHT - 5 * CARD_SIZE) / 2;
    player->animation.animationProgress = 1.0f;
    player->animation.isAnimating = false;
    player->animation.currentMove = MOVE_NONE;
}

void destroyPlayer() {
    if (player->texture) {
        SDL_DestroyTexture(player->texture->texture);
        player->texture = NULL; // Prevent dangling pointer
    }
}
