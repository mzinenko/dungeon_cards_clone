#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_ANIMATIONS 128

// Structure to represent a single animation
typedef struct {
    float duration;           // Total duration of the animation in seconds
    float elapsedTime;        // Elapsed time since the animation started
    bool isRunning;           // Whether the animation is currently active
    void (*onAnimationEnd)(); // Callback when animation ends
} Animation;

// Global animation manager
typedef struct {
    Animation animations[MAX_ANIMATIONS]; // Array of animations
    int count;                            // Number of active animations
} AnimationManager;

// Initialize the animation manager
void initAnimationManager(AnimationManager* manager) {
    manager->count = 0;
}

// Add a new animation to the manager
void addAnimation(AnimationManager* manager, float duration, void (*onAnimationEnd)()) {
    if (manager->count >= MAX_ANIMATIONS) {
        printf("Max animations reached. Cannot add more.\n");
        return;
    }

    Animation* animation = &manager->animations[manager->count++];
    animation->duration = duration;
    animation->elapsedTime = 0.0f;
    animation->isRunning = true;
    animation->onAnimationEnd = onAnimationEnd;
}

// Update all animations in the manager
void updateAnimations(AnimationManager* manager, float deltaTime) {
    for (int i = 0; i < manager->count; i++) {
        Animation* animation = &manager->animations[i];
        if (!animation->isRunning) continue;

        animation->elapsedTime += deltaTime;
        if (animation->elapsedTime >= animation->duration) {
            animation->isRunning = false;
            animation->elapsedTime = animation->duration; // Clamp to duration

            // Trigger callback if set
            if (animation->onAnimationEnd) {
                animation->onAnimationEnd();
            }

            // Remove animation by shifting others down
            for (int j = i; j < manager->count - 1; j++) {
                manager->animations[j] = manager->animations[j + 1];
            }
            manager->count--;
            i--; // Stay on the current index
        }
    }
}

// Example callback
void exampleCallback() {
    printf("Animation completed!\n");
}

// Example usage
int main() {
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window for demonstration
    SDL_Window* window = SDL_CreateWindow("Animation Manager",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    AnimationManager animationManager;
    initAnimationManager(&animationManager);

    // Add animations
    addAnimation(&animationManager, 2.0f, exampleCallback); // 2 seconds
    addAnimation(&animationManager, 3.0f, exampleCallback); // 3 seconds

    bool running = true;
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Convert to seconds
        lastTime = currentTime;

        // Update animations
        updateAnimations(&animationManager, deltaTime);

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // You can render animation-related visuals here

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
