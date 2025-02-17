#include "../inc/header.h"

void initFactionUpgrades(Faction *faction) {
    printf("Initializing faction upgrades...\n");
    const FactionUpgrade *sourceUpgrades =
        (faction->type == FACTION_VANGUARD) ? vanguardUpgrades : crimsonUpgrades;

    int count = 0;
    while (count < MAX_FACTION_UPGRADES && sourceUpgrades[count].name != NULL)
    {
        faction->upgrades[count] = sourceUpgrades[count];
        count++;
    }
    faction->upgradeCount = count;
}

bool purchaseUpgrade(Faction *faction, int upgradeIndex) {
    if (!faction || upgradeIndex >= faction->upgradeCount)
        return false;

    printf("Purchase upgrade: %s\n", faction->upgrades[upgradeIndex].name);

    FactionUpgrade *upgrade = &faction->upgrades[upgradeIndex];

    if (faction->playerRank < upgrade->requiredRank)
        return false;
    if (upgrade->currentLevel >= upgrade->maxLevel)
        return false;

    int goldCost = upgrade->baseCost * (upgrade->currentLevel + 1);
    if (progress->totalCoins < goldCost)
        return false;

    for (int i = 0; i < 5; i++) {
        if (upgrade->gemCosts[i] > 0 && progress->gems[i] < upgrade->gemCosts[i]) {
            return false;
        }
    }

    progress->totalCoins -= goldCost;
    for (int i = 0; i < 5; i++) {
        progress->gems[i] -= upgrade->gemCosts[i];
    }

    upgrade->currentLevel++;
    return true;
}

void applyStartUpgrades(void) {
    for (int i = 0; i <  progress->vanguardUpgrades.count; i++) {
        int upgradeId = progress->vanguardUpgrades.upgrades[i].upgradeId;
        if (upgradeId >= 0 && upgradeId < vanguardFaction->upgradeCount) {
            if (vanguardFaction->upgrades[upgradeId].applyOn == APPLY_ON_START && vanguardFaction->upgrades[upgradeId].applyEffect != NULL) {
                vanguardFaction->upgrades[upgradeId].applyEffect(vanguardFaction->upgrades[upgradeId].currentLevel);
            }
        }
    }

    for (int i = 0; i < progress->crimsonUpgrades.count; i++) {
        int upgradeId = progress->crimsonUpgrades.upgrades[i].upgradeId;
        if (upgradeId >= 0 && upgradeId < crimsonFaction->upgradeCount) {
            if (crimsonFaction->upgrades[upgradeId].applyOn == APPLY_ON_START && crimsonFaction->upgrades[upgradeId].applyEffect != NULL) {
                crimsonFaction->upgrades[upgradeId].applyEffect(crimsonFaction->upgrades[upgradeId].currentLevel);
            }
        }
    }
}
