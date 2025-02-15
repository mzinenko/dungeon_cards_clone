#include "../inc/header.h"

void initTextures(void)
{
    static Texture _eventObserverTextures[] = {
        {"resource/public/e_obs_curse.bmp", NULL, {0, 0, 224, 240}, 15, 0, 0, false},
        {"resource/public/projectile_acid.bmp", NULL, {0, 0, 56, 32}, 12, 0, 0, true},
    };
    static Texture _enemyTextures[] = {
        {"resource/public/enemy_01.bmp", NULL, {0, 0, 32, 32}, 6, 0, 0, false},
        {"resource/public/enemy_02.bmp", NULL, {0, 0, 32, 32}, 6, 0, 0, false},
        {"resource/public/enemy_03.bmp", NULL, {0, 0, 32, 32}, 8, 0, 0, false},
        {"resource/public/enemy_04.bmp", NULL, {0, 0, 32, 32}, 6, 0, 0, false},
    };
    static Texture _coinTextures[] = {
        {"resource/public/coins_sheet.bmp", NULL, {0, 48, 24, 24}, 1, 0, 0, true},
        {"resource/public/coins_sheet.bmp", NULL, {24, 48, 24, 24}, 1, 0, 0, true},
        {"resource/public/coins_sheet.bmp", NULL, {48, 48, 24, 24}, 1, 0, 0, true},
        {"resource/public/coins_sheet.bmp", NULL, {72, 48, 24, 24}, 1, 0, 0, true},
    };
    static Texture _heroTextures[] = {
        {"resource/public/hero_guard.bmp", NULL, {0, 0, 40, 23}, 5, 0, 0, true},
        {"resource/public/hero_icon.bmp", NULL, {0, 0, 32, 32}, 1, 0, 0, true},
    };
    static Texture _potionTextures[] = {
        {"resource/public/potion_heal_1.bmp", NULL, {0, 0, 32, 32}, 1, 0, 0, true},
        {"resource/public/potion_heal_2.bmp", NULL, {0, 0, 32, 32}, 1, 0, 0, true},
    };
    static Texture _uiTextures[] = {
        {"resource/public/ui/ui_bg_menu.bmp", NULL, {0, 0, 1720, 976}, 1, 0, 0, true},
        {"resource/public/ui/ui_btn_settings.bmp", NULL, {0, 0, 248, 248}, 3, 0, 8, false},
        {"resource/public/ui/ui_btn_play.bmp", NULL, {0, 0, 248, 248}, 3, 0, 8, false},
        {"resource/public/ui/ui_btn_info.bmp", NULL, {0, 0, 248, 248}, 3, 0, 8, false},
        {"resource/public/ui/ui_banner_1.bmp", NULL, {0, 0, 174, 174}, 1, 0, 0, true},
        {"resource/public/ui/ui_banner_2.bmp", NULL, {0, 0, 174, 174}, 1, 0, 0, true},
        {"resource/public/ui/ui_guild_1.bmp", NULL, {0, 0, 480, 270}, 1, 0, 0, true},
        {"resource/public/ui/ui_guild_2.bmp", NULL, {0, 0, 480, 270}, 1, 0, 0, true},  
        {"resource/public/ui/ui_button_small.bmp", NULL, {0, 0, 45, 15}, 3, 0, 1, false},
        {"resource/public/ui/ui_bg_main.bmp", NULL, {0, 0, 0, 0}, 1, 0, 0, false},
        {"resource/public/ui/ui_grid.bmp", NULL, {0, 0, 0, 0}, 1, 0, 0, false }
    };

    eventObserverTextures = _eventObserverTextures;
    enemyTextures = _enemyTextures;
    coinTextures = _coinTextures;
    heroTextures = _heroTextures;
    potionTextures = _potionTextures;
    uiTextures = _uiTextures;

    eventObserverTexturesCount = sizeof(_eventObserverTextures) / sizeof(_eventObserverTextures[0]);
    enemyTexturesCount = sizeof(_enemyTextures) / sizeof(_enemyTextures[0]);
    coinTexturesCount = sizeof(_coinTextures) / sizeof(_coinTextures[0]);
    heroTexturesCount = sizeof(_heroTextures) / sizeof(_heroTextures[0]);
    potionTexturesCount = sizeof(_potionTextures) / sizeof(_potionTextures[0]);
    uiTexturesCount = sizeof(_uiTextures) / sizeof(_uiTextures[0]);
}

void loadTextures(void)
{
    // Load textures for eventObserverTextures array
    for (size_t i = 0; i < eventObserverTexturesCount; i++)
    {
        eventObserverTextures[i].texture = loadTexture(eventObserverTextures[i].texturePath);
        if (!eventObserverTextures[i].texture)
        {
            printf("Failed to load texture: %s\n", eventObserverTextures[i].texturePath);
        }
    }

    // Load textures for enemyTextures array
    for (size_t i = 0; i < enemyTexturesCount; i++)
    {
        enemyTextures[i].texture = loadTexture(enemyTextures[i].texturePath);
        if (!enemyTextures[i].texture)
        {
            printf("Failed to load texture: %s\n", enemyTextures[i].texturePath);
        }
    }

    // Load textures for coinTextures array
    for (size_t i = 0; i < coinTexturesCount; i++)
    {
        coinTextures[i].texture = loadTexture(coinTextures[i].texturePath);
        if (!coinTextures[i].texture)
        {
            printf("Failed to load texture: %s\n", coinTextures[i].texturePath);
        }
    }

    // Load textures for heroTextures array
    for (size_t i = 0; i < heroTexturesCount; i++)
    {
        heroTextures[i].texture = loadTexture(heroTextures[i].texturePath);
        if (!heroTextures[i].texture)
        {
            printf("Failed to load texture: %s\n", heroTextures[i].texturePath);
        }
    }

    // Load textures for potionTextures array
    for (size_t i = 0; i < potionTexturesCount; i++)
    {
        potionTextures[i].texture = loadTexture(potionTextures[i].texturePath);
        if (!potionTextures[i].texture)
        {
            printf("Failed to load texture: %s\n", potionTextures[i].texturePath);
        }
    }

    // Load textures for uiTextures array
    for (size_t i = 0; i < uiTexturesCount; i++)
    {
        uiTextures[i].texture = loadTexture(uiTextures[i].texturePath);
        if (!uiTextures[i].texture)
        {
            printf("Failed to load texture: %s\n", uiTextures[i].texturePath);
        }
    }

    printf("All textures loaded successfully!\n");
}

SDL_Texture *loadTexture(const char *filePath)
{
    SDL_Surface *surface = (SDL_Surface *)SDL_LoadBMP(filePath);
    if (!surface)
    {
        printf("Error loading image: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void destroyAllTextures(void)
{
    // Destroy textures for eventObserverTextures array
    for (size_t i = 0; i < eventObserverTexturesCount; i++)
    {
        if (eventObserverTextures[i].texture)
        {
            SDL_DestroyTexture(eventObserverTextures[i].texture);
            eventObserverTextures[i].texture = NULL;
        }
    }

    // Destroy textures for enemyTextures array
    for (size_t i = 0; i < enemyTexturesCount; i++)
    {
        if (enemyTextures[i].texture)
        {
            SDL_DestroyTexture(enemyTextures[i].texture);
            enemyTextures[i].texture = NULL;
        }
    }

    // Destroy textures for coinTextures array
    for (size_t i = 0; i < coinTexturesCount; i++)
    {
        if (coinTextures[i].texture)
        {
            SDL_DestroyTexture(coinTextures[i].texture);
            coinTextures[i].texture = NULL;
        }
    }

    // Destroy textures for heroTextures array
    for (size_t i = 0; i < heroTexturesCount; i++)
    {
        if (heroTextures[i].texture)
        {
            SDL_DestroyTexture(heroTextures[i].texture);
            heroTextures[i].texture = NULL;
        }
    }

    // Destroy textures for potionTextures array
    for (size_t i = 0; i < potionTexturesCount; i++)
    {
        if (potionTextures[i].texture)
        {
            SDL_DestroyTexture(potionTextures[i].texture);
            potionTextures[i].texture = NULL;
        }
    }

    // Destroy textures for uiTextures array
    for (size_t i = 0; i < uiTexturesCount; i++)
    {
        if (uiTextures[i].texture)
        {
            SDL_DestroyTexture(uiTextures[i].texture);
            uiTextures[i].texture = NULL;
        }
    }

    printf("All textures destroyed successfully!\n");
}
