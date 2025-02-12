#include "header.h"

void initStore() {
    store = malloc(sizeof(Store));
    // Initialize item list
    store->itemCount = 3;
    store->items = malloc(sizeof(StoreItem) * store->itemCount);
    store->selectedItem = 0;
    
    store->items[0] = (StoreItem){
        "Max HP Up",
        "Increase max HP by 2",
        100,
        5,
        upgradeMaxHP
    };
    
    store->items[1] = (StoreItem){
        "Attack Up",
        "Increase base attack by 1",
        150,
        3,
        upgradeAttack
    };
    
    store->items[2] = (StoreItem){
        "Defense Up",
        "Increase base defense by 1",
        150,
        3,
        upgradeDefense
    };
    
    // Initialize button positions
    store->buyButton = (SDL_Rect){
        WINDOW_WIDTH/2 - 60,
        WINDOW_HEIGHT - 100,
        120,
        40
    };
    
    store->backButton = (SDL_Rect){
        50,
        WINDOW_HEIGHT - 100,
        120,
        40
    };
}

void upgradeMaxHP() {
    progress->maxHpUpgrade++;
}

void upgradeAttack() {
    progress->attackUpgrade++;
}

void upgradeDefense() {
    progress->defenseUpgrade++;
}

int getUpgradeCost(const StoreItem *item) {
    int level = 0;
    
    // Determine current level based on item type
    if (item->applyUpgrade == upgradeMaxHP) level = progress->maxHpUpgrade;
    else if (item->applyUpgrade == upgradeAttack) level = progress->attackUpgrade;
    else if (item->applyUpgrade == upgradeDefense) level = progress->defenseUpgrade;
    return item->baseCost * (level + 1);
}

void destroyStore() {
    free(store->items);
}
