#ifndef GRID_H
#define GRID_H

#include <SDL2/SDL.h>
#include "player.h"
#include "card.h"

typedef struct {
    int rows;
    int cols;
    Card **cells;  // 2D array of cards
} Grid;

// Grid initialization and cleanup
void initGrid(Grid *grid, int rows, int cols);
void destroyGrid(Grid *grid);

// Grid rendering
void drawGrid(SDL_Renderer *renderer, Grid *grid, Player *player, TTF_Font *font);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, 
               int x, int y, SDL_Color color);
// Game mechanics
void populateGrid(Grid *grid);
Card generateRandomCard();

// Interaction with the player
void interaction(Grid *grid, Player *player);

#endif // GRID_H
