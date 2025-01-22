#include "grid.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

void initGrid(Grid *grid, int rows, int cols) {
    grid->rows = rows;
    grid->cols = cols;

    printf("%d %d\n", rows, cols);

    // Allocate memory for the grid of cards
    grid->cells = malloc(rows * sizeof(Card *));
    for (int i = 0; i < rows; i++) {
        grid->cells[i] = malloc(cols * sizeof(Card));
        for (int j = 0; j < cols; j++) {
            // Initialize with empty cards
            grid->cells[i][j] = (Card){
                .type = CARD_NONE,
                .attack = 0,
                .defense = 0,
                .value = 0,
                .isVisible = 1
            };
        }
    }
}

void drawGrid(SDL_Renderer *renderer, Grid *grid, Player *player, TTF_Font *font) {
    int cellSize = 150;  // Each cell is 150x150 pixels
    int gridWidth = grid->cols * cellSize;
    int gridHeight = grid->rows * cellSize;
    
    // Calculate grid position to center it
    int gridX = (WINDOW_WIDTH - gridWidth) / 2;
    int gridY = (WINDOW_HEIGHT - gridHeight) / 2;

    for (int i = 0; i < grid->rows; i++) {
        for (int j = 0; j < grid->cols; j++) {
            SDL_Rect cellRect = {
                gridX + (j * cellSize),
                gridY + (i * cellSize),
                cellSize,
                cellSize
            };
            
            // Draw cell background
            if (player->x == j && player->y == i) {
                SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);  // Player red
            } else {
                SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);  // Dark gray for empty
            }
            SDL_RenderFillRect(renderer, &cellRect);
            
            // Draw cell border
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &cellRect);

            // Draw card if present
            if (grid->cells[i][j].type != CARD_NONE) {
                Card *card = &grid->cells[i][j];
                SDL_Color cardColor;
                getCardColor(card, &cardColor);
                
                // Draw card background
                SDL_SetRenderDrawColor(renderer, cardColor.r, cardColor.g, cardColor.b, cardColor.a);
                SDL_Rect cardRect = {
                    cellRect.x + 5,
                    cellRect.y + 5,
                    cellRect.w - 10,
                    cellRect.h - 10
                };
                SDL_RenderFillRect(renderer, &cardRect);

                // Prepare text for card stats
                SDL_Color textColor = {255, 255, 255, 255};
                char statText[32];
                
                // Draw card stats as text
                if (card->attack > 0) {
                    snprintf(statText, sizeof(statText), "ATK: %d", card->attack);
                    renderText(renderer, font, statText, 
                             cardRect.x + 10, cardRect.y + 10, textColor);
                }
                
                if (card->defense > 0) {
                    snprintf(statText, sizeof(statText), "DEF: %d", card->defense);
                    renderText(renderer, font, statText,
                             cardRect.x + 10, cardRect.y + 30, textColor);
                }
                
                if (card->value > 0) {
                    switch (card->type) {
                        case CARD_POTION:
                            snprintf(statText, sizeof(statText), "HEAL: %d", card->value);
                            break;
                        case CARD_COIN:
                            snprintf(statText, sizeof(statText), "GOLD: %d", card->value);
                            break;
                        case CARD_KEY:
                            snprintf(statText, sizeof(statText), "KEY");
                            break;
                        default:
                            statText[0] = '\0';
                    }
                    if (statText[0]) {
                        renderText(renderer, font, statText,
                                 cardRect.x + 10, cardRect.y + 50, textColor);
                    }
                }
            }
        }
    }
}

// Helper function to render text
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, 
               int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect destRect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

void populateGrid(Grid *grid) {
    static int initialized = 0;
    const int y_center = grid->rows / 2 - (grid->rows % 2 == 0 ? 1 : 0);
    const int x_center = grid->cols / 2 - (grid->cols % 2 == 0 ? 1 : 0);

    if (!initialized) {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }

    printf("%d %d\n", y_center, x_center);

    // Populate all cells except center one
    for (int i = 0; i < grid->rows; i++) {
        for (int j = 0; j < grid->cols; j++) {
            if (i != y_center || j != x_center) {
                grid->cells[i][j] = generateRandomCard();
            }
        }
    }

    grid->cells[y_center][x_center] = (Card){CARD_NONE, 0, 0, 0, 1};
}

// Add this function to grid.c
Card generateRandomCard() {
    Card (*createFuncs[])(int) = {
        createWeaponCard,
        createShieldCard,
        createPotionCard,
        createCoinCard
    };
    int numCreateFuncs = 4;
    
    int createFuncIndex = rand() % (numCreateFuncs + 1);
    int value = rand() % 10 + 1;
    int defense = rand() % 10 + 1;
    
    return createFuncIndex >= numCreateFuncs ? 
           createEnemyCard(value, defense) : 
           createFuncs[createFuncIndex](value);
}

// Update interaction in grid.c
void interaction(Grid *grid, Player *player) {
    Card card = grid->cells[player->y][player->x];
    int refill_previous = 0;  // Flag to track if we should refill previous position
    
    switch (card.type) {
    case CARD_ENEMY: {
        int enemy_hp_left = card.defense - player->attack;
        int damage = enemy_hp_left > 0 ? card.attack + enemy_hp_left : 0;
        int defense_left = player->defense - damage;
        int player_hp_left = player->hp - (defense_left > 0 ? 0 : -defense_left);

        if (player_hp_left > 0) {
            player->hp = player_hp_left;
            player->defense = defense_left > 0 ? defense_left : 0;
            player->attack -= enemy_hp_left < 0 ? card.defense : player->attack;
            refill_previous = 1;
        } else {
            player->hp = 0;
            player->isAlive = -1;
            printf("Player died\n");
        }
        break;
    }
    case CARD_WEAPON: 
        player->attack = card.attack;
        refill_previous = 1;
        break;
    case CARD_SHIELD:
        player->defense = card.defense;
        refill_previous = 1;
        break;
    case CARD_COIN:
        player->gold += card.value;
        refill_previous = 1;
        break;
    case CARD_POTION:
        if (player->hp < player->maxHp) {
            player->hp += card.value;
            if (player->hp > player->maxHp) {
                player->hp = player->maxHp;
            }
        }
        refill_previous = 1;
        break;
    default:
        break;
    }

    // If interaction was successful, refill the previous position
    if (refill_previous && 
        (player->prevX != player->x || player->prevY != player->y)) {  // Only refill if we actually moved
        
        printf("Refilling previous position at (%d, %d)\n", player->prevX, player->prevY);
        // Clear the current card at the interaction spot
        grid->cells[player->y][player->x] = (Card){CARD_NONE, 0, 0, 0, 1};
        
        // Refill the previous position with a new random card
        grid->cells[player->prevY][player->prevX] = generateRandomCard();
    }
}

void destroyGrid(Grid *grid) {
    for (int i = 0; i < grid->rows; i++) {
        free(grid->cells[i]);
    }
    free(grid->cells);
}
