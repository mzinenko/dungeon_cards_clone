#include "../inc/header.h"

void initCard(Card *card)
{
    card->id = NULL;
    card->type = CARD_NONE;
    card->rarity = RARITY_COMMON;

    card->attack.damage = 0;
    card->attack.range = 0;
    card->attack.ammo = 0;
    card->attack.critChance = 0.0f;
    card->attack.multiShotChance = 0.0f;

    card->defense.hp = 0;
    card->defense.maxHp = 0;
    card->defense.armor = 0;
    card->defense.armorType = ARMOR_PHYSICAL;
    card->defense.isInvulnerable = false;

    card->value.value = 0;
    card->value.multiplier = 1.0f;

    card->customData = NULL;
    card->texture = NULL;
}

void drawCardContent(Card *card, SDL_Rect cardRect)
{
    if (card->type == CARD_NONE)
        return;

    // Draw card background
    SDL_Color cardColor;
    getCardColor(card, &cardColor);
    SDL_SetRenderDrawColor(renderer, cardColor.r, cardColor.g, cardColor.b, cardColor.a);
    SDL_RenderFillRect(renderer, &cardRect);

    // Prepare text for card stats
    SDL_Color textColor = {255, 255, 255, 255};
    char statText[32];

    // Draw card stats as text
    if (card->attack.damage > 0)
    {
        snprintf(statText, sizeof(statText), "DMG: %d", card->attack.damage);
        renderText(statText,
                   cardRect.x + 10, cardRect.y + 10, textColor);
    }

    if (card->attack.ammo > 0)
    {
        snprintf(statText, sizeof(statText), "AMMO: %d", card->attack.ammo);
        renderText(statText,
                   cardRect.x + 10, cardRect.y + 30, textColor);
    }

    if (card->attack.range > 0)
    {
        snprintf(statText, sizeof(statText), "RNG: %d", card->attack.range);
        renderText(statText,
                   cardRect.x + 10, cardRect.y + 50, textColor);
    }

    if (card->defense.armor > 0 || (card->defense.armor == 0 && card->type == CARD_ENEMY))
    {
        snprintf(statText, sizeof(statText), "DEF: %d", card->defense.armor);
        renderText(statText,
                   cardRect.x + 10, cardRect.y + 70, textColor);
    }

    if (card->value.value > 0 || card->defense.hp > 0)
    {
        switch (card->type)
        {
        case CARD_POTION:
            snprintf(statText, sizeof(statText), "HEAL: %d", card->value.value);
            break;
        case CARD_ENEMY:
            snprintf(statText, sizeof(statText), "HP: %d", card->defense.hp);
            break;
        case CARD_COIN:
            snprintf(statText, sizeof(statText), "GOLD: %d", card->value.value);
            break;
        default:
            statText[0] = '\0';
        }
        if (statText[0])
        {
            renderText(statText,
                       cardRect.x + 10, cardRect.y + 90, textColor);
        }
    }

    if (isTriggered(card))
    {
        SDL_Color exclamationColor = {255, 0, 0, 255}; // Red color
        renderText("!", cardRect.x + cardRect.w - 15, cardRect.y + cardRect.h - 15, exclamationColor);
    }

    if (!card || !card->texture || !card->texture->texture)
        return;

    SDL_Rect spriteClip;
    const Texture *cardTexture = card->texture;

    static Uint32 lastFrameTime = 0;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastFrameTime > FRAME_DELAY && cardTexture->frameCount > 1)
    {
        card->texture->currentFrame = (card->texture->currentFrame + 1) % cardTexture->frameCount;
        lastFrameTime = currentTime;
    }

    if (cardTexture->alignment)
    { // Vertical alignment
        spriteClip.x = cardTexture->clipRect.x;
        spriteClip.y = cardTexture->clipRect.y + card->texture->currentFrame * (cardTexture->clipRect.h + cardTexture->frameGap);
        spriteClip.w = cardTexture->clipRect.w;
        spriteClip.h = cardTexture->clipRect.h;
    }
    else
    { // Horizontal alignment
        spriteClip.x = cardTexture->clipRect.x + card->texture->currentFrame * (cardTexture->clipRect.w + cardTexture->frameGap);
        spriteClip.y = cardTexture->clipRect.y;
        spriteClip.w = cardTexture->clipRect.w;
        spriteClip.h = cardTexture->clipRect.h;
    }

    float destW = spriteClip.w;
    float destH = spriteClip.h;

    // Render the animated sprite
    SDL_Rect destRect = {cardRect.x + (CARD_SIZE - 20 - destW), cardRect.y + 10, destW, destH};

    SDL_RenderCopy(renderer, cardTexture->texture, &spriteClip, &destRect);
}

Card *createEmptyCard(void)
{
    Card *card = malloc(sizeof(Card));

    *card = (Card){
        .type = CARD_NONE,
        .rarity = RARITY_COMMON,
        .attack = {0, 0, 0, 0.0f, 0.0f},
        .defense = {0, 0, 0, 0, 0, false},
        .value = {0, 0.0f},
        .id = NULL,
        .customData = NULL,
        .texture = NULL};

    return card;
}

Card *createCard(CardType type, CardRarity rarity)
{
    Card *card = malloc(sizeof(Card));
    memset(card, 0, sizeof(Card));

    card->type = type;
    card->rarity = rarity;

    // Generate rarity-based ranges
    float damageMin = RARITY_RANGES[card->rarity].damageMin;
    float damageMax = RARITY_RANGES[card->rarity].damageMax;
    float hpMin = RARITY_RANGES[card->rarity].hpMin;
    float hpMax = RARITY_RANGES[card->rarity].hpMax;
    float armorMin = RARITY_RANGES[card->rarity].armorMin;
    float armorMax = RARITY_RANGES[card->rarity].armorMax;

    // Type-specific generation
    switch (card->type)
    {
    case CARD_ENEMY:
        card->attack.damage = frandomInRange(damageMin, damageMax);
        card->attack.range = rand() % 3 + 1;
        card->defense.hp = frandomInRange(hpMin, hpMax);
        card->defense.maxHp = card->defense.hp;
        card->defense.armor = frandomInRange(armorMin, armorMax);
        card->defense.maxArmor = card->defense.armor;
        card->texture = &enemyTextures[rand() % enemyTexturesCount];
        break;
    case CARD_WEAPON:
        card->attack.damage = frandomInRange(damageMin, damageMax);
        card->attack.range = rand() % 3 + 1;
        card->attack.ammo = rand() % 3 + 1;
        card->attack.critChance = frandomInRange(0.05f, 0.3f);
        break;
    case CARD_SHIELD:
    {
        int shieldArmor = frandomInRange(armorMin, armorMax);
        card->defense.armor = (shieldArmor > 0 ? shieldArmor : 1);
        card->defense.maxArmor = card->defense.armor;
        break;
    }
    case CARD_POTION:
        card->value.value = frandomInRange(1, 5);
        card->value.multiplier = frandomInRange(1.0f, 2.0f);
        card->texture = &potionTextures[rand() % potionTexturesCount];
        break;
    case CARD_COIN:
        card->value.value = frandomInRange(1, 10);
        card->value.multiplier = frandomInRange(1.0f, 2.0f); // Add multiplier for gem calculation
        card->texture = &coinTextures[card->rarity];         // Use rarity for texture selection
        break;
    default:
        break;
    }

    // Optional: Generate unique ID
    char idBuffer[32];
    snprintf(idBuffer, sizeof(idBuffer), "%s_%d",
             cardTypeToString(card->type), rand() % 1000);
    card->id = mx_strdup(idBuffer);

    return card;
}

Card *generateCard(void)
{
    int weightCount = 0;
    CardWeight *weights = calculateCardWeights(&weightCount);

    // Select type and rarity
    CardType type = selectCardTypeByWeight(weights, weightCount);
    CardRarity rarity = selectRarityByWeight();

    return createCard(type, rarity);
}

void getCardColor(const Card *card, SDL_Color *color)
{
    if (card)
        *color = (SDL_Color){70, 70, 70, 255};
}

char *cardTypeToString(CardType type)
{
    switch (type)
    {
    case CARD_ENEMY:
        return "Enemy";
    case CARD_WEAPON:
        return "Weapon";
    case CARD_SHIELD:
        return "Shield";
    case CARD_POTION:
        return "Potion";
    case CARD_COIN:
        return "Coin";
    default:
        return "Unknown";
    }
}

bool isTriggered(const Card *card)
{
    return card->defense.armor != card->defense.maxArmor || card->defense.hp != card->defense.maxHp;
}

void freeCard(Card *card)
{
    if (card->id)
    {
        free(card->id);
        card->id = NULL;
    }
    if (card->texture)
    {
        SDL_DestroyTexture(card->texture->texture);
        card->texture = NULL;
    }
    if (card->customData)
    {
        free(card->customData);
        card->customData = NULL;
    }
}
