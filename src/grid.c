#include "../inc/header.h"

void initGrid(int rows, int cols) {
    grid = malloc(sizeof(Grid));

    grid->rows = rows;
    grid->cols = cols;

    grid->cells = malloc(rows * sizeof(Card *));
    for (int i = 0; i < rows; i++) {
        grid->cells[i] = malloc(cols * sizeof(Card));
        for (int j = 0; j < cols; j++) {
            grid->cells[i][j] = *createEmptyCard();
        }
    }
}

void drawGridBackground(void) {
    int gridX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
    int gridY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;

    SDL_Rect gridRect = {
        gridX - 40,
        gridY - 40,
        GRID_VIRTUAL_SIZE + 80,
        GRID_VIRTUAL_SIZE + 80
    };
    SDL_RenderCopy(renderer, uiTextures[10].texture, NULL, &gridRect);

    for (int i = 0; i < grid->rows; i++) {
        for (int j = 0; j < grid->cols; j++) {
            SDL_Rect cellRect = {
                gridX + (j * CARD_VIRTUAL_SIZE),
                gridY + (i * CARD_VIRTUAL_SIZE),
                CARD_VIRTUAL_SIZE,
                CARD_VIRTUAL_SIZE
            };

            // Draw cell border
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }
}

void drawGrid(void) {
    int gridX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
    int gridY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;

    for (int i = 0; i < grid->rows; i++) {
        for (int j = 0; j < grid->cols; j++) {
            SDL_Rect cellRect = {
                gridX + (j * CARD_VIRTUAL_SIZE),
                gridY + (i * CARD_VIRTUAL_SIZE),
                CARD_VIRTUAL_SIZE,
                CARD_VIRTUAL_SIZE
            };

            if (player->x == j && player->y == i) {
                SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
            }
            SDL_RenderFillRect(renderer, &cellRect);

            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &cellRect);

            if (grid->cells[i][j].type != CARD_NONE) {
                SDL_Rect cardRect = {
                    cellRect.x + 2,
                    cellRect.y + 2,
                    cellRect.w - 4,
                    cellRect.h - 4
                };
                drawCardContent(&grid->cells[i][j], cardRect);
            }
        }
    }
}

void initCardAnimation(void) {
    cardAnim = malloc(sizeof(CardAnimation));

    cardAnim->cards = NULL;
    cardAnim->cardCount = 0;
    cardAnim->isAnimating = 0;
    cardAnim->animationDuration = 0.3f; 
    cardAnim->currentTime = 0;
    cardAnim->direction = MOVE_NONE;
}

void startCardAnimation(MoveDirection direction) {
    int gridX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
    int gridY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;

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

    cardAnim->cards = malloc(sizeof(AnimatedCard) * count);
    cardAnim->cardCount = count;
    cardAnim->isAnimating = 1;
    cardAnim->currentTime = 0;
    cardAnim->direction = direction;

    int idx = 0;
    switch (direction) {
        case MOVE_UP:
            for (int i = player->prevY + 1; i < grid->rows; i++) {
                cardAnim->cards[idx].card = grid->cells[i][player->prevX];
                cardAnim->cards[idx].startX = gridX + player->prevX * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].startY = gridY + i * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].targetX = gridX + player->prevX * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].targetY = gridY + (i - 1) * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].currentX = cardAnim->cards[idx].startX;
                cardAnim->cards[idx].currentY = cardAnim->cards[idx].startY;
                cardAnim->cards[idx].animationProgress = 0;
                idx++;
            }
            break;
        case MOVE_DOWN:
            for (int i = player->prevY - 1; i >= 0; i--) {
                cardAnim->cards[idx].card = grid->cells[i][player->prevX];
                cardAnim->cards[idx].startX = gridX + player->prevX * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].startY = gridY + i * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].targetX = gridX + player->prevX * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].targetY = gridY + (i + 1) * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].currentX = cardAnim->cards[idx].startX;
                cardAnim->cards[idx].currentY = cardAnim->cards[idx].startY;
                cardAnim->cards[idx].animationProgress = 0;
                idx++;
            }
            break;
        case MOVE_LEFT:
            for (int i = player->prevX + 1; i < grid->cols; i++) {
                cardAnim->cards[idx].card = grid->cells[player->prevY][i];
                cardAnim->cards[idx].startX = gridX + i * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].startY = gridY + player->prevY * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].targetX = gridX + (i - 1) * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].targetY = gridY + player->prevY * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].currentX = cardAnim->cards[idx].startX;
                cardAnim->cards[idx].currentY = cardAnim->cards[idx].startY;
                cardAnim->cards[idx].animationProgress = 0;
                idx++;
            }
            break;
        case MOVE_RIGHT:
            for (int i = player->prevX - 1; i >= 0; i--) {
                cardAnim->cards[idx].card = grid->cells[player->prevY][i];
                cardAnim->cards[idx].startX = gridX + i * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].startY = gridY + player->prevY * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].targetX = gridX + (i + 1) * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].targetY = gridY + player->prevY * CARD_VIRTUAL_SIZE;
                cardAnim->cards[idx].currentX = cardAnim->cards[idx].startX;
                cardAnim->cards[idx].currentY = cardAnim->cards[idx].startY;
                cardAnim->cards[idx].animationProgress = 0;
                idx++;
            }
            break;
        default:
            break;
    }
}

void updateCardAnimation(float deltaTime) {
    if (!cardAnim->isAnimating)
        return;

    cardAnim->currentTime += deltaTime;
    float progress = cardAnim->currentTime / cardAnim->animationDuration;

    if (progress >= 1.0f) {
        cardAnim->isAnimating = 0;
        free(cardAnim->cards);
        cardAnim->cards = NULL;
        cardAnim->cardCount = 0;
        return;
    }

    float easedProgress = progress * progress * (3 - 2 * progress);

    for (int i = 0; i < cardAnim->cardCount; i++) {
        cardAnim->cards[i].animationProgress = easedProgress;
        cardAnim->cards[i].currentX = cardAnim->cards[i].startX +
                                      (cardAnim->cards[i].targetX - cardAnim->cards[i].startX) * easedProgress;
        cardAnim->cards[i].currentY = cardAnim->cards[i].startY +
                                      (cardAnim->cards[i].targetY - cardAnim->cards[i].startY) * easedProgress;
    }
}

void drawGridWithAnimation(void) {
    int gridX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
    int gridY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;

    drawGridBackground();

    if (cardAnim->isAnimating) {
        for (int i = 0; i < grid->rows; i++) {
            for (int j = 0; j < grid->cols; j++) {
                if (((cardAnim->direction == MOVE_LEFT && j >= player->x) || 
                     (cardAnim->direction == MOVE_RIGHT && j <= player->x)) && 
                    (player->y == i)) {
                    continue;
                }
                if (((cardAnim->direction == MOVE_UP && i >= player->y) || 
                     (cardAnim->direction == MOVE_DOWN && i <= player->y)) && 
                    (player->x == j)) {
                    continue;
                }

                if (grid->cells[i][j].type != CARD_NONE) {
                    SDL_Rect cardRect = {
                        gridX + (j * CARD_VIRTUAL_SIZE) + 2,
                        gridY + (i * CARD_VIRTUAL_SIZE) + 2,
                        CARD_VIRTUAL_SIZE - 4,
                        CARD_VIRTUAL_SIZE - 4
                    };
                    drawCardContent(&grid->cells[i][j], cardRect);
                }
            }
        }

        for (int i = 0; i < cardAnim->cardCount; i++) {
            SDL_Rect cellRect = {
                (int)cardAnim->cards[i].currentX + 2,
                (int)cardAnim->cards[i].currentY + 2,
                CARD_VIRTUAL_SIZE - 4,
                CARD_VIRTUAL_SIZE - 4
            };

            drawCardContent(&cardAnim->cards[i].card, cellRect);
        }
    } else {
        for (int i = 0; i < grid->rows; i++) {
            for (int j = 0; j < grid->cols; j++) {
                if (grid->cells[i][j].type != CARD_NONE) {
                    SDL_Rect cardRect = {
                        gridX + (j * CARD_VIRTUAL_SIZE) + 2,
                        gridY + (i * CARD_VIRTUAL_SIZE) + 2,
                        CARD_VIRTUAL_SIZE - 4,
                        CARD_VIRTUAL_SIZE - 4
                    };
                    drawCardContent(&grid->cells[i][j], cardRect);
                }
            }
        }
    }

    drawPlayer();
}

void populateGrid(void) {
    static int initialized = 0;
    const int y_center = grid->rows / 2 - (grid->rows % 2 == 0 ? 1 : 0);
    const int x_center = grid->cols / 2 - (grid->cols % 2 == 0 ? 1 : 0);

    if (!initialized) {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }

    for (int i = 0; i < grid->rows; i++) {
        for (int j = 0; j < grid->cols; j++) {
            if (i != y_center || j != x_center) {
                grid->cells[i][j] = *generateCard();
                grid->cells[i][j].animation.x = j;
                grid->cells[i][j].animation.y = i;
            }
        }
    }

    grid->cells[y_center][x_center] = *createEmptyCard();
}

void shiftCards(int startRow, int startCol, MoveDirection direction) {
    Card card = *generateCard(); 

    switch (direction) {
    case MOVE_UP:
        for (int row = startRow + 1; row < grid->rows; row++) {        
            grid->cells[row - 1][startCol] = grid->cells[row][startCol];
            grid->cells[row - 1][startCol].animation.x = startCol;
            grid->cells[row - 1][startCol].animation.y = row - 1;
        }
        
        grid->cells[grid->rows - 1][startCol] = card;
        grid->cells[grid->rows - 1][startCol].animation.x = startCol;
        grid->cells[grid->rows - 1][startCol].animation.y = grid->rows - 1;
        break;

    case MOVE_DOWN:
        for (int row = startRow - 1; row >= 0; row--) {
            grid->cells[row + 1][startCol] = grid->cells[row][startCol];
            grid->cells[row + 1][startCol].animation.x = startCol;
            grid->cells[row + 1][startCol].animation.y = row + 1;
        }
        
        grid->cells[0][startCol] = card;
        grid->cells[0][startCol].animation.x = startCol;
        grid->cells[0][startCol].animation.y = 0;
        break;

    case MOVE_LEFT:
        for (int col = startCol + 1; col < grid->cols; col++) {
            grid->cells[startRow][col - 1] = grid->cells[startRow][col];
            grid->cells[startRow][col - 1].animation.x = col - 1; 
            grid->cells[startRow][col - 1].animation.y = startRow;
        }        
        grid->cells[startRow][grid->cols - 1] = card;
        grid->cells[startRow][grid->cols - 1].animation.x = grid->cols - 1;
        grid->cells[startRow][grid->cols - 1].animation.y = startRow;
        break;

    case MOVE_RIGHT:
        for (int col = startCol - 1; col >= 0; col--) {
            grid->cells[startRow][col + 1] = grid->cells[startRow][col];
            grid->cells[startRow][col + 1].animation.x = col + 1; 
            grid->cells[startRow][col + 1].animation.y = startRow;
        }
        
        grid->cells[startRow][0] = card;
        grid->cells[startRow][0].animation.x = 0;
        grid->cells[startRow][0].animation.y = startRow;
        break;

    default:
        break;
    }

    printf("New card at row %d col %d: type %s rarity %d attack %d defense %d value %d\n",
           startRow, startCol, cardTypeToString(card.type), card.rarity, card.attack.damage, card.defense.hp, card.value.value);
}

void interaction(void) {
    printf("Player coords: (%d, %d)\n", player->x, player->y);
    Card card = grid->cells[player->y][player->x];
    int refill_previous = 0; 

    switch (card.type) {
    case CARD_ENEMY: {
        printf("Player melee damage: %d\n", player->attack.meleeDamage);
        int enemy_hp_left = card.defense.hp + card.defense.armor - player->attack.meleeDamage;
        int damage = enemy_hp_left > 0 ? card.attack.damage + enemy_hp_left : 0;
        int defense_left = player->defense.armor - damage;
        int player_hp_left = player->defense.hp - (defense_left > 0 ? 0 : -defense_left);

        if (player_hp_left > 0) {
            player->defense.hp = player_hp_left;
            player->defense.armor = defense_left > 0 ? defense_left : 0;
            refill_previous = 1;
        }
        else {
            player->defense.hp = 0;
            player->isAlive = 0;
            printf("Player died\n");
        }
        break;
    }
    case CARD_WEAPON:
        player->attack.damage = card.attack.damage;
        player->attack.range = card.attack.range;
        player->attack.ammo = card.attack.ammo;
        player->attack.critChance = card.attack.critChance;
        refill_previous = 1;
        break;
    case CARD_SHIELD:
        player->defense.armor = card.defense.armor;
        refill_previous = 1;
        break;
    case CARD_COIN:
        player->gold += card.value.value;        
        progress->gems[card.rarity] += (int)(card.value.multiplier * 1.0f);
        refill_previous = 1;
        break;
    case CARD_POTION:
        if (player->defense.hp < player->defense.maxHp) {
            player->defense.hp += card.value.value;
            if (player->defense.hp > player->defense.maxHp) {
                player->defense.hp = player->defense.maxHp;
            }
        }
        refill_previous = 1;
        break;
    default:
        break;
    }
    
    if (refill_previous &&
        (player->prevX != player->x || player->prevY != player->y)) {
        freeCard(&grid->cells[player->prevY][player->prevX]);
        grid->cells[player->y][player->x] = *createEmptyCard();
        freeCard(&grid->cells[player->prevY][player->prevX]);
        grid->cells[player->prevY][player->prevX] = *createEmptyCard();

        enemyTurn();
        turn++;
        if (turn % 5 == 0) {
            printf("Turn %d\n", turn);
            eventTurn();
        }
    }
}

void destroyGrid(void) {
    for (int i = 0; i < grid->rows; i++) {
        freeCard(grid->cells[i]);
        free(grid->cells[i]);
    }
    free(grid->cells);
}
