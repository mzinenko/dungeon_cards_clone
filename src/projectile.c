#include "../inc/header.h"

void renderProjectiles(void) {
    if (player->animation.isAnimating || 
        (eventObserver && eventObserver->isAnimating))
        return;

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

    for (int i = 0; i < projectileCount; i++) {
        if (!projectiles[i].active)
            continue;

        double angle = atan2(projectiles[i].vy, projectiles[i].vx) * 180.0 / M_PI;

        if (!projectiles[i].texture || !projectiles[i].texture->texture)
            return;

        SDL_Rect spriteClip;
        const Texture *projectileTexture = projectiles[i].texture;

        static Uint32 lastFrameTime = 0;
        Uint32 currentTime = SDL_GetTicks();
        
        if (currentTime - lastFrameTime > FRAME_DELAY && 
            projectileTexture->frameCount > 1) {
            projectiles[i].texture->currentFrame = 
                (projectiles[i].texture->currentFrame + 1) % 
                projectileTexture->frameCount;
            lastFrameTime = currentTime;
        }

        if (projectileTexture->alignment) {
            spriteClip.x = projectileTexture->clipRect.x;
            spriteClip.y = projectileTexture->clipRect.y + 
                projectiles[i].texture->currentFrame * 
                (projectileTexture->clipRect.h + projectileTexture->frameGap);
            spriteClip.w = projectileTexture->clipRect.w;
            spriteClip.h = projectileTexture->clipRect.h;
        } else {
            spriteClip.x = projectileTexture->clipRect.x + 
                projectiles[i].texture->currentFrame * 
                (projectileTexture->clipRect.w + projectileTexture->frameGap);
            spriteClip.y = projectileTexture->clipRect.y;
            spriteClip.w = projectileTexture->clipRect.w;
            spriteClip.h = projectileTexture->clipRect.h;
        }

        float destW = PROJECTILE_VIRTUAL_WIDTH;
        float destH = PROJECTILE_VIRTUAL_HEIGHT;
        
        SDL_Rect destRect = {
            projectiles[i].x - destW / 2,
            projectiles[i].y - destH / 2,
            destW,
            destH
        };

        SDL_RenderCopyEx(
            renderer,
            projectileTexture->texture,
            &spriteClip,
            &destRect,
            angle + 180,
            NULL,
            SDL_FLIP_NONE
        );
    }
}

void updateProjectiles(Animation *animation, float deltaTime) {
    if (projectileCount == 0) {
        animation->isRunning = false;
        return;
    }

    for (int i = 0; i < projectileCount; i++) {
        if (!projectiles[i].active)
            continue;

        projectiles[i].elapsedTime += deltaTime;

        float progress = projectiles[i].elapsedTime / projectiles[i].duration;
        if (progress >= 1.0f)
            progress = 1.0f;

        if (progress < 0.5f) {
            progress = 4.0f * progress * progress * progress;
        }
        else {
            float t = (progress - 1.0f);
            progress = 1.0f + 4.0f * t * t * t;
        }

        projectiles[i].x = projectiles[i].targetX * progress + (1.0f - progress) * (projectiles[i].x - projectiles[i].vx * deltaTime);
        projectiles[i].y = projectiles[i].targetY * progress + (1.0f - progress) * (projectiles[i].y - projectiles[i].vy * deltaTime);

        if (progress >= 1.0f) {
            projectiles[i].active = false;

            for (int j = i; j < projectileCount - 1; j++) {
                projectiles[j] = projectiles[j + 1];
            }

            projectileCount--;
            if (projectileCount > 0) {
                Projectile *temp = realloc(projectiles, projectileCount * sizeof(Projectile));
                if (temp == NULL) {
                    fprintf(stderr, "Failed to allocate memory after removing projectile\n");
                    return;
                }
                projectiles = temp;
            }
            else {
                free(projectiles);
                projectiles = NULL;
            }
            i--;
        }
    }
}

void shootProjectile(float startX, float startY, float targetX, float targetY, 
                    float durationMs) {
    Projectile *temp = realloc(projectiles, 
                              (projectileCount + 1) * sizeof(Projectile));
    if (temp == NULL) {
        fprintf(stderr, "Failed to allocate memory for new projectile\n");
        return;
    }
    projectiles = temp;

    Projectile *newProjectile = &projectiles[projectileCount];
    float dx = targetX - startX;
    float dy = targetY - startY;
    float distance = sqrtf(dx * dx + dy * dy);

    float durationSeconds = durationMs / 1000.0f;
    float speed = distance / durationSeconds;

    newProjectile->vx = (dx / distance) * speed;
    newProjectile->vy = (dy / distance) * speed;
    newProjectile->x = startX;
    newProjectile->y = startY;
    newProjectile->targetX = targetX;
    newProjectile->targetY = targetY;
    newProjectile->active = true;
    newProjectile->elapsedTime = 0.0f;
    newProjectile->duration = durationSeconds;
    newProjectile->texture = &eventObserverTextures[1];

    projectileCount++;
}
