#include "../inc/header.h"

void initSettingsUI(void) {
    settingsUI = malloc(sizeof(SettingsUI));
    
    // Calculate positions based on virtual resolution
    int centerX = VIRTUAL_WIDTH / 2;
    int centerY = VIRTUAL_HEIGHT / 2;
    
    // Volume slider
    settingsUI->volumeSlider = (SDL_Rect){
        centerX - 100,
        centerY - 50,
        200,
        20
    };
    
    // Volume handle
    float volumePercentage = audioManager->musicVolume / 128.0f;
    settingsUI->volumeHandle = (SDL_Rect){
        settingsUI->volumeSlider.x + (int)(volumePercentage * settingsUI->volumeSlider.w),
        settingsUI->volumeSlider.y - 5,
        20,
        30
    };
    
    // Mute button
    settingsUI->muteButton = (SDL_Rect){
        centerX - 60,
        centerY + 20,
        120,
        30
    };
    
    // Back button
    settingsUI->backButton = (SDL_Rect){
        centerX - 60,
        centerY + 70,
        120,
        30
    };
    
    settingsUI->isDraggingVolume = false;
    settingsUI->isVolumeHovered = false;
    settingsUI->isMuteHovered = false;
    settingsUI->isBackHovered = false;
}

void drawSettingsUI(void) {
    // Draw background
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    
    // Draw title
    SDL_Color textColor = {255, 255, 255, 255};
    renderText("Settings",
        VIRTUAL_WIDTH / 2 - 40,
        VIRTUAL_HEIGHT / 4,
        textColor
    );
    
    // Draw volume slider background
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &settingsUI->volumeSlider);
    
    // Draw volume text
    char volumeText[32];
    snprintf(volumeText, sizeof(volumeText), "Volume: %d%%", 
        (int)((audioManager->musicVolume / 128.0f) * 100));
    renderText(volumeText,
        settingsUI->volumeSlider.x,
        settingsUI->volumeSlider.y - 20,
        textColor
    );
    
    // Draw volume handle
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &settingsUI->volumeHandle);
    
    // Draw buttons
    drawButton(
        audioManager->isMuted ? "Unmute" : "Mute",
        settingsUI->muteButton,
        settingsUI->isMuteHovered
    );
    
    drawButton("Back", settingsUI->backButton, settingsUI->isBackHovered);
}

void handleSettingsInput(void) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Convert window coordinates to virtual coordinates
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);
    
    // Update hover states
    settingsUI->isVolumeHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->volumeHandle);
    settingsUI->isMuteHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->muteButton);
    settingsUI->isBackHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->backButton);
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            // Handle volume slider drag start
            if (settingsUI->isVolumeHovered) {
                settingsUI->isDraggingVolume = true;
            }
            
            // Handle mute button
            if (settingsUI->isMuteHovered) {
                toggleMute();
            }
            
            // Handle back button
            if (settingsUI->isBackHovered) {
                gameContext->currentState = STATE_MAIN_MENU;
            }
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            settingsUI->isDraggingVolume = false;
        }
    }
    else if (event.type == SDL_MOUSEMOTION) {
        if (settingsUI->isDraggingVolume) {
            // Calculate new volume based on slider position
            int sliderX = virtualMouseX - settingsUI->volumeSlider.x;
            sliderX = sliderX < 0 ? 0 : sliderX;
            sliderX = sliderX > settingsUI->volumeSlider.w ? 
                settingsUI->volumeSlider.w : sliderX;
            
            float percentage = (float)sliderX / settingsUI->volumeSlider.w;
            int newVolume = (int)(percentage * 128);
            
            setMusicVolume(newVolume);
            
            // Update handle position
            settingsUI->volumeHandle.x = settingsUI->volumeSlider.x + sliderX;
        }
    }
}

void cleanupSettingsUI(void) {
    if (settingsUI) {
        free(settingsUI);
        settingsUI = NULL;
    }
}
