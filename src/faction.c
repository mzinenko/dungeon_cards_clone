#include "../inc/header.h"

void initFactions(void)
{
    // Initialize Sanctum Vanguard
    vanguardFaction = malloc(sizeof(Faction));
    *vanguardFaction = (Faction){
        .type = FACTION_VANGUARD,
        .name = "Sanctum Vanguard",
        .playerRank = 100,
        .upgradeCount = 0,
        .questCount = 0,
        .activeQuestCount = 0,
        .currentDialog = NULL,
        .ui = {.contentRect = {0, 0, 0, 0}, .returnButton = {0, 0, 0, 0}, .leaderArtRect = {0, 0, 0, 0}, .dialogRect = {0, 0, 0, 0}}};

    // Initialize Crimson Path
    crimsonFaction = malloc(sizeof(Faction));
    *crimsonFaction = (Faction){
        .type = FACTION_CRIMSON_PATH,
        .name = "The Crimson Path",
        .playerRank = 0,
        .upgradeCount = 0,
        .questCount = 0,
        .activeQuestCount = 0,
        .currentDialog = NULL,
        .ui = {.contentRect = {0, 0, 0, 0}, .returnButton = {0, 0, 0, 0}, .leaderArtRect = {0, 0, 0, 0}, .dialogRect = {0, 0, 0, 0}}};

    initFactionUI(vanguardFaction);
    initFactionUI(crimsonFaction);

    // Load faction leader textures
    // We'll implement this later when we have the art assets
    initFactionUpgrades(vanguardFaction);
    initFactionUpgrades(crimsonFaction);
}

void initFactionUI(Faction* faction) {
    faction->ui = (FactionUI){
        .leaderArtRect = {
            FACTION_PADDING,
            FACTION_PADDING,
            VIRTUAL_WIDTH * 0.3f,  // 30% of width for leader art
            VIRTUAL_HEIGHT * 0.6f   // 60% of height for leader art
        },
        .dialogRect = {
            FACTION_PADDING,
            VIRTUAL_HEIGHT - FACTION_DIALOG_HEIGHT - FACTION_PADDING,
            VIRTUAL_WIDTH - FACTION_PADDING * 2,
            FACTION_DIALOG_HEIGHT
        },
        .contentRect = {
            VIRTUAL_WIDTH * 0.3f + FACTION_PADDING * 2,
            FACTION_TAB_HEIGHT + FACTION_PADDING * 2,
            VIRTUAL_WIDTH * 0.7f - FACTION_PADDING * 3,
            VIRTUAL_HEIGHT - FACTION_TAB_HEIGHT - FACTION_DIALOG_HEIGHT - FACTION_PADDING * 4
        },
        .returnButton = {
            VIRTUAL_WIDTH - FACTION_BUTTON_WIDTH - FACTION_PADDING,
            FACTION_PADDING,
            FACTION_BUTTON_WIDTH,
            FACTION_BUTTON_HEIGHT
        },
        .isStoreTab = true,
        .dialogState = DIALOG_FIRST_ENCOUNTER,
        .returnHovered = false,
        .scrollOffset = 0,
        .maxScrollOffset = 0
    };
}

void switchToFaction(FactionType type)
{
    switch (type)
    {
    case FACTION_VANGUARD:
        currentFaction = vanguardFaction;
        break;
    case FACTION_CRIMSON_PATH:
        currentFaction = crimsonFaction;
        break;
    default:
        currentFaction = NULL;
        break;
    }

    // Reset UI state when switching
    if (currentFaction)
    {
        currentFaction->ui.isStoreTab = true;
        updateFactionDialog();
    }
}

void drawFactionUI(void) {
    if (!currentFaction)
        return;

    // Draw background
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, NULL);

    // Draw leader art area
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &currentFaction->ui.leaderArtRect);

    // Draw content area
    SDL_RenderFillRect(renderer, &currentFaction->ui.contentRect);

    // Draw dialog area
    SDL_RenderFillRect(renderer, &currentFaction->ui.dialogRect);

    // Draw faction name and rank
    SDL_Color textColor = {255, 255, 255, 255};
    char rankText[64];
    snprintf(rankText, sizeof(rankText), "%s - Standing: %d",
             currentFaction->name, currentFaction->playerRank);
    renderText(rankText, FACTION_PADDING * 2, FACTION_PADDING, textColor);

    // Draw tabs
    SDL_Rect storeTab = {
        currentFaction->ui.contentRect.x,
        FACTION_PADDING,
        FACTION_TAB_WIDTH,
        FACTION_TAB_HEIGHT
    };
    SDL_Rect questTab = {
        currentFaction->ui.contentRect.x + FACTION_TAB_WIDTH + FACTION_PADDING,
        FACTION_PADDING,
        FACTION_TAB_WIDTH,
        FACTION_TAB_HEIGHT
    };

    drawButton("Store", storeTab, currentFaction->ui.isStoreTab);
    drawButton("Quests", questTab, !currentFaction->ui.isStoreTab);

    // Draw current content
    if (currentFaction->ui.isStoreTab) {
        drawFactionStore();
    } else {
        drawFactionQuests();
    }

    // Draw dialog text
    if (currentFaction->currentDialog) {
        renderText(currentFaction->currentDialog,
            currentFaction->ui.dialogRect.x + FACTION_PADDING,
            currentFaction->ui.dialogRect.y + FACTION_PADDING,
            textColor
        );
    }

    // Draw return button
    drawButton("Return", currentFaction->ui.returnButton, 
        currentFaction->ui.returnHovered);
}

// Separate store drawing function for better organization
void drawFactionStore(void) {
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color lockedColor = {128, 128, 128, 255};

    // Create content clip area
    SDL_Rect clipRect = currentFaction->ui.contentRect;
    SDL_RenderSetClipRect(renderer, &clipRect);

    int startY = currentFaction->ui.contentRect.y - currentFaction->ui.scrollOffset;
    int contentHeight = currentFaction->upgradeCount * 
        (FACTION_ITEM_HEIGHT + FACTION_ITEM_SPACING) + FACTION_PADDING;

    currentFaction->ui.maxScrollOffset = 
        contentHeight - currentFaction->ui.contentRect.h;
    if (currentFaction->ui.maxScrollOffset < 0) 
        currentFaction->ui.maxScrollOffset = 0;

    // Keep scroll in bounds
    if (currentFaction->ui.scrollOffset > currentFaction->ui.maxScrollOffset)
        currentFaction->ui.scrollOffset = currentFaction->ui.maxScrollOffset;
    if (currentFaction->ui.scrollOffset < 0)
        currentFaction->ui.scrollOffset = 0;

    // Draw upgrades
    for (int i = 0; i < currentFaction->upgradeCount; i++) {
        FactionUpgrade *upgrade = &currentFaction->upgrades[i];
        SDL_Rect upgradeRect = {
            currentFaction->ui.contentRect.x + FACTION_PADDING,
            startY + i * (FACTION_ITEM_HEIGHT + FACTION_ITEM_SPACING),
            currentFaction->ui.contentRect.w - FACTION_PADDING * 2,
            FACTION_ITEM_HEIGHT
        };

        // Only draw if visible
        if (upgradeRect.y + upgradeRect.h >= currentFaction->ui.contentRect.y &&
            upgradeRect.y <= currentFaction->ui.contentRect.y + 
                currentFaction->ui.contentRect.h) {

            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            SDL_RenderFillRect(renderer, &upgradeRect);

            SDL_Color *color = currentFaction->playerRank >= upgrade->requiredRank
                ? &textColor
                : &lockedColor;

            // Draw upgrade info
            char levelText[128];
            snprintf(levelText, sizeof(levelText), "%s (Level %d/%d)",
                upgrade->name, upgrade->currentLevel, upgrade->maxLevel);
            renderText(levelText, 
                upgradeRect.x + FACTION_PADDING,
                upgradeRect.y + FACTION_PADDING,
                *color
            );

            renderText(upgrade->description,
                upgradeRect.x + FACTION_PADDING,
                upgradeRect.y + FACTION_PADDING * 3,
                *color
            );

            // Draw costs
            char costText[256];
            int goldCost = upgrade->baseCost * (upgrade->currentLevel + 1);
            char gemCosts[128] = "";
            for (int j = 0; j < 5; j++) {
                if (upgrade->gemCosts[j] > 0) {
                    char gemText[32];
                    const char *rarityNames[] = {
                        "Common", "Uncommon", "Rare", "Epic", "Legendary"
                    };
                    snprintf(gemText, sizeof(gemText), "%d %s%s",
                        upgrade->gemCosts[j],
                        rarityNames[j],
                        strlen(gemCosts) == 0 ? "" : ","
                    );
                    strcat(gemCosts, gemText);
                }
            }

            // Draw costs text
            snprintf(costText, sizeof(costText), "Cost: %d gold%s%s",
                goldCost,
                strlen(gemCosts) > 0 ? " + " : "",
                gemCosts
            );
            renderText(costText,
                upgradeRect.x + FACTION_PADDING,
                upgradeRect.y + FACTION_PADDING * 5,
                *color
            );

            // Draw rank requirement
            char rankText[64];
            snprintf(rankText, sizeof(rankText), "Requires Standing %d",
                upgrade->requiredRank);
            renderText(rankText,
                upgradeRect.x + FACTION_PADDING,
                upgradeRect.y + FACTION_PADDING * 7,
                *color
            );

            // Draw buy button if available
            if (currentFaction->playerRank >= upgrade->requiredRank &&
                upgrade->currentLevel < upgrade->maxLevel) {

                bool canAfford = (progress->totalCoins >= goldCost);
                for (int j = 0; j < 5 && canAfford; j++) {
                    if (upgrade->gemCosts[j] > 0 && 
                        progress->gems[j] < upgrade->gemCosts[j]) {
                        canAfford = false;
                    }
                }

                SDL_Rect buyButton = {
                    upgradeRect.x + upgradeRect.w - FACTION_BUTTON_WIDTH - FACTION_PADDING,
                    upgradeRect.y + upgradeRect.h - FACTION_BUTTON_HEIGHT - FACTION_PADDING,
                    FACTION_BUTTON_WIDTH,
                    FACTION_BUTTON_HEIGHT
                };

                SDL_SetRenderDrawColor(renderer,
                    canAfford ? 60 : 40,
                    canAfford ? 60 : 40,
                    canAfford ? 60 : 40,
                    255
                );
                SDL_RenderFillRect(renderer, &buyButton);
                renderText("Buy",
                    buyButton.x + FACTION_PADDING,
                    buyButton.y + FACTION_PADDING / 2,
                    canAfford ? textColor : lockedColor
                );
            }
        }
    }

    SDL_RenderSetClipRect(renderer, NULL);

    // Draw scrollbar if needed
    if (currentFaction->ui.maxScrollOffset > 0) {
        int scrollbarHeight = currentFaction->ui.contentRect.h * 
            (currentFaction->ui.contentRect.h / 
            (float)(currentFaction->upgradeCount * 
            (FACTION_ITEM_HEIGHT + FACTION_ITEM_SPACING)));

        int scrollbarY = currentFaction->ui.contentRect.y +
            (currentFaction->ui.scrollOffset * 
            (currentFaction->ui.contentRect.h - scrollbarHeight) /
            currentFaction->ui.maxScrollOffset);

        SDL_Rect scrollbar = {
            currentFaction->ui.contentRect.x + 
                currentFaction->ui.contentRect.w - 4,
            scrollbarY,
            4,
            scrollbarHeight
        };

        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderFillRect(renderer, &scrollbar);
    }
}

void handleFactionInput(void) {
    if (!currentFaction)
        return;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Convert window coordinates to virtual coordinates
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

    // Check tab clicks
    SDL_Rect storeTab = {
        currentFaction->ui.contentRect.x,
        FACTION_PADDING,
        FACTION_TAB_WIDTH,
        FACTION_TAB_HEIGHT
    };
    SDL_Rect questTab = {
        currentFaction->ui.contentRect.x + FACTION_TAB_WIDTH + FACTION_PADDING,
        FACTION_PADDING,
        FACTION_TAB_WIDTH,
        FACTION_TAB_HEIGHT
    };

    currentFaction->ui.returnHovered = isMouseOverButton(virtualMouseX, virtualMouseY,
        currentFaction->ui.returnButton);

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (isMouseOverButton(virtualMouseX, virtualMouseY, storeTab)) {
            currentFaction->ui.isStoreTab = true;
            return;
        }
        else if (isMouseOverButton(virtualMouseX, virtualMouseY, questTab)) {
            currentFaction->ui.isStoreTab = false;
            return;
        }

        if (currentFaction->ui.returnHovered) {
            gameContext->currentState = STATE_HUB;
            currentFaction = NULL;
            return;
        }
    }

    // Handle scroll wheel
    if (event.type == SDL_MOUSEWHEEL) {
        if (currentFaction && currentFaction->ui.isStoreTab) {
            // Only scroll if mouse is over content area
            if (virtualMouseX >= currentFaction->ui.contentRect.x &&
                virtualMouseX <= currentFaction->ui.contentRect.x + currentFaction->ui.contentRect.w &&
                virtualMouseY >= currentFaction->ui.contentRect.y &&
                virtualMouseY <= currentFaction->ui.contentRect.y + currentFaction->ui.contentRect.h) {

                currentFaction->ui.scrollOffset -= event.wheel.y * 8;

                // Keep scroll offset in bounds
                if (currentFaction->ui.scrollOffset < 0) {
                    currentFaction->ui.scrollOffset = 0;
                }
                if (currentFaction->ui.scrollOffset > currentFaction->ui.maxScrollOffset) {
                    currentFaction->ui.scrollOffset = currentFaction->ui.maxScrollOffset;
                }
            }
        }
    }

    // Handle upgrade purchases
    if (event.type == SDL_MOUSEBUTTONDOWN && currentFaction->ui.isStoreTab) {
        int startY = currentFaction->ui.contentRect.y - currentFaction->ui.scrollOffset;

        for (int i = 0; i < currentFaction->upgradeCount; i++) {
            SDL_Rect upgradeRect = {
                currentFaction->ui.contentRect.x + FACTION_PADDING,
                startY + i * (FACTION_ITEM_HEIGHT + FACTION_ITEM_SPACING),
                currentFaction->ui.contentRect.w - FACTION_PADDING * 2,
                FACTION_ITEM_HEIGHT
            };

            // Only check if upgrade is visible
            if (upgradeRect.y + upgradeRect.h >= currentFaction->ui.contentRect.y &&
                upgradeRect.y <= currentFaction->ui.contentRect.y + currentFaction->ui.contentRect.h) {

                SDL_Rect buyButton = {
                    upgradeRect.x + upgradeRect.w - FACTION_BUTTON_WIDTH - FACTION_PADDING,
                    upgradeRect.y + upgradeRect.h - FACTION_BUTTON_HEIGHT - FACTION_PADDING,
                    FACTION_BUTTON_WIDTH,
                    FACTION_BUTTON_HEIGHT
                };

                if (isMouseOverButton(virtualMouseX, virtualMouseY, buyButton)) {
                    handleUpgradePurchase(currentFaction, i);
                    break;
                }
            }
        }
    }
}

void handleUpgradePurchase(Faction* faction, int upgradeIndex) {
    if (purchaseUpgrade(faction, upgradeIndex)) {
        if (faction->type == FACTION_VANGUARD) {
            // Check if upgrade already exists
            int existingIndex = -1;
            for (int i = 0; i < progress->vanguardUpgrades.count; i++) {
                if (progress->vanguardUpgrades.upgrades[i].upgradeId == upgradeIndex) {
                    existingIndex = i;
                    break;
                }
            }

            if (existingIndex >= 0) {
                progress->vanguardUpgrades.upgrades[existingIndex].currentLevel++;
            } else {
                SavedUpgrade newUpgrade = {
                    .upgradeId = upgradeIndex,
                    .currentLevel = 1
                };
                progress->vanguardUpgrades.upgrades[progress->vanguardUpgrades.count] = newUpgrade;
                progress->vanguardUpgrades.count++;
            }
        } else if (faction->type == FACTION_CRIMSON_PATH) {
            // Check if upgrade already exists
            int existingIndex = -1;
            for (int i = 0; i < progress->crimsonUpgrades.count; i++) {
                if (progress->crimsonUpgrades.upgrades[i].upgradeId == upgradeIndex) {
                    existingIndex = i;
                    break;
                }
            }

            if (existingIndex >= 0) {
                progress->crimsonUpgrades.upgrades[existingIndex].currentLevel++;
            } else {
                SavedUpgrade newUpgrade = {
                    .upgradeId = upgradeIndex,
                    .currentLevel = 1
                };
                progress->crimsonUpgrades.upgrades[progress->crimsonUpgrades.count] = newUpgrade;
                progress->crimsonUpgrades.count++;
            }
        }
        saveProgress(saveSelectUI->selectedFile);
    }
}

void drawFactionQuests(void)
{
    // Placeholder for quest UI
    SDL_Color textColor = {255, 255, 255, 255};
    renderText("Quests Coming Soon",
               currentFaction->ui.contentRect.x + 20,
               currentFaction->ui.contentRect.y + 20,
               textColor);
}

void updateFactionDialog(void)
{
    // We'll implement proper dialog system later
    if (!currentFaction)
        return;

    if (currentFaction->ui.dialogState == DIALOG_FIRST_ENCOUNTER)
    {
        if (currentFaction->type == FACTION_VANGUARD)
        {
            currentFaction->currentDialog = "Hold. Providence Keep security checkpoint. I am Lord Sentinel Aldrich Vale.";
        }
        else
        {
            currentFaction->currentDialog = "Fresh blood walks into our den. I am Ravenna, Beast Queen of the Crimson Path.";
        }
    }
}
