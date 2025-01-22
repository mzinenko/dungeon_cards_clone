// gamestate.h
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "grid.h"

typedef enum {
    STATE_MAIN_MENU,
    STATE_GAMEPLAY,
    STATE_GAME_OVER
} GameState;

typedef struct {
    int characterSelected;  // For future character selection
    int score;             // Can be based on gold collected or other metrics
    GameState currentState;
} GameContext;

void initGameContext(GameContext *context);
void drawMainMenu(SDL_Renderer *renderer, TTF_Font *font);
void drawGameOver(SDL_Renderer *renderer, TTF_Font *font, GameContext *context);
void handleMenuInput(SDL_Event *event, GameContext *context, Player *player, Grid *grid);
void handleGameOverInput(SDL_Event *event, GameContext *context, Player *player, Grid *grid);

#endif // GAMESTATE_H
