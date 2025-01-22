#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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
} Player;

void initPlayer(Player *player);
void movePlayer(Player *player, int dx, int dy, int gridRows, int gridCols);
void handlePlayerInput(Player *player, const SDL_Event *event, int gridRows, int gridCols);
void drawPlayerStats(SDL_Renderer *renderer, const Player *player, TTF_Font *font);

#endif // PLAYER_H
