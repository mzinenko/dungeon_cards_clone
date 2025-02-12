#include "header.h"

// Function to initialize faction upgrades
void initFactionUpgrades(Faction* faction) {
    const FactionUpgrade* sourceUpgrades = 
        (faction->type == FACTION_VANGUARD) ? vanguardUpgrades : crimsonUpgrades;
    
    int count = 0;
    while(count < MAX_FACTION_UPGRADES && sourceUpgrades[count].name != NULL) {
        faction->upgrades[count] = sourceUpgrades[count];
        count++;
    }
    faction->upgradeCount = count;
}

// Function to handle upgrade purchase
bool purchaseUpgrade(Faction* faction, int upgradeIndex) {
    if (!faction || upgradeIndex >= faction->upgradeCount) return false;

    printf("Purchase upgrade: %s\n", faction->upgrades[upgradeIndex].name);

    FactionUpgrade* upgrade = &faction->upgrades[upgradeIndex];
    
    // Check requirements
    if (faction->playerRank < upgrade->requiredRank) return false;
    if (upgrade->currentLevel >= upgrade->maxLevel) return false;
    
    // Calculate actual cost based on current level
    int goldCost = upgrade->baseCost * (upgrade->currentLevel + 1);
    if (progress->totalCoins < goldCost) return false;

    // Check gem requirements
    for (int i = 0; i < 5; i++) {
        if (upgrade->gemCosts[i] > 0 && progress->gems[i] < upgrade->gemCosts[i]) {
            return false;
        }
    }

    // Apply costs
    progress->totalCoins -= goldCost;
    for (int i = 0; i < 5; i++) {
        progress->gems[i] -= upgrade->gemCosts[i];
    }
    
    upgrade->currentLevel++;
    return true;
}
