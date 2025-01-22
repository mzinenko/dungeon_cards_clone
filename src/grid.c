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

// New helper function to draw card content
void drawCardContent(SDL_Renderer *renderer, Card *card, SDL_Rect cardRect, TTF_Font *font) {
    if (card->type == CARD_NONE) return;

    // Draw card background
    SDL_Color cardColor;
    getCardColor(card, &cardColor);
    SDL_SetRenderDrawColor(renderer, cardColor.r, cardColor.g, cardColor.b, cardColor.a);
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

// Modified drawGrid function
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
                SDL_Rect cardRect = {
                    cellRect.x + 5,
                    cellRect.y + 5,
                    cellRect.w - 10,
                    cellRect.h - 10
                };
                drawCardContent(renderer, &grid->cells[i][j], cardRect, font);
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

void initCardAnimation(CardAnimation *anim) {
    anim->cards = NULL;
    anim->cardCount = 0;
    anim->isAnimating = 0;
    anim->animationDuration = 0.3f; // 300ms animation
    anim->currentTime = 0;
    anim->direction = MOVE_NONE;
}

void startCardAnimation(Grid *grid, CardAnimation *anim, Player *player, MoveDirection direction) {
    int cellSize = 150;  // Match your grid cell size
    int gridX = (WINDOW_WIDTH - (grid->cols * cellSize)) / 2;
    int gridY = (WINDOW_HEIGHT - (grid->rows * cellSize)) / 2;
    
    // Count cards that need to move
    int count = 0;
    switch (direction) {
        case MOVE_UP:
            count = grid->rows - player->prevY - 1;
            break;
        case MOVE_DOWN:
            count = player->prevY;
            break;
        case MOVE_LEFT:
            count = grid->cols - player->prevX - 1;
            break;
        case MOVE_RIGHT:
            count = player->prevX;
            break;
        default:
            return;
    }
    
    // Allocate animation data
    anim->cards = malloc(sizeof(AnimatedCard) * count);
    anim->cardCount = count;
    anim->isAnimating = 1;
    anim->currentTime = 0;
    anim->direction = direction;
    
    // Setup animation data for each card
    int idx = 0;
    switch (direction) {
        case MOVE_UP:
            for (int i = player->prevY + 1; i < grid->rows; i++) {
                anim->cards[idx].card = grid->cells[i][player->prevX];
                anim->cards[idx].startX = gridX + player->prevX * cellSize;
                anim->cards[idx].startY = gridY + i * cellSize;
                anim->cards[idx].targetX = gridX + player->prevX * cellSize;
                anim->cards[idx].targetY = gridY + (i - 1) * cellSize;
                anim->cards[idx].currentX = anim->cards[idx].startX;
                anim->cards[idx].currentY = anim->cards[idx].startY;
                anim->cards[idx].animationProgress = 0;
                idx++;
            }
            break;
        case MOVE_DOWN:
            for (int i = player->prevY - 1; i >= 0; i--) {
                anim->cards[idx].card = grid->cells[i][player->prevX];
                anim->cards[idx].startX = gridX + player->prevX * cellSize;
                anim->cards[idx].startY = gridY + i * cellSize;
                anim->cards[idx].targetX = gridX + player->prevX * cellSize;
                anim->cards[idx].targetY = gridY + (i + 1) * cellSize;
                anim->cards[idx].currentX = anim->cards[idx].startX;
                anim->cards[idx].currentY = anim->cards[idx].startY;
                anim->cards[idx].animationProgress = 0;
                idx++;
            }
            break;
        case MOVE_LEFT:
            for (int i = player->prevX + 1; i < grid->cols; i++) {
                anim->cards[idx].card = grid->cells[player->prevY][i];
                anim->cards[idx].startX = gridX + i * cellSize;
                anim->cards[idx].startY = gridY + player->prevY * cellSize;
                anim->cards[idx].targetX = gridX + (i - 1) * cellSize;
                anim->cards[idx].targetY = gridY + player->prevY * cellSize;
                anim->cards[idx].currentX = anim->cards[idx].startX;
                anim->cards[idx].currentY = anim->cards[idx].startY;
                anim->cards[idx].animationProgress = 0;
                idx++;
            }
            break;
        case MOVE_RIGHT:
            for (int i = player->prevX - 1; i >= 0; i--) {
                anim->cards[idx].card = grid->cells[player->prevY][i];
                anim->cards[idx].startX = gridX + i * cellSize;
                anim->cards[idx].startY = gridY + player->prevY * cellSize;
                anim->cards[idx].targetX = gridX + (i + 1) * cellSize;
                anim->cards[idx].targetY = gridY + player->prevY * cellSize;
                anim->cards[idx].currentX = anim->cards[idx].startX;
                anim->cards[idx].currentY = anim->cards[idx].startY;
                anim->cards[idx].animationProgress = 0;
                idx++;
            }
            break;
        default:
            break;
    }
}

void updateCardAnimation(CardAnimation *anim, float deltaTime) {
    if (!anim->isAnimating) return;
    
    anim->currentTime += deltaTime;
    float progress = anim->currentTime / anim->animationDuration;
    
    if (progress >= 1.0f) {
        anim->isAnimating = 0;
        free(anim->cards);
        anim->cards = NULL;
        anim->cardCount = 0;
        return;
    }
    
    // Update card positions using smooth interpolation
    float easedProgress = progress * progress * (3 - 2 * progress); // Smooth easing
    
    for (int i = 0; i < anim->cardCount; i++) {
        anim->cards[i].animationProgress = easedProgress;
        anim->cards[i].currentX = anim->cards[i].startX + 
            (anim->cards[i].targetX - anim->cards[i].startX) * easedProgress;
        anim->cards[i].currentY = anim->cards[i].startY + 
            (anim->cards[i].targetY - anim->cards[i].startY) * easedProgress;
    }
}

// Add this new helper function to draw just the grid background
void drawGridBackground(SDL_Renderer *renderer, const Grid *grid) {
    int cellSize = 150;
    int gridWidth = grid->cols * cellSize;
    int gridHeight = grid->rows * cellSize;
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
            SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);  // Dark gray for empty
            SDL_RenderFillRect(renderer, &cellRect);
            
            // Draw cell border
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }
}

// Modify drawGridWithAnimation to use the new approach
void drawGridWithAnimation(SDL_Renderer *renderer, Grid *grid, CardAnimation *anim, 
                         Player *player, TTF_Font *font) {
    int cellSize = 150;
    int gridWidth = grid->cols * cellSize;
    int gridHeight = grid->rows * cellSize;
    int gridX = (WINDOW_WIDTH - gridWidth) / 2;
    int gridY = (WINDOW_HEIGHT - gridHeight) / 2;

    // First draw the grid background
    drawGridBackground(renderer, grid);
    
    if (anim->isAnimating) {
        // During animation, only draw non-animated cards
        for (int i = 0; i < grid->rows; i++) {
            for (int j = 0; j < grid->cols; j++) {
                printf("Direction %d", anim->direction);
                // Skip the entire row/column being animated based on player's previous position
                if (((anim->direction == MOVE_LEFT && j >= player->x) || (anim->direction == MOVE_RIGHT && j <= player->x)) && (player->y == i)) {
                    continue;
                }
                if (((anim->direction == MOVE_UP && i >= player->y) || (anim->direction == MOVE_DOWN && i <= player->y)) && (player->x == j)) {
                    continue;
                }
                
                if (grid->cells[i][j].type != CARD_NONE) {
                    SDL_Rect cardRect = {
                        gridX + (j * cellSize) + 5,
                        gridY + (i * cellSize) + 5,
                        cellSize - 10,
                        cellSize - 10
                    };
                    drawCardContent(renderer, &grid->cells[i][j], cardRect, font);
                }
            }
        }
        
        // Draw animated cards on top
        for (int i = 0; i < anim->cardCount; i++) {
            SDL_Rect cellRect = {
                (int)anim->cards[i].currentX + 5,
                (int)anim->cards[i].currentY + 5,
                cellSize - 10,
                cellSize - 10
            };
            
            // Draw card background
            SDL_Color cardColor;
            getCardColor(&anim->cards[i].card, &cardColor);
            SDL_SetRenderDrawColor(renderer, cardColor.r, cardColor.g, cardColor.b, cardColor.a);
            SDL_RenderFillRect(renderer, &cellRect);
            
            // Draw card content
            drawCardContent(renderer, &anim->cards[i].card, cellRect, font);
        }
    } else {
        // When not animating, draw all cards normally
        for (int i = 0; i < grid->rows; i++) {
            for (int j = 0; j < grid->cols; j++) {
                if (grid->cells[i][j].type != CARD_NONE) {
                    SDL_Rect cardRect = {
                        gridX + (j * cellSize) + 5,
                        gridY + (i * cellSize) + 5,
                        cellSize - 10,
                        cellSize - 10
                    };
                    drawCardContent(renderer, &grid->cells[i][j], cardRect, font);
                }
            }
        }
    }

    drawPlayer(renderer, player);
}

void shiftCards(Grid *grid, int startRow, int startCol, MoveDirection direction) {
    switch (direction) {
        case MOVE_UP:
            for (int row = startRow + 1; row < grid->rows; row++) {
                grid->cells[row - 1][startCol] = grid->cells[row][startCol];
            }
            // Add new card at bottom
            grid->cells[grid->rows - 1][startCol] = generateRandomCard();
            break;
            
        case MOVE_DOWN:
            for (int row = startRow - 1; row >= 0; row--) {
                grid->cells[row + 1][startCol] = grid->cells[row][startCol];
            }
            // Add new card at top
            grid->cells[0][startCol] = generateRandomCard();
            break;
            
        case MOVE_LEFT:
            for (int col = startCol + 1; col < grid->cols; col++) {
                grid->cells[startRow][col - 1] = grid->cells[startRow][col];
            }
            // Add new card at right
            grid->cells[startRow][grid->cols - 1] = generateRandomCard();
            break;
            
        case MOVE_RIGHT:
            for (int col = startCol - 1; col >= 0; col--) {
                grid->cells[startRow][col + 1] = grid->cells[startRow][col];
            }
            // Add new card at left
            grid->cells[startRow][0] = generateRandomCard();
            break;
            
        default:
            break;
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
        
        // Clear the current card at the interaction spot
        grid->cells[player->y][player->x] = (Card){CARD_NONE, 0, 0, 0, 1};
        grid->cells[player->prevY][player->prevX] = (Card){CARD_NONE, 0, 0, 0, 1};
    }
}

void destroyGrid(Grid *grid) {
    for (int i = 0; i < grid->rows; i++) {
        free(grid->cells[i]);
    }
    free(grid->cells);
}
