#include "../inc/header.h"

void renderEventObserver(void) {
    if (!eventObserver || !eventObserver->texture || !eventObserver->active)
        return;

    SDL_Rect spriteClip;
    Texture *eventObserverTexture = eventObserver->texture;

    static Uint32 lastFrameTime = 0;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastFrameTime > FRAME_DELAY && eventObserverTexture->frameCount > 1) {
        eventObserverTexture->currentFrame = 
            (eventObserverTexture->currentFrame + 1) % eventObserverTexture->frameCount;
        lastFrameTime = currentTime;
    }

    // Calculate frame clip based on alignment
    if (eventObserverTexture->alignment) {
        spriteClip.x = eventObserverTexture->clipRect.x;
        spriteClip.y = eventObserverTexture->clipRect.y + 
            eventObserverTexture->currentFrame * 
            (eventObserverTexture->clipRect.h + eventObserverTexture->frameGap);
        spriteClip.w = eventObserverTexture->clipRect.w;
        spriteClip.h = eventObserverTexture->clipRect.h;
    } else {
        spriteClip.x = eventObserverTexture->clipRect.x + 
            eventObserverTexture->currentFrame * 
            (eventObserverTexture->clipRect.w + eventObserverTexture->frameGap);
        spriteClip.y = eventObserverTexture->clipRect.y;
        spriteClip.w = eventObserverTexture->clipRect.w;
        spriteClip.h = eventObserverTexture->clipRect.h;
    }

    // Scale dimensions to virtual resolution
    float destW = EVENT_OBSERVER_VIRTUAL_WIDTH;
    float destH = EVENT_OBSERVER_VIRTUAL_HEIGHT;

    SDL_Rect destRect = {
        eventObserver->x,
        eventObserver->y,
        destW,
        destH
    };

    SDL_RenderCopy(renderer, eventObserverTexture->texture, &spriteClip, &destRect);
}

void eventTurn(void) {
    EventType eventType = randomInRange(0, 1);

    eventObserver = malloc(sizeof(EventObserver));
    if (!eventObserver) {
        fprintf(stderr, "Failed to allocate memory for EventObserver\n");
        return;
    }

    eventObserver->texture = &eventObserverTextures[0];

    // Calculate positions in virtual coordinates
    float gridStartX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2;
    eventObserver->x = gridStartX + GRID_VIRTUAL_SIZE - EVENT_OBSERVER_VIRTUAL_WIDTH;
    eventObserver->y = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2;
    eventObserver->targetX = eventObserver->x + EVENT_OBSERVER_VIRTUAL_WIDTH;
    eventObserver->targetY = eventObserver->y;
    eventObserver->vx = 50.0f; // Adjusted for virtual scale
    eventObserver->vy = 0;
    eventObserver->active = true;

    // Create animation
    Animation *eventObserverAnimation = malloc(sizeof(Animation));
    if (!eventObserverAnimation) {
        fprintf(stderr, "Failed to allocate memory for eventObserverAnimation\n");
        free(eventObserver);
        return;
    }

    EventType *eventTypePtr = malloc(sizeof(EventType));
    if (!eventTypePtr) {
        fprintf(stderr, "Failed to allocate memory for EventType\n");
        free(eventObserverAnimation);
        free(eventObserver);
        return;
    }
    *eventTypePtr = eventType;

    *eventObserverAnimation = (Animation){
        .updateAnimation = updateEventObserver,
        .onAnimationEnd = triggerEvent,
        .isRunning = true,
        .userData = eventTypePtr,
    };

    addAnimation(eventObserverAnimation);
}

void updateEventObserver(Animation *animation, float deltaTime) {
    if (!eventObserver || !eventObserver->active || !animation)
        return;

    // Update position in virtual coordinates
    eventObserver->x += eventObserver->vx * deltaTime;
    eventObserver->y += eventObserver->vy * deltaTime;

    if ((eventObserver->x >= eventObserver->targetX && eventObserver->vx > 0) || 
        (eventObserver->x <= eventObserver->targetX && eventObserver->vx < 0)) {
        eventObserver->vx = 0;
        eventObserver->vy = 0;
        animation->isRunning = false;
    }
}

void triggerEvent(Animation *animation)
{
    EventType eventType = *((EventType *)animation->userData);

    // Free the allocated memory for eventType
    free(animation->userData);
    animation->userData = NULL;

    Animation *eventAnimation = (Animation *)malloc(sizeof(Animation));
    if (!eventAnimation)
    {
        fprintf(stderr, "Failed to allocate memory for animation.\n");
        return;
    }

    EventType *eventTypePtr = malloc(sizeof(EventType));
    if (!eventTypePtr)
    {
        fprintf(stderr, "Failed to allocate memory for EventType.\n");
        free(eventObserver); // Free the observer if allocation fails
        return;
    }
    *eventTypePtr = eventType;

    *eventAnimation = (Animation){
        .updateAnimation = updateProjectiles, // Update logic for the event projectiles
        .onAnimationEnd = eventTurnResults,   // Results of the event turn after animation
        .isRunning = true,
        .userData = eventTypePtr, // Attach the event observer
    };

    switch (eventType)
    {
    case EVENT_GOLDMANIA:
    {
        for (int i = 0; i < grid->rows; i++)
        {
            for (int j = 0; j < grid->cols; j++)
            {
                if (grid->cells[i][j].type == CARD_COIN)
                {
                    float targetX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2 + (j + 0.5f) * CARD_VIRTUAL_SIZE;
                    float targetY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2 + (i + 0.5f) * CARD_VIRTUAL_SIZE;

                    shootProjectile(eventObserver->targetX, eventObserver->targetY, targetX, targetY, 1000);
                }
            }
        }
    }
    break;

    case EVENT_PLAGUE:
    {
        for (int i = 0; i < grid->rows; i++)
        {
            for (int j = 0; j < grid->cols; j++)
            {
                if (grid->cells[i][j].type == CARD_ENEMY)
                {
                    float targetX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2 + (j + 0.5f) * CARD_VIRTUAL_SIZE;
                    float targetY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2 + (i + 0.5f) * CARD_VIRTUAL_SIZE;

                    shootProjectile(eventObserver->targetX, eventObserver->targetY, targetX, targetY, 1000);
                }
            }
        }

        float targetX = (VIRTUAL_WIDTH - GRID_VIRTUAL_SIZE) / 2 + player->x * CARD_VIRTUAL_SIZE + CARD_VIRTUAL_SIZE / 2;
        float targetY = (VIRTUAL_HEIGHT - GRID_VIRTUAL_SIZE) / 2 + player->y * CARD_VIRTUAL_SIZE + CARD_VIRTUAL_SIZE / 2;

        shootProjectile(eventObserver->targetX, eventObserver->targetY, targetX, targetY, 1000);
    }
    break;

    default:
        break;
    }

    printf("projectiles length = %d event type = %d\n", projectileCount, eventType);

    addAnimation(eventAnimation);
}

void eventTurnResults(Animation *animation)
{
    EventType eventType = *((EventType *)animation->userData);

    switch (eventType)
    {
    case EVENT_GOLDMANIA:
    {
        for (int i = 0; i < grid->rows; i++)
        {
            for (int j = 0; j < grid->cols; j++)
            {
                if (grid->cells[i][j].type == CARD_COIN)
                {
                    grid->cells[i][j].value.value *= 2;
                }
            }
        }
    }
    break;

    case EVENT_PLAGUE:
    {
        // Apply effects to the enemies
        for (int i = 0; i < grid->rows; i++)
        {
            for (int j = 0; j < grid->cols; j++)
            {
                if (grid->cells[i][j].type == CARD_ENEMY)
                {
                    grid->cells[i][j].defense.hp -= 2;
                    grid->cells[i][j].defense.maxHp -= 2;

                    if (grid->cells[i][j].defense.hp <= 0)
                    {
                        grid->cells[i][j] = *createCard(CARD_COIN, grid->cells[i][j].rarity);
                    }
                }
            }
        }

        // Apply effects to the player
        player->defense.hp -= 6;

        // Check if the player is still alive
        if (player->defense.hp <= 0)
        {
            player->isAlive = false;
            gameContext->score = player->gold;
            gameContext->currentState = STATE_GAME_OVER;
        }
    }
    break;

    default:
        break;
    }

    Animation *eventObserverBackAnimation = (Animation *)malloc(sizeof(Animation));
    if (!eventObserverBackAnimation)
    {
        fprintf(stderr, "Failed to allocate memory for eventObserverBackAnimation.\n");
        return;
    }

    eventObserver->targetX = eventObserver->x - eventObserver->texture->clipRect.w;
    eventObserver->vx = -100.0f;

    *eventObserverBackAnimation = (Animation){
        .updateAnimation = updateEventObserver,
        .onAnimationEnd = freeObserver,
        .isRunning = false,
        .userData = eventObserver,
    };

    addAnimation(eventObserverBackAnimation);
}

void freeObserver(Animation *animation)
{
    if (!animation)
        return;

    free(eventObserver);
    eventObserver = NULL;
}
