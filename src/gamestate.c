// gamestate.c
#include "gamestate.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

void initGameContext(GameContext *context) {
    context->currentState = STATE_MAIN_MENU;
    context->characterSelected = 0;
    context->score = 0;
}

// Helper function to create buttons
SDL_bool isMouseOverButton(int mouseX, int mouseY, SDL_Rect button) {
    return (mouseX >= button.x && mouseX <= button.x + button.w &&
            mouseY >= button.y && mouseY <= button.y + button.h);
}

void drawButton(SDL_Renderer *renderer, TTF_Font *font, const char *text, 
                SDL_Rect rect, SDL_bool isHovered) {
    // Draw button background
    SDL_SetRenderDrawColor(renderer, 
                          isHovered ? 100 : 70,
                          isHovered ? 100 : 70,
                          isHovered ? 100 : 70, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    // Draw button border
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    // Draw button text
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    if (textSurface) {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_Rect textRect = {
                rect.x + (rect.w - textSurface->w) / 2,
                rect.y + (rect.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void drawMainMenu(SDL_Renderer *renderer, TTF_Font *font) {
    // Get mouse position for hover effects
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Semi-transparent background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect fullscreen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &fullscreen);
    
    // Title
    SDL_Color titleColor = {255, 255, 255, 255};
    renderText(renderer, font, "Dungeon Cards", 
              WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/3, titleColor);
    
    // Character selection (for future use)
    SDL_Rect charRect = {WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 50, 200, 100};
    drawButton(renderer, font, "Hero 1", charRect, 
               isMouseOverButton(mouseX, mouseY, charRect));
    
    // Start button
    SDL_Rect startRect = {WINDOW_WIDTH/2 - 60, WINDOW_HEIGHT*2/3, 120, 50};
    drawButton(renderer, font, "Start Game", startRect,
               isMouseOverButton(mouseX, mouseY, startRect));
}

void drawGameOver(SDL_Renderer *renderer, TTF_Font *font, GameContext *context) {
    // Create blur effect by drawing semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect fullscreen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &fullscreen);
    
    // Get mouse position for hover effects
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Game Over text
    SDL_Color textColor = {255, 255, 255, 255};
    renderText(renderer, font, "Game Over", 
              WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/3, textColor);
    
    // Score
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", context->score);
    renderText(renderer, font, scoreText,
              WINDOW_WIDTH/2 - 60, WINDOW_HEIGHT/2 - 30, textColor);
    
    // Buttons
    SDL_Rect playAgainRect = {WINDOW_WIDTH/2 - 130, WINDOW_HEIGHT*2/3, 120, 50};
    SDL_Rect homeRect = {WINDOW_WIDTH/2 + 10, WINDOW_HEIGHT*2/3, 120, 50};
    
    drawButton(renderer, font, "Play Again", playAgainRect,
               isMouseOverButton(mouseX, mouseY, playAgainRect));
    drawButton(renderer, font, "Home", homeRect,
               isMouseOverButton(mouseX, mouseY, homeRect));
}

void handleMenuInput(SDL_Event *event, GameContext *context, Player *player, Grid *grid) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event->button.x;
        int mouseY = event->button.y;
        
        // Start button
        SDL_Rect startRect = {WINDOW_WIDTH/2 - 60, WINDOW_HEIGHT*2/3, 120, 50};
        if (isMouseOverButton(mouseX, mouseY, startRect)) {
            context->currentState = STATE_GAMEPLAY;
            initPlayer(player);
            initGrid(grid, 5, 5);
            populateGrid(grid);
        }
        
        // Character selection (for future use)
        SDL_Rect charRect = {WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 50, 200, 100};
        if (isMouseOverButton(mouseX, mouseY, charRect)) {
            context->characterSelected = 1;  // For now, only one character
        }
    }
}

void handleGameOverInput(SDL_Event *event, GameContext *context, Player *player, Grid *grid) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event->button.x;
        int mouseY = event->button.y;
        
        // Play Again button
        SDL_Rect playAgainRect = {WINDOW_WIDTH/2 - 130, WINDOW_HEIGHT*2/3, 120, 50};
        if (isMouseOverButton(mouseX, mouseY, playAgainRect)) {
            context->currentState = STATE_GAMEPLAY;
            initPlayer(player);
            initGrid(grid, 5, 5);
            populateGrid(grid);
        }
        
        // Home button
        SDL_Rect homeRect = {WINDOW_WIDTH/2 + 10, WINDOW_HEIGHT*2/3, 120, 50};
        if (isMouseOverButton(mouseX, mouseY, homeRect)) {
            context->currentState = STATE_MAIN_MENU;
        }
    }
}
