#include "../inc/header.h"

// Function to calculate card weights based on player and grid state
CardWeight *calculateCardWeights(int *weightCount) {
    CardWeight *weights = malloc(sizeof(CardWeight) * CARD_COIN + 1);
    *weightCount = 0;

    weights[(*weightCount)++] = (CardWeight){CARD_ENEMY, calculateEnemyWeight()};
    weights[(*weightCount)++] = (CardWeight){CARD_WEAPON, calculateWeaponWeight()};
    weights[(*weightCount)++] = (CardWeight){CARD_SHIELD, calculateShieldWeight()};
    weights[(*weightCount)++] = (CardWeight){CARD_POTION, calculatePotionWeight()};
    weights[(*weightCount)++] = (CardWeight){CARD_COIN, calculateCoinWeight()};

    return weights;
}

// Weighted random selection
CardType selectCardTypeByWeight(CardWeight *weights, int weightCount) {
    float totalWeight = 0;
    for (int i = 0; i < weightCount; i++) {
        totalWeight += weights[i].weight;
    }

    float randomValue = ((float)rand() / (float)RAND_MAX) * totalWeight;

    float currentSum = 0;
    for (int i = 0; i < weightCount; i++) {
        currentSum += weights[i].weight;
        if (randomValue <= currentSum) {
            return weights[i].type;
        }
    }

    return CARD_NONE;
}

CardRarity selectRarityByWeight(void) {
    float totalWeight = 0;
    for (int i = 0; i < 5; i++)
        totalWeight += RARITY_CHANCES[i];

    float randomValue = ((float)rand() / (float)RAND_MAX) * totalWeight;
    float currentSum = 0;

    for (int i = 0; i < 5; i++) {
        currentSum += RARITY_CHANCES[i];
        if (randomValue <= currentSum)
            return i;
    }

    return RARITY_COMMON;
}

float calculateEnemyWeight(void) {
    if (player->defense.hp > grid->rows) {
    }
    return 1.0f;
}

float calculateWeaponWeight(void) {
    if (player->defense.hp > grid->rows) {
    }
    return .75f;
}

float calculateShieldWeight(void) {
    if (player->defense.hp > grid->rows) {
    }
    return .5f;
}

float calculatePotionWeight(void) {
    if (player->defense.hp > grid->rows) {
    }
    return .25f;
}

float calculateCoinWeight(void) {
    if (player->defense.hp > grid->rows) {
    }
    return .5f;
}
