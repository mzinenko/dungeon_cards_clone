#include "../inc/header.h"

void initRenderContext(void) {
    renderCtx = malloc(sizeof(RenderContext));
    
    // Create render target texture
    renderCtx->renderTarget = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        VIRTUAL_WIDTH,
        VIRTUAL_HEIGHT
    );
    
    // Set texture scaling quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");  // For pixel-perfect scaling
    
    // Initial scale calculation
    updateRenderScale();
}

void updateRenderScale(void) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    
    // Calculate scaling factors
    float scaleX = (float)windowWidth / VIRTUAL_WIDTH;
    float scaleY = (float)windowHeight / VIRTUAL_HEIGHT;
    
    // Use the smaller scale factor to maintain aspect ratio
    float scale = fminf(scaleX, scaleY);
    
    renderCtx->scaleX = scale;
    renderCtx->scaleY = scale;
    
    // Calculate letterboxing
    int scaledWidth = (int)(VIRTUAL_WIDTH * scale);
    int scaledHeight = (int)(VIRTUAL_HEIGHT * scale);
    
    renderCtx->outputRect.x = (windowWidth - scaledWidth) / 2;
    renderCtx->outputRect.y = (windowHeight - scaledHeight) / 2;
    renderCtx->outputRect.w = scaledWidth;
    renderCtx->outputRect.h = scaledHeight;
}

void beginRender(void) {
    // Switch render target to our virtual resolution texture
    SDL_SetRenderTarget(renderer, renderCtx->renderTarget);
    SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
    SDL_RenderClear(renderer);
}

void endRender(void) {
    // Switch back to the window target
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Render the scaled virtual texture
    SDL_RenderCopy(renderer, renderCtx->renderTarget, NULL, &renderCtx->outputRect);
    SDL_RenderPresent(renderer);
}

// Convert window coordinates to virtual coordinates
void windowToVirtual(int windowX, int windowY, int* virtualX, int* virtualY) {
    *virtualX = (int)((windowX - renderCtx->outputRect.x) / renderCtx->scaleX);
    *virtualY = (int)((windowY - renderCtx->outputRect.y) / renderCtx->scaleY);
}

// Convert virtual coordinates to window coordinates
void virtualToWindow(int virtualX, int virtualY, int* windowX, int* windowY) {
    *windowX = (int)(virtualX * renderCtx->scaleX) + renderCtx->outputRect.x;
    *windowY = (int)(virtualY * renderCtx->scaleY) + renderCtx->outputRect.y;
}

void destroyRenderContext(void) {
    if (renderCtx) {
        if (renderCtx->renderTarget) {
            SDL_DestroyTexture(renderCtx->renderTarget);
        }
        free(renderCtx);
        renderCtx = NULL;
    }
}

void drawButton(const char *text, SDL_Rect rect, bool isHovered)
{
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
    if (textSurface)
    {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture)
        {
            SDL_Rect textRect = {
                rect.x + (rect.w - textSurface->w) / 2,
                rect.y + (rect.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h};
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void renderText(const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture)
        {
            SDL_Rect destRect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

bool isMouseOverButton(int mouseX, int mouseY, SDL_Rect button)
{
    return (mouseX >= button.x && mouseX <= button.x + button.w &&
            mouseY >= button.y && mouseY <= button.y + button.h);
}
