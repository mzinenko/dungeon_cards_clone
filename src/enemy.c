#include "../inc/header.h"

void enemyTurn(void)
{
    for (int i = 0; i < grid->rows; i++)
    {
        for (int j = 0; j < grid->cols; j++)
        {
            if (grid->cells[i][j].type == CARD_ENEMY)
            {
                Card *enemyCard = &grid->cells[i][j];

                if (isTriggered(enemyCard) && isInRange(player->x, player->y, j, i, enemyCard->attack.range))
                {
                    float startX = (WINDOW_WIDTH - GRID_SIZE) / 2 + (j + 0.5f) * CARD_SIZE;
                    float startY = (WINDOW_HEIGHT - GRID_SIZE) / 2 + (i + 0.5f) * CARD_SIZE;
                    float targetX = (WINDOW_WIDTH - GRID_SIZE) / 2 + player->x * CARD_SIZE + CARD_SIZE / 2;
                    float targetY = (WINDOW_HEIGHT - GRID_SIZE) / 2 + player->y * CARD_SIZE + CARD_SIZE / 2;

                    if (!isShooting)
                    {
                        shootProjectile(startX, startY, targetX, targetY, 1000);
                    }

                    // Dynamically allocate the animation object
                    Animation *enemyAnimation = (Animation *)malloc(sizeof(Animation));
                    if (!enemyAnimation)
                    {
                        fprintf(stderr, "Failed to allocate memory for animation.\n");
                        return;
                    }

                    *enemyAnimation = (Animation){
                        .updateAnimation = updateProjectiles,
                        .onAnimationEnd = enemyTurnResults,
                        .isRunning = false,
                        .userData = enemyCard, // Pass the attacking enemy's card
                    };

                    addAnimation(enemyAnimation);
                }
            }
        }
    }
}

void enemyInteraction(int enemyX, int enemyY)
{
    // Verify we're clicking on an enemy within attack range
    if (!canAttackEnemy(enemyX, enemyY))
    {
        return;
    }

    float startX = player->animation.currentX + CARD_SIZE / 2;
    float startY = player->animation.currentY + CARD_SIZE / 2;
    float targetX = (WINDOW_WIDTH - GRID_SIZE) / 2 + (enemyX + 0.5) * CARD_SIZE; // Adjust based on grid cell size
    float targetY = (WINDOW_HEIGHT - GRID_SIZE) / 2 + (enemyY + 0.5) * CARD_SIZE;

    Animation *shootAnimation = (Animation *)malloc(sizeof(Animation));
    if (!shootAnimation)
    {
        fprintf(stderr, "Failed to allocate memory for animation.\n");
        return;
    }

    *shootAnimation = (Animation){
        .updateAnimation = updateProjectiles,
        .onAnimationEnd = enemyInteractionResults,
        .isRunning = false,
        .userData = NULL,
    };

    // Trigger projectile animation
    if (!isShooting)
        shootProjectile(startX, startY, targetX, targetY, 1000); // 500 units/sec speed

    // Perform the calculations after the projectile hits
    Card *enemyCard = &grid->cells[enemyY][enemyX];

    shootAnimation->userData = enemyCard;

    addAnimation(shootAnimation);
}

void enemyTurnResults(Animation *animation)
{
    Card *enemyCard = (Card *)animation->userData;

    // Apply damage to player
    player->defense.armor -= enemyCard->attack.damage;

    if (player->defense.armor < 0)
    {
        player->defense.hp += player->defense.armor;
        player->defense.armor = 0;
    }

    if (player->defense.hp <= 0)
    {
        player->isAlive = false;
        gameContext->score = player->gold;
        gameContext->currentState = STATE_GAME_OVER;
        printf("Player has died!\n");
    }

    // Free the dynamically allocated animation
    free(animation);
}

void enemyInteractionResults(Animation *animation)
{
    Card *enemyCard = (Card *)animation->userData;

    printf("Card x %d, y %d", enemyCard->animation.x, enemyCard->animation.y);

    player->attack.ammo--;

    // Use weapon damage instead of melee damage
    int weaponDamage = player->attack.damage;

    // Apply damage to enemy
    int totalDamage = weaponDamage;

    // Optional: Implement critical hit chance
    if (((float)rand() / (float)RAND_MAX) < player->attack.critChance)
    {
        totalDamage *= 2;
    }

    // Reduce enemy armor first
    enemyCard->defense.armor -= totalDamage;

    if (enemyCard->defense.armor < 0)
    {
        // Excess damage goes to HP
        enemyCard->defense.hp += enemyCard->defense.armor;
        enemyCard->defense.armor = 0;
    }

    // Check if enemy is dead
    if (enemyCard->defense.hp <= 0)
    {
        int posX = enemyCard->animation.x, posY = enemyCard->animation.y;
        // Destroy enemy card and shift grid
        enemyCard = createCard(CARD_COIN, enemyCard->rarity);
        grid->cells[posY][posX] = *enemyCard;
        grid->cells[posY][posX].animation.x = posX;
        grid->cells[posY][posX].animation.y = posY;
    }
}
