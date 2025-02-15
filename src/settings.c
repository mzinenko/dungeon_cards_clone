#include "../inc/header.h"

void initSettingsUI(void) {
    settingsUI = malloc(sizeof(SettingsUI));
    if (!settingsUI) {
        printf("Failed to allocate memory for settings UI\n");
        return;
    }
    
    int centerX = VIRTUAL_WIDTH / 2;
    int centerY = VIRTUAL_HEIGHT / 2;
    
    // Music volume slider
    settingsUI->musicSlider = (SDL_Rect){
        centerX - 100,
        centerY - 80,
        200,
        20
    };
    
    // Effects volume slider
    settingsUI->effectsSlider = (SDL_Rect){
        centerX - 100,
        centerY - 20,
        200,
        20
    };
    
    // Music handle - position based on current volume
    float musicPercentage = audioManager->musicVolume / 128.0f;
    settingsUI->musicHandle = (SDL_Rect){
        settingsUI->musicSlider.x + (int)(musicPercentage * (settingsUI->musicSlider.w - 20)),
        settingsUI->musicSlider.y - 5,
        20,
        30
    };
    
    // Effects handle - position based on current volume
    float effectsPercentage = audioManager->soundVolume / 128.0f;
    settingsUI->effectsHandle = (SDL_Rect){
        settingsUI->effectsSlider.x + (int)(effectsPercentage * (settingsUI->effectsSlider.w - 20)),
        settingsUI->effectsSlider.y - 5,
        20,
        30
    };
    
    settingsUI->muteButton = (SDL_Rect){
        centerX - 60,
        centerY + 20,
        120,
        30
    };
    
    settingsUI->backButton = (SDL_Rect){
        centerX - 60,
        centerY + 70,
        120,
        30
    };
    
    // Initialize states
    settingsUI->isDraggingMusic = false;
    settingsUI->isDraggingEffects = false;
    settingsUI->isMusicHovered = false;
    settingsUI->isEffectsHovered = false;
    settingsUI->isMuteHovered = false;
    settingsUI->isBackHovered = false;
}

void drawSettingsUI(void) {
    // Draw semi-transparent background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, NULL);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    SDL_Color textColor = {255, 255, 255, 255};
    renderText("Audio Settings",
        VIRTUAL_WIDTH / 2 - 60,
        VIRTUAL_HEIGHT / 4,
        textColor
    );
    
    // Draw music volume text and slider
    renderText("Music Volume",
        settingsUI->musicSlider.x,
        settingsUI->musicSlider.y - 25,
        textColor
    );
    
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &settingsUI->musicSlider);
    
    SDL_Rect musicFilled = settingsUI->musicSlider;
    musicFilled.w = settingsUI->musicHandle.x - settingsUI->musicSlider.x + 10;
    SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    SDL_RenderFillRect(renderer, &musicFilled);
    
    // Draw effects volume text and slider
    renderText("Effects Volume",
        settingsUI->effectsSlider.x,
        settingsUI->effectsSlider.y - 25,
        textColor
    );
    
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &settingsUI->effectsSlider);
    
    SDL_Rect effectsFilled = settingsUI->effectsSlider;
    effectsFilled.w = settingsUI->effectsHandle.x - settingsUI->effectsSlider.x + 10;
    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    SDL_RenderFillRect(renderer, &effectsFilled);
    
    // Draw handles
    SDL_SetRenderDrawColor(renderer, 
        settingsUI->isMusicHovered ? 150 : 100,
        settingsUI->isMusicHovered ? 150 : 100,
        settingsUI->isMusicHovered ? 250 : 200,
        255
    );
    SDL_RenderFillRect(renderer, &settingsUI->musicHandle);
    
    SDL_SetRenderDrawColor(renderer, 
        settingsUI->isEffectsHovered ? 150 : 100,
        settingsUI->isEffectsHovered ? 250 : 200,
        settingsUI->isEffectsHovered ? 150 : 100,
        255
    );
    SDL_RenderFillRect(renderer, &settingsUI->effectsHandle);
    
    // Draw current values
    char volumeText[32];
    snprintf(volumeText, sizeof(volumeText), "%d%%", 
        (int)((audioManager->musicVolume / 128.0f) * 100));
    renderText(volumeText,
        settingsUI->musicSlider.x + settingsUI->musicSlider.w + 10,
        settingsUI->musicSlider.y,
        textColor
    );
    
    snprintf(volumeText, sizeof(volumeText), "%d%%", 
        (int)((audioManager->soundVolume / 128.0f) * 100));
    renderText(volumeText,
        settingsUI->effectsSlider.x + settingsUI->effectsSlider.w + 10,
        settingsUI->effectsSlider.y,
        textColor
    );
    
    drawButton(
        audioManager->isMuted ? "Unmute All" : "Mute All",
        settingsUI->muteButton,
        settingsUI->isMuteHovered
    );
    
    drawButton("Back", settingsUI->backButton, settingsUI->isBackHovered);
}

void handleSettingsInput(void) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);
    
    // Update hover states
    settingsUI->isMusicHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->musicHandle);
    settingsUI->isEffectsHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->effectsHandle);
    settingsUI->isMuteHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->muteButton);
    settingsUI->isBackHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->backButton);
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            // Handle volume sliders drag start
            if (settingsUI->isMusicHovered) {
                settingsUI->isDraggingMusic = true;
            }
            if (settingsUI->isEffectsHovered) {
                settingsUI->isDraggingEffects = true;
            }
            if (settingsUI->isMuteHovered) {
                toggleMute();
                if (!audioManager->isMuted) {
                    playSound("resource/music/click.wav");
                }
            }
            if (settingsUI->isBackHovered) {
                playSound("resource/music/click.wav");
                gameContext->currentState = STATE_MAIN_MENU;
                cleanupSettingsUI();
            }
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        settingsUI->isDraggingMusic = false;
        settingsUI->isDraggingEffects = false;
        
        // Play test sound when releasing either slider
        if (!audioManager->isMuted) {
            playSound("resource/music/click.wav");
        }
    }
    else if (event.type == SDL_MOUSEMOTION) {
        if (settingsUI->isDraggingMusic) {
            // Update music volume
            int minX = settingsUI->musicSlider.x;
            int maxX = settingsUI->musicSlider.x + settingsUI->musicSlider.w - 20;
            int newX = virtualMouseX - 10;
            
            newX = (newX < minX) ? minX : (newX > maxX) ? maxX : newX;
            settingsUI->musicHandle.x = newX;
            
            float percentage = (float)(newX - minX) / (maxX - minX);
            setMusicVolume((int)(percentage * 128));
        }
        
        if (settingsUI->isDraggingEffects) {
            // Update effects volume
            int minX = settingsUI->effectsSlider.x;
            int maxX = settingsUI->effectsSlider.x + settingsUI->effectsSlider.w - 20;
            int newX = virtualMouseX - 10;
            
            newX = (newX < minX) ? minX : (newX > maxX) ? maxX : newX;
            settingsUI->effectsHandle.x = newX;
            
            float percentage = (float)(newX - minX) / (maxX - minX);
            setSoundVolume((int)(percentage * 128));
        }
    }
}

void cleanupSettingsUI(void) {
    if (settingsUI) {
        free(settingsUI);
        settingsUI = NULL;
    }
}