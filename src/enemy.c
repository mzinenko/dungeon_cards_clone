#include "../inc/header.h"

void enemyTurn(void) {
    for (int i = 0; i < grid->rows; i++) {
        for (int j = 0; j < grid->cols; j++) {
            if (grid->cells[i][j].type == CARD_ENEMY) {
                Card *enemyCard = &grid->cells[i][j];

                if (isTriggered(enemyCard) && isInRange(player->x, player->y, j, i, enemyCard->attack.range)) {
                    float startX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2 + (j + 0.5f) * CARD_VIRTUAL_SIZE;
                    float startY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2 + (i + 0.5f) * CARD_VIRTUAL_SIZE;
                    float targetX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2 + player->x * CARD_VIRTUAL_SIZE + CARD_VIRTUAL_SIZE / 2;
                    float targetY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2 + player->y * CARD_VIRTUAL_SIZE + CARD_VIRTUAL_SIZE / 2;

                    if (!isShooting) {
                        shootProjectile(startX, startY, targetX, targetY, 1000);
                    }

                
                    Animation *enemyAnimation = (Animation *)malloc(sizeof(Animation));
                    if (!enemyAnimation) {
                        fprintf(stderr, "Failed to allocate memory for animation.\n");
                        return;
                    }

                    *enemyAnimation = (Animation) {
                        .updateAnimation = updateProjectiles,
                        .onAnimationEnd = enemyTurnResults,
                        .isRunning = false,
                        .userData = enemyCard, 
                    };

                    addAnimation(enemyAnimation);
                }
            }
        }
    }
}

void enemyInteraction(int enemyX, int enemyY) {
    
    if (!canAttackEnemy(enemyX, enemyY)) {
        return;
    }

    float startX = player->animation.currentX + CARD_VIRTUAL_SIZE / 2;
    float startY = player->animation.currentY + CARD_VIRTUAL_SIZE / 2;
    float targetX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2 + (enemyX + 0.5) * CARD_VIRTUAL_SIZE;
    float targetY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2 + (enemyY + 0.5) * CARD_VIRTUAL_SIZE;

    Animation *shootAnimation = (Animation *)malloc(sizeof(Animation));
    if (!shootAnimation) {
        fprintf(stderr, "Failed to allocate memory for animation.\n");
        return;
    }

    *shootAnimation = (Animation) {
        .updateAnimation = updateProjectiles,
        .onAnimationEnd = enemyInteractionResults,
        .isRunning = false,
        .userData = NULL,
    };


    if (!isShooting) {
        playSound("resource/music/shoot1.wav");
        shootProjectile(startX, startY, targetX, targetY, 1000);
    }

    
    Card *enemyCard = &grid->cells[enemyY][enemyX];

    shootAnimation->userData = enemyCard;

    addAnimation(shootAnimation);
}

void enemyTurnResults(Animation *animation) {
    Card *enemyCard = (Card *)animation->userData;
    
    player->defense.armor -= enemyCard->attack.damage;

    if (player->defense.armor < 0) {
        player->defense.hp += player->defense.armor;
        player->defense.armor = 0;
    }

    if (player->defense.hp <= 0) {
        player->isAlive = false;
        gameContext->score = player->gold;
        gameContext->currentState = STATE_GAME_OVER;
        printf("Player has died!\n");
    }
    
    free(animation);
}

void enemyInteractionResults(Animation *animation) {
    Card *enemyCard = (Card *)animation->userData;

    printf("Card x %d, y %d", enemyCard->animation.x, enemyCard->animation.y);

    player->attack.ammo--;
    
    int weaponDamage = player->attack.damage;


    int totalDamage = weaponDamage;

    if (((float)rand() / (float)RAND_MAX) < player->attack.critChance) {
        totalDamage *= 2;
    }    
    enemyCard->defense.armor -= totalDamage;

    if (enemyCard->defense.armor < 0) {
        enemyCard->defense.hp += enemyCard->defense.armor;
        enemyCard->defense.armor = 0;
    }

    if (enemyCard->defense.hp <= 0) {
        int posX = enemyCard->animation.x, posY = enemyCard->animation.y;
        enemyCard = createCard(CARD_COIN, enemyCard->rarity);
        grid->cells[posY][posX] = *enemyCard;
        grid->cells[posY][posX].animation.x = posX;
        grid->cells[posY][posX].animation.y = posY;
    }
}
