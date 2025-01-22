// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum {
    MOVE_NONE,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT
} MoveDirection;

typedef struct {
    int x;
    int y;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int gold;
    int keys;
    int prevX;
    int prevY;
    int isAlive;
    // Animation properties
    float currentX;
    float currentY;
    float animationProgress;
    float animationDuration;
    int isAnimating;
    MoveDirection currentMove;
} Player;

void initPlayer(Player *player);
void movePlayer(Player *player, int dx, int dy, int gridRows, int gridCols, MoveDirection *outDirection);
MoveDirection handlePlayerInput(Player *player, const SDL_Event *event, int gridRows, int gridCols);
void drawPlayerStats(SDL_Renderer *renderer, const Player *player, TTF_Font *font);
void updatePlayerAnimation(Player *player, float deltaTime);
void drawPlayer(SDL_Renderer *renderer, const Player *player);

#endif // PLAYER_H
