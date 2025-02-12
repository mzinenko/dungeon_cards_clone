#include "header.h"

// Function to calculate card weights based on player and grid state
CardWeight* calculateCardWeights(int* weightCount) {
    CardWeight* weights = malloc(sizeof(CardWeight) * CARD_COIN + 1);
    *weightCount = 0;

    // Base weight calculations
    weights[(*weightCount)++] = (CardWeight){ CARD_ENEMY, calculateEnemyWeight(player, grid) };
    weights[(*weightCount)++] = (CardWeight){ CARD_WEAPON, calculateWeaponWeight(player, grid) };
    weights[(*weightCount)++] = (CardWeight){ CARD_SHIELD, calculateShieldWeight(player, grid) };
    weights[(*weightCount)++] = (CardWeight){ CARD_POTION, calculatePotionWeight(player, grid) };
    weights[(*weightCount)++] = (CardWeight){ CARD_COIN, calculateCoinWeight(player, grid) };

    return weights;
}

// Weighted random selection
CardType selectCardTypeByWeight(CardWeight* weights, int weightCount) {
    // Calculate total weight
    float totalWeight = 0;
    for (int i = 0; i < weightCount; i++) {
        totalWeight += weights[i].weight;
    }

    // Generate random value
    float randomValue = ((float)rand() / (float)RAND_MAX) * totalWeight;

    // Select card type
    float currentSum = 0;
    for (int i = 0; i < weightCount; i++) {
        currentSum += weights[i].weight;
        if (randomValue <= currentSum) {
            return weights[i].type;
        }
    }

    // Fallback (should not happen)
    return CARD_NONE;
}

CardRarity selectRarityByWeight() {
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

// Stub weight calculation functions (to be implemented with game-specific logic)
float calculateEnemyWeight() {
    // Consider player health, current enemies, grid state
    if (player->defense.hp > grid->rows) {

    }
    return 1.0f;
}

float calculateWeaponWeight() {
    // Consider player's current weapon, attack stats
    if (player->defense.hp > grid->rows) {

    }
    return .75f;
}

float calculateShieldWeight() {
    // Consider player's current defense, health
    if (player->defense.hp > grid->rows) {

    }
    return .5f;
}

float calculatePotionWeight() {
    // Consider player's health, potential buffs
    if (player->defense.hp > grid->rows) {

    }
    return .25f;
}

float calculateCoinWeight() {
    // Consider player's current gold, game progression
    if (player->defense.hp > grid->rows) {

    }
    return .5f;
}
