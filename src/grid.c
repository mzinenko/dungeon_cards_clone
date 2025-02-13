#include "../inc/header.h"

void initGrid(int rows, int cols)
{
    grid = malloc(sizeof(Grid));

    grid->rows = rows;
    grid->cols = cols;

    // Allocate memory for the grid of cards
    grid->cells = malloc(rows * sizeof(Card *));
    for (int i = 0; i < rows; i++)
    {
        grid->cells[i] = malloc(cols * sizeof(Card));
        for (int j = 0; j < cols; j++)
        {
            // Initialize with empty cards
            grid->cells[i][j] = *createEmptyCard();
        }
    }
}

void drawGridBackground(void)
{
    int gridWidth = grid->cols * CARD_SIZE;
    int gridHeight = grid->rows * CARD_SIZE;
    int gridX = (WINDOW_WIDTH - gridWidth) / 2;
    int gridY = (WINDOW_HEIGHT - gridHeight) / 2;

    for (int i = 0; i < grid->rows; i++)
    {
        for (int j = 0; j < grid->cols; j++)
        {
            SDL_Rect cellRect = {
                gridX + (j * CARD_SIZE),
                gridY + (i * CARD_SIZE),
                CARD_SIZE,
                CARD_SIZE};

            // Draw cell background
            SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255); // Dark gray for empty
            SDL_RenderFillRect(renderer, &cellRect);

            // Draw cell border
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }
}

void drawGrid(void)
{
    int gridWidth = grid->cols * CARD_SIZE;
    int gridHeight = grid->rows * CARD_SIZE;

    // Calculate grid position to center it
    int gridX = (WINDOW_WIDTH - gridWidth) / 2;
    int gridY = (WINDOW_HEIGHT - gridHeight) / 2;

    for (int i = 0; i < grid->rows; i++)
    {
        for (int j = 0; j < grid->cols; j++)
        {
            SDL_Rect cellRect = {
                gridX + (j * CARD_SIZE),
                gridY + (i * CARD_SIZE),
                CARD_SIZE,
                CARD_SIZE};

            // Draw cell background
            if (player->x == j && player->y == i)
            {
                SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255); // Player red
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255); // Dark gray for empty
            }
            SDL_RenderFillRect(renderer, &cellRect);

            // Draw cell border
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &cellRect);

            // Draw card if present
            if (grid->cells[i][j].type != CARD_NONE)
            {
                SDL_Rect cardRect = {
                    cellRect.x + 5,
                    cellRect.y + 5,
                    cellRect.w - 10,
                    cellRect.h - 10};
                drawCardContent(&grid->cells[i][j], cardRect);
            }
        }
    }
}

void initCardAnimation(void)
{
    cardAnim = malloc(sizeof(CardAnimation));

    cardAnim->cards = NULL;
    cardAnim->cardCount = 0;
    cardAnim->isAnimating = 0;
    cardAnim->animationDuration = 0.3f; // 300ms animation
    cardAnim->currentTime = 0;
    cardAnim->direction = MOVE_NONE;
}

void startCardAnimation(MoveDirection direction)
{
    int gridX = (WINDOW_WIDTH - (grid->cols * CARD_SIZE)) / 2;
    int gridY = (WINDOW_HEIGHT - (grid->rows * CARD_SIZE)) / 2;

    // Count cards that need to move
    int count = 0;
    switch (direction)
    {
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
    cardAnim->cards = malloc(sizeof(AnimatedCard) * count);
    cardAnim->cardCount = count;
    cardAnim->isAnimating = 1;
    cardAnim->currentTime = 0;
    cardAnim->direction = direction;

    // Setup animation data for each card
    int idx = 0;
    switch (direction)
    {
    case MOVE_UP:
        for (int i = player->prevY + 1; i < grid->rows; i++)
        {
            cardAnim->cards[idx].card = grid->cells[i][player->prevX];
            cardAnim->cards[idx].startX = gridX + player->prevX * CARD_SIZE;
            cardAnim->cards[idx].startY = gridY + i * CARD_SIZE;
            cardAnim->cards[idx].targetX = gridX + player->prevX * CARD_SIZE;
            cardAnim->cards[idx].targetY = gridY + (i - 1) * CARD_SIZE;
            cardAnim->cards[idx].currentX = cardAnim->cards[idx].startX;
            cardAnim->cards[idx].currentY = cardAnim->cards[idx].startY;
            cardAnim->cards[idx].animationProgress = 0;
            idx++;
        }
        break;
    case MOVE_DOWN:
        for (int i = player->prevY - 1; i >= 0; i--)
        {
            cardAnim->cards[idx].card = grid->cells[i][player->prevX];
            cardAnim->cards[idx].startX = gridX + player->prevX * CARD_SIZE;
            cardAnim->cards[idx].startY = gridY + i * CARD_SIZE;
            cardAnim->cards[idx].targetX = gridX + player->prevX * CARD_SIZE;
            cardAnim->cards[idx].targetY = gridY + (i + 1) * CARD_SIZE;
            cardAnim->cards[idx].currentX = cardAnim->cards[idx].startX;
            cardAnim->cards[idx].currentY = cardAnim->cards[idx].startY;
            cardAnim->cards[idx].animationProgress = 0;
            idx++;
        }
        break;
    case MOVE_LEFT:
        for (int i = player->prevX + 1; i < grid->cols; i++)
        {
            cardAnim->cards[idx].card = grid->cells[player->prevY][i];
            cardAnim->cards[idx].startX = gridX + i * CARD_SIZE;
            cardAnim->cards[idx].startY = gridY + player->prevY * CARD_SIZE;
            cardAnim->cards[idx].targetX = gridX + (i - 1) * CARD_SIZE;
            cardAnim->cards[idx].targetY = gridY + player->prevY * CARD_SIZE;
            cardAnim->cards[idx].currentX = cardAnim->cards[idx].startX;
            cardAnim->cards[idx].currentY = cardAnim->cards[idx].startY;
            cardAnim->cards[idx].animationProgress = 0;
            idx++;
        }
        break;
    case MOVE_RIGHT:
        for (int i = player->prevX - 1; i >= 0; i--)
        {
            cardAnim->cards[idx].card = grid->cells[player->prevY][i];
            cardAnim->cards[idx].startX = gridX + i * CARD_SIZE;
            cardAnim->cards[idx].startY = gridY + player->prevY * CARD_SIZE;
            cardAnim->cards[idx].targetX = gridX + (i + 1) * CARD_SIZE;
            cardAnim->cards[idx].targetY = gridY + player->prevY * CARD_SIZE;
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

void updateCardAnimation(float deltaTime)
{
    if (!cardAnim->isAnimating)
        return;

    cardAnim->currentTime += deltaTime;
    float progress = cardAnim->currentTime / cardAnim->animationDuration;

    if (progress >= 1.0f)
    {
        cardAnim->isAnimating = 0;
        free(cardAnim->cards);
        cardAnim->cards = NULL;
        cardAnim->cardCount = 0;
        return;
    }

    // Update card positions using smooth interpolation
    float easedProgress = progress * progress * (3 - 2 * progress); // Smooth easing

    for (int i = 0; i < cardAnim->cardCount; i++)
    {
        cardAnim->cards[i].animationProgress = easedProgress;
        cardAnim->cards[i].currentX = cardAnim->cards[i].startX +
                                      (cardAnim->cards[i].targetX - cardAnim->cards[i].startX) * easedProgress;
        cardAnim->cards[i].currentY = cardAnim->cards[i].startY +
                                      (cardAnim->cards[i].targetY - cardAnim->cards[i].startY) * easedProgress;
    }
}

void drawGridWithAnimation(void)
{
    int gridWidth = grid->cols * CARD_SIZE;
    int gridHeight = grid->rows * CARD_SIZE;
    int gridX = (WINDOW_WIDTH - gridWidth) / 2;
    int gridY = (WINDOW_HEIGHT - gridHeight) / 2;

    // First draw the grid background
    drawGridBackground();

    if (cardAnim->isAnimating)
    {
        // During animation, only draw non-animated cards
        for (int i = 0; i < grid->rows; i++)
        {
            for (int j = 0; j < grid->cols; j++)
            {
                // Skip the entire row/column being animated based on player's previous position
                if (((cardAnim->direction == MOVE_LEFT && j >= player->x) || (cardAnim->direction == MOVE_RIGHT && j <= player->x)) && (player->y == i))
                {
                    continue;
                }
                if (((cardAnim->direction == MOVE_UP && i >= player->y) || (cardAnim->direction == MOVE_DOWN && i <= player->y)) && (player->x == j))
                {
                    continue;
                }

                if (grid->cells[i][j].type != CARD_NONE)
                {
                    SDL_Rect cardRect = {
                        gridX + (j * CARD_SIZE) + 5,
                        gridY + (i * CARD_SIZE) + 5,
                        CARD_SIZE - 10,
                        CARD_SIZE - 10};
                    drawCardContent(&grid->cells[i][j], cardRect);
                }
            }
        }

        // Draw animated cards on top
        for (int i = 0; i < cardAnim->cardCount; i++)
        {
            SDL_Rect cellRect = {
                (int)cardAnim->cards[i].currentX + 5,
                (int)cardAnim->cards[i].currentY + 5,
                CARD_SIZE - 10,
                CARD_SIZE - 10};

            // Draw card background
            SDL_Color cardColor;
            getCardColor(&cardAnim->cards[i].card, &cardColor);
            SDL_SetRenderDrawColor(renderer, cardColor.r, cardColor.g, cardColor.b, cardColor.a);
            SDL_RenderFillRect(renderer, &cellRect);

            // Draw card content
            drawCardContent(&cardAnim->cards[i].card, cellRect);
        }
    }
    else
    {
        // When not animating, draw all cards normally
        for (int i = 0; i < grid->rows; i++)
        {
            for (int j = 0; j < grid->cols; j++)
            {
                if (grid->cells[i][j].type != CARD_NONE)
                {
                    SDL_Rect cardRect = {
                        gridX + (j * CARD_SIZE) + 5,
                        gridY + (i * CARD_SIZE) + 5,
                        CARD_SIZE - 10,
                        CARD_SIZE - 10};
                    drawCardContent(&grid->cells[i][j], cardRect);
                }
            }
        }
    }

    drawPlayer();
}

void populateGrid(void)
{
    static int initialized = 0;
    const int y_center = grid->rows / 2 - (grid->rows % 2 == 0 ? 1 : 0);
    const int x_center = grid->cols / 2 - (grid->cols % 2 == 0 ? 1 : 0);

    if (!initialized)
    {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }

    // Populate all cells except center one
    for (int i = 0; i < grid->rows; i++)
    {
        for (int j = 0; j < grid->cols; j++)
        {
            if (i != y_center || j != x_center)
            {
                grid->cells[i][j] = *generateCard();
                grid->cells[i][j].animation.x = j;
                grid->cells[i][j].animation.y = i;
            }
        }
    }

    grid->cells[y_center][x_center] = *createEmptyCard();
}

void shiftCards(int startRow, int startCol, MoveDirection direction)
{
    Card card = *generateCard(); // Generate new card

    switch (direction)
    {
    case MOVE_UP:
        for (int row = startRow + 1; row < grid->rows; row++)
        {
            // Shift the card at (row, startCol) to (row-1, startCol)
            grid->cells[row - 1][startCol] = grid->cells[row][startCol];
            grid->cells[row - 1][startCol].animation.x = startCol; // Update animation.x
            grid->cells[row - 1][startCol].animation.y = row - 1;  // Update animation.y
        }
        // Add new card at bottom and update its animation
        grid->cells[grid->rows - 1][startCol] = card;
        grid->cells[grid->rows - 1][startCol].animation.x = startCol;
        grid->cells[grid->rows - 1][startCol].animation.y = grid->rows - 1;
        break;

    case MOVE_DOWN:
        for (int row = startRow - 1; row >= 0; row--)
        {
            // Shift the card at (row, startCol) to (row+1, startCol)
            grid->cells[row + 1][startCol] = grid->cells[row][startCol];
            grid->cells[row + 1][startCol].animation.x = startCol; // Update animation.x
            grid->cells[row + 1][startCol].animation.y = row + 1;  // Update animation.y
        }
        // Add new card at top and update its animation
        grid->cells[0][startCol] = card;
        grid->cells[0][startCol].animation.x = startCol;
        grid->cells[0][startCol].animation.y = 0;
        break;

    case MOVE_LEFT:
        for (int col = startCol + 1; col < grid->cols; col++)
        {
            // Shift the card at (startRow, col) to (startRow, col-1)
            grid->cells[startRow][col - 1] = grid->cells[startRow][col];
            grid->cells[startRow][col - 1].animation.x = col - 1;  // Update animation.x
            grid->cells[startRow][col - 1].animation.y = startRow; // Update animation.y
        }
        // Add new card at right and update its animation
        grid->cells[startRow][grid->cols - 1] = card;
        grid->cells[startRow][grid->cols - 1].animation.x = grid->cols - 1;
        grid->cells[startRow][grid->cols - 1].animation.y = startRow;
        break;

    case MOVE_RIGHT:
        for (int col = startCol - 1; col >= 0; col--)
        {
            // Shift the card at (startRow, col) to (startRow, col+1)
            grid->cells[startRow][col + 1] = grid->cells[startRow][col];
            grid->cells[startRow][col + 1].animation.x = col + 1;  // Update animation.x
            grid->cells[startRow][col + 1].animation.y = startRow; // Update animation.y
        }
        // Add new card at left and update its animation
        grid->cells[startRow][0] = card;
        grid->cells[startRow][0].animation.x = 0;
        grid->cells[startRow][0].animation.y = startRow;
        break;

    default:
        break;
    }

    // Print the new card details including its position and animation
    printf("New card at row %d col %d: type %s rarity %d attack %d defense %d value %d\n",
           startRow, startCol, cardTypeToString(card.type), card.rarity, card.attack.damage, card.defense.hp, card.value.value);
}

void interaction(void)
{
    printf("Player coords: (%d, %d)\n", player->x, player->y);
    Card card = grid->cells[player->y][player->x];
    int refill_previous = 0; // Flag to track if we should refill previous position

    switch (card.type)
    {
    case CARD_ENEMY:
    {
        printf("Player melee damage: %d\n", player->attack.meleeDamage);
        int enemy_hp_left = card.defense.hp + card.defense.armor - player->attack.meleeDamage;
        int damage = enemy_hp_left > 0 ? card.attack.damage + enemy_hp_left : 0;
        int defense_left = player->defense.armor - damage;
        int player_hp_left = player->defense.hp - (defense_left > 0 ? 0 : -defense_left);

        if (player_hp_left > 0)
        {
            player->defense.hp = player_hp_left;
            player->defense.armor = defense_left > 0 ? defense_left : 0;
            refill_previous = 1;
        }
        else
        {
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
        // Add gold value
        player->gold += card.value.value;
        // Add gems based on rarity
        progress->gems[card.rarity] += (int)(card.value.multiplier * 1.0f);
        refill_previous = 1;
        break;
    case CARD_POTION:
        if (player->defense.hp < player->defense.maxHp)
        {
            player->defense.hp += card.value.value;
            if (player->defense.hp > player->defense.maxHp)
            {
                player->defense.hp = player->defense.maxHp;
            }
        }
        refill_previous = 1;
        break;
    default:
        break;
    }

    // If interaction was successful, refill the previous position
    if (refill_previous &&
        (player->prevX != player->x || player->prevY != player->y))
    { // Only refill if we actually moved

        freeCard(&grid->cells[player->prevY][player->prevX]);
        // Clear the current card at the interaction spot
        grid->cells[player->y][player->x] = *createEmptyCard();
        freeCard(&grid->cells[player->prevY][player->prevX]);
        grid->cells[player->prevY][player->prevX] = *createEmptyCard();

        enemyTurn();
        turn++;
        if (turn % 5 == 0)
        {
            printf("Turn %d\n", turn);
            eventTurn();
        }
    }
}

void destroyGrid(void)
{
    for (int i = 0; i < grid->rows; i++)
    {
        freeCard(grid->cells[i]);
        free(grid->cells[i]);
    }
    free(grid->cells);
}
