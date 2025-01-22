#include "player.h"
#include "grid.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Modify player.c to include the new animation logic

void initPlayer(Player *player) {
    player->x = 2;
    player->y = 2;
    player->hp = 10;
    player->maxHp = 10;
    player->attack = 1;
    player->defense = 0;
    player->gold = 0;
    player->keys = 0;
    player->prevX = 2;
    player->prevY = 2;
    player->isAlive = 1;
    
    // Initialize animation properties
    player->currentX = player->x * 150 + (1200 - 5 * 150) / 2;  // Center in grid
    player->currentY = player->y * 150 + (800 - 5 * 150) / 2;   // Center in grid
    player->animationProgress = 1.0f;
    player->animationDuration = 0.3f;  // Match card animation duration
    player->isAnimating = 0;
    player->currentMove = MOVE_NONE;
}

void movePlayer(Player *player, int dx, int dy, int gridRows, int gridCols, MoveDirection *outDirection) {
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
        player->isAnimating = 1;
        player->animationProgress = 0.0f;
        
        // Determine direction
        if (dx > 0) *outDirection = MOVE_RIGHT;
        else if (dx < 0) *outDirection = MOVE_LEFT;
        else if (dy > 0) *outDirection = MOVE_DOWN;
        else if (dy < 0) *outDirection = MOVE_UP;
        
        player->currentMove = *outDirection;
    }
}

void updatePlayerAnimation(Player *player, float deltaTime) {
    if (!player->isAnimating) return;
    
    player->animationProgress += deltaTime / player->animationDuration;
    
    if (player->animationProgress >= 1.0f) {
        player->animationProgress = 1.0f;
        player->isAnimating = 0;
        // Snap to final position
        player->currentX = player->x * 150 + (1200 - 5 * 150) / 2;
        player->currentY = player->y * 150 + (800 - 5 * 150) / 2;
        return;
    }
    
    // Calculate current position using smooth easing
    float easedProgress = player->animationProgress * player->animationProgress * (3 - 2 * player->animationProgress);
    
    float startX = player->prevX * 150 + (1200 - 5 * 150) / 2;
    float startY = player->prevY * 150 + (800 - 5 * 150) / 2;
    float targetX = player->x * 150 + (1200 - 5 * 150) / 2;
    float targetY = player->y * 150 + (800 - 5 * 150) / 2;
    
    player->currentX = startX + (targetX - startX) * easedProgress;
    player->currentY = startY + (targetY - startY) * easedProgress;
}

void drawPlayer(SDL_Renderer *renderer, const Player *player) {
    int cellSize = 150;
    SDL_Rect playerRect = {
        (int)player->currentX,
        (int)player->currentY,
        cellSize,
        cellSize
    };
    
    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);  // Player red
    SDL_RenderFillRect(renderer, &playerRect);
}

MoveDirection handlePlayerInput(Player *player, const SDL_Event *event, int gridRows, int gridCols) {
    MoveDirection moveDir = MOVE_NONE;
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                movePlayer(player, 0, -1, gridRows, gridCols, &moveDir);
                break;
            case SDLK_DOWN:
                movePlayer(player, 0, 1, gridRows, gridCols, &moveDir);
                break;
            case SDLK_LEFT:
                movePlayer(player, -1, 0, gridRows, gridCols, &moveDir);
                break;
            case SDLK_RIGHT:
                movePlayer(player, 1, 0, gridRows, gridCols, &moveDir);
                break;
        }
    }
    return moveDir;
}

void drawPlayerStats(SDL_Renderer *renderer, const Player *player, TTF_Font *font) {
    // Draw stats panel in top left corner
    SDL_Rect statsPanel = {20, 20, 200, 160};
    
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
    
    SDL_Rect hpBarFg = {30, 40, (int)(180.0f * player->hp / player->maxHp), 20};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBarFg);

    // Draw stats as text
    snprintf(statText, sizeof(statText), "HP: %d/%d", player->hp, player->maxHp);
    renderText(renderer, font, statText, 30, 70, textColor);

    snprintf(statText, sizeof(statText), "Attack: %d", player->attack);
    renderText(renderer, font, statText, 30, 90, textColor);

    snprintf(statText, sizeof(statText), "Defense: %d", player->defense);
    renderText(renderer, font, statText, 30, 110, textColor);

    snprintf(statText, sizeof(statText), "Gold: %d", player->gold);
    renderText(renderer, font, statText, 30, 130, textColor);

    snprintf(statText, sizeof(statText), "Keys: %d", player->keys);
    renderText(renderer, font, statText, 30, 150, textColor);
}
