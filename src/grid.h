#ifndef GRID_H
#define GRID_H

#include <SDL2/SDL.h>
#include "player.h"
#include "card.h"

typedef struct {
    AnimatedCard *cards;
    int cardCount;
    int isAnimating;
    float animationDuration;
    float currentTime;
    MoveDirection direction;
} CardAnimation;

typedef struct {
    int rows;
    int cols;
    Card **cells;  // 2D array of cards
} Grid;

// Grid initialization and cleanup
void initGrid(Grid *grid, int rows, int cols);
void initCardAnimation(CardAnimation *anim);
void destroyGrid(Grid *grid);

// Grid rendering
void drawCardContent(SDL_Renderer *renderer, Card *card, SDL_Rect cardRect, TTF_Font *font);
void drawGridBackground(SDL_Renderer *renderer, const Grid *grid);
void drawGrid(SDL_Renderer *renderer, Grid *grid, Player *player, TTF_Font *font);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, 
               int x, int y, SDL_Color color);

void startCardAnimation(Grid *grid, CardAnimation *anim, Player *player, MoveDirection direction);
void updateCardAnimation(CardAnimation *anim, float deltaTime);
void drawGridWithAnimation(SDL_Renderer *renderer, Grid *grid, CardAnimation *anim, 
                         Player *player, TTF_Font *font);
void shiftCards(Grid *grid, int startRow, int startCol, MoveDirection direction);

// Game mechanics
void populateGrid(Grid *grid);
Card generateRandomCard();

// Interaction with the player
void interaction(Grid *grid, Player *player);

#endif // GRID_H
