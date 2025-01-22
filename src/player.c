#include "player.h"
#include "grid.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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
}

void movePlayer(Player *player, int dx, int dy, int gridRows, int gridCols) {
    printf("Moving player by (%d, %d)\n", dx, dy);
    
    int newX = player->x + dx;
    int newY = player->y + dy;
    
    // Boundary checking
    if (newX >= 0 && newX < gridCols && newY >= 0 && newY < gridRows) {
        // Store current position as previous before updating
        player->prevX = player->x;
        player->prevY = player->y;
        // Update to new position
        player->x = newX;
        player->y = newY;
    }

    printf("Player moved to (%d, %d) from (%d, %d)\n", 
           player->x, player->y, player->prevX, player->prevY);
}

void handlePlayerInput(Player *player, const SDL_Event *event, int gridRows, int gridCols) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                movePlayer(player, 0, -1, gridRows, gridCols);
                break;
            case SDLK_DOWN:
                movePlayer(player, 0, 1, gridRows, gridCols);
                break;
            case SDLK_LEFT:
                movePlayer(player, -1, 0, gridRows, gridCols);
                break;
            case SDLK_RIGHT:
                movePlayer(player, 1, 0, gridRows, gridCols);
                break;
        }
    }
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
