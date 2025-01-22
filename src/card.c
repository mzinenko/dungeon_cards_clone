#include "card.h"
#include <SDL2/SDL.h>

Card createEnemyCard(int attack, int defense) {
    return (Card){
        .type = CARD_ENEMY,
        .attack = attack,
        .defense = defense,
        .value = 0,
        .isVisible = 1
    };
}

Card createWeaponCard(int attack) {
    return (Card){
        .type = CARD_WEAPON,
        .attack = attack,
        .defense = 0,
        .value = 0,
        .isVisible = 1
    };
}

Card createShieldCard(int defense) {
    return (Card){
        .type = CARD_SHIELD,
        .attack = 0,
        .defense = defense,
        .value = 0,
        .isVisible = 1
    };
}

Card createPotionCard(int healing) {
    return (Card){
        .type = CARD_POTION,
        .attack = 0,
        .defense = 0,
        .value = healing,
        .isVisible = 1
    };
}

Card createCoinCard(int value) {
    return (Card){
        .type = CARD_COIN,
        .attack = 0,
        .defense = 0,
        .value = value,
        .isVisible = 1
    };
}

Card createKeyCard(void) {
    return (Card){
        .type = CARD_KEY,
        .attack = 0,
        .defense = 0,
        .value = 1,
        .isVisible = 1
    };
}

void getCardColor(const Card *card, SDL_Color *color) {
    switch (card->type) {
        case CARD_ENEMY:
            *color = (SDL_Color){255, 0, 0, 255};  // Red
            break;
        case CARD_WEAPON:
            *color = (SDL_Color){139, 69, 19, 255};  // Brown
            break;
        case CARD_SHIELD:
            *color = (SDL_Color){72, 61, 139, 255};  // Dark slate blue
            break;
        case CARD_POTION:
            *color = (SDL_Color){0, 255, 0, 255};  // Green
            break;
        case CARD_COIN:
            *color = (SDL_Color){255, 215, 0, 255};  // Gold
            break;
        case CARD_KEY:
            *color = (SDL_Color){255, 255, 0, 255};  // Yellow
            break;
        default:
            *color = (SDL_Color){128, 128, 128, 255};  // Gray
    }
}
