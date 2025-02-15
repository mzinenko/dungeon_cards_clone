#include "../inc/header.h"

void initSettingsUI(void) {
    settingsUI = malloc(sizeof(SettingsUI));
    
    int centerX = VIRTUAL_WIDTH / 2;
    int centerY = VIRTUAL_HEIGHT / 2;
    
    settingsUI->volumeSlider = (SDL_Rect){
        centerX - 100,
        centerY - 50,
        200,
        20
    };
    
    float volumePercentage = audioManager->musicVolume / 128.0f;
    settingsUI->volumeHandle = (SDL_Rect){
        settingsUI->volumeSlider.x + (int)(volumePercentage * settingsUI->volumeSlider.w),
        settingsUI->volumeSlider.y - 5,
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
    
    settingsUI->isDraggingVolume = false;
    settingsUI->isVolumeHovered = false;
    settingsUI->isMuteHovered = false;
    settingsUI->isBackHovered = false;
}

void drawSettingsUI(void) {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color textColor = {255, 255, 255, 255};
    renderText("Settings",
        VIRTUAL_WIDTH / 2 - 40,
        VIRTUAL_HEIGHT / 4,
        textColor
    );
    
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &settingsUI->volumeSlider);
    
    char volumeText[32];
    snprintf(volumeText, sizeof(volumeText), "Volume: %d%%", 
        (int)((audioManager->musicVolume / 128.0f) * 100));
    renderText(volumeText,
        settingsUI->volumeSlider.x,
        settingsUI->volumeSlider.y - 20,
        textColor
    );
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &settingsUI->volumeHandle);
    
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
    
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);
    
    settingsUI->isVolumeHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->volumeHandle);
    settingsUI->isMuteHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->muteButton);
    settingsUI->isBackHovered = isMouseOverButton(virtualMouseX, virtualMouseY, 
        settingsUI->backButton);
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            if (settingsUI->isVolumeHovered) {
                settingsUI->isDraggingVolume = true;
            }
            if (settingsUI->isMuteHovered) {
                toggleMute();
            }
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
            int sliderX = virtualMouseX - settingsUI->volumeSlider.x;
            sliderX = sliderX < 0 ? 0 : sliderX;
            sliderX = sliderX > settingsUI->volumeSlider.w ? 
                settingsUI->volumeSlider.w : sliderX;
            
            float percentage = (float)sliderX / settingsUI->volumeSlider.w;
            int newVolume = (int)(percentage * 128);
            
            setMusicVolume(newVolume);
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
