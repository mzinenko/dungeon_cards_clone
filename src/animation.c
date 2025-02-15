#include "../inc/header.h"

void initAnimationManager(void) {
    animationManager = malloc(sizeof(AnimationManager));

    animationManager->count = 0;
}

// Add a new animation to the manager
void addAnimation(Animation *animation) {
    printf("here");
    if (animationManager->count >= MAX_ANIMATIONS)
    {
        printf("Max animations reached. Cannot add more.\n");
        return;
    }

    animationManager->animations[animationManager->count++] = animation;
    animation->isRunning = true;
}

// Update all animations in the animationManager
void updateAnimations(float deltaTime) {
    for (int i = 0; i < animationManager->count; i++) {
        Animation *animation = animationManager->animations[i];
        if (!animation->isRunning)
            continue;
            
        animation->updateAnimation(animation, deltaTime);
        if (!animation->isRunning && animation->onAnimationEnd) {
            animation->onAnimationEnd(animation);
            for (int j = i; j < animationManager->count - 1; j++) {
                animationManager->animations[j] = animationManager->animations[j + 1];
            }
            animationManager->count--;
            i--;
        }
    }
}

bool isAnyAnimationRunning(void) {
    for (int i = 0; i < animationManager->count; i++) {
        if (animationManager->animations[i]->isRunning) {
            return true;
        }
    }
    return false;
}
