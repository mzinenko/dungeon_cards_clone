#include "../inc/header.h"

void initRenderContext(void) {
    renderCtx = malloc(sizeof(RenderContext));
    
    renderCtx->renderTarget = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        VIRTUAL_WIDTH,
        VIRTUAL_HEIGHT
    );
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    updateRenderScale();
}

void updateRenderScale(void) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    
    float scaleX = (float)windowWidth / VIRTUAL_WIDTH;
    float scaleY = (float)windowHeight / VIRTUAL_HEIGHT;
    float scale = fminf(scaleX, scaleY);
    
    renderCtx->scaleX = scale;
    renderCtx->scaleY = scale;

    int scaledWidth = (int)(VIRTUAL_WIDTH * scale);
    int scaledHeight = (int)(VIRTUAL_HEIGHT * scale);
    
    renderCtx->outputRect.x = (windowWidth - scaledWidth) / 2;
    renderCtx->outputRect.y = (windowHeight - scaledHeight) / 2;
    renderCtx->outputRect.w = scaledWidth;
    renderCtx->outputRect.h = scaledHeight;
}

void beginRender(void) {
    SDL_SetRenderTarget(renderer, renderCtx->renderTarget);
    SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
    SDL_RenderClear(renderer);
}

void endRender(void) {
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_RenderCopy(renderer, renderCtx->renderTarget, NULL, &renderCtx->outputRect);
    SDL_RenderPresent(renderer);
}

void windowToVirtual(int windowX, int windowY, int* virtualX, int* virtualY) {
    *virtualX = (int)((windowX - renderCtx->outputRect.x) / renderCtx->scaleX);
    *virtualY = (int)((windowY - renderCtx->outputRect.y) / renderCtx->scaleY);
}

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

void drawButton(const char *text, SDL_Rect rect, bool isHovered) {
    Texture *buttonTexture = &uiTextures[8]; 
    
    int frame = isHovered ? 1 : 0;

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    if (textSurface) {
    
    SDL_Rect spriteClip = {
        buttonTexture->clipRect.x + frame * (buttonTexture->clipRect.w + buttonTexture->frameGap),
        buttonTexture->clipRect.y,
        buttonTexture->clipRect.w,
        buttonTexture->clipRect.h
    };

    rect.w = rect.w > textSurface->w ? rect.w : textSurface->w;
    rect.w += 4;
    rect.h = rect.h > textSurface->h ? rect.h : textSurface->h;
    rect.h += 2;

    SDL_RenderCopy(renderer, buttonTexture->texture, &spriteClip, &rect);

        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_Rect textRect = {
                rect.x + (rect.w - textSurface->w) / 2,
                rect.y + (rect.h - textSurface->h) / 2 + 1,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void renderText(const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect destRect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

bool isMouseOverButton(int mouseX, int mouseY, SDL_Rect button) {
    return (mouseX >= button.x && mouseX <= button.x + button.w &&
            mouseY >= button.y && mouseY <= button.y + button.h);
}
