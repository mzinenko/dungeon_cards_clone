#ifndef CARD_H
#define CARD_H

#include <SDL2/SDL.h>

typedef enum {
    CARD_NONE,
    CARD_ENEMY,
    CARD_WEAPON,
    CARD_SHIELD,
    CARD_POTION,
    CARD_COIN,
    CARD_KEY
} CardType;

typedef struct {
    CardType type;
    int attack;     // For enemies and weapons
    int defense;    // For enemies and shields
    int value;      // For potions (healing), coins (gold value), keys (count)
    int isVisible;  // Whether card is revealed
} Card;

typedef struct {
    float startX;
    float startY;
    float currentX;
    float currentY;
    float targetX;
    float targetY;
    Card card;
    float animationProgress; // 0.0 1.0
} AnimatedCard;

// Card creation functions
Card createEnemyCard(int attack, int defense);
Card createWeaponCard(int attack);
Card createShieldCard(int defense);
Card createPotionCard(int healing);
Card createCoinCard(int value);
Card createKeyCard(void);

// Get card color for rendering
void getCardColor(const Card *card, SDL_Color *color);

#endif // CARD_H
