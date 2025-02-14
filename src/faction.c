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
        .ui = {
            .leaderArtRect = {WINDOW_WIDTH / 4 - 200, 100, 400, 500},
            .dialogRect = {WINDOW_WIDTH / 4 - 200, 620, WINDOW_WIDTH / 2 + 400, 150},
            .contentRect = {WINDOW_WIDTH / 2 + 50, 100, 500, 500},
            .isStoreTab = true,
            .dialogState = DIALOG_FIRST_ENCOUNTER,
            .returnButton = {WINDOW_WIDTH - 120, 20, 100, 40},
            .returnHovered = false,
        }};

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
        .ui = {
            .leaderArtRect = {WINDOW_WIDTH / 4 - 200, 100, 400, 500},
            .dialogRect = {WINDOW_WIDTH / 4 - 200, 620, WINDOW_WIDTH / 2 + 400, 150},
            .contentRect = {WINDOW_WIDTH / 2 + 50, 100, 500, 500},
            .isStoreTab = true,
            .dialogState = DIALOG_FIRST_ENCOUNTER,
            .returnButton = {WINDOW_WIDTH - 120, 20, 100, 40},
            .returnHovered = false,
        }};

    // Load faction leader textures
    // We'll implement this later when we have the art assets
    initFactionUpgrades(vanguardFaction);
    initFactionUpgrades(crimsonFaction);
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

void drawFactionUI(void)
{
    if (!currentFaction)
        return;

    // Draw background
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, NULL);

    // Draw leader art area
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &currentFaction->ui.leaderArtRect);

    // Draw content area (store/quests)
    SDL_RenderFillRect(renderer, &currentFaction->ui.contentRect);

    // Draw dialog area
    SDL_RenderFillRect(renderer, &currentFaction->ui.dialogRect);

    // Draw faction name and rank
    SDL_Color textColor = {255, 255, 255, 255};
    char rankText[64];
    snprintf(rankText, sizeof(rankText), "%s - Rank: %d",
             currentFaction->name, currentFaction->playerRank);
    renderText(rankText, 20, 20, textColor);

    // Draw store/quest tabs
    SDL_Rect storeTab = {
        currentFaction->ui.contentRect.x,
        currentFaction->ui.contentRect.y - 40,
        100,
        40};
    SDL_Rect questTab = {
        currentFaction->ui.contentRect.x + 110,
        currentFaction->ui.contentRect.y - 40,
        100,
        40};

    // Draw tabs
    drawButton("Store", storeTab, currentFaction->ui.isStoreTab);
    drawButton("Quests", questTab, !currentFaction->ui.isStoreTab);

    // Draw current dialog
    if (currentFaction->currentDialog)
    {
        renderText(currentFaction->currentDialog,
                   currentFaction->ui.dialogRect.x + 10,
                   currentFaction->ui.dialogRect.y + 10,
                   textColor);
    }

    // Draw either store or quest content based on current tab
    if (currentFaction->ui.isStoreTab)
    {
        drawFactionStore();
    }
    else
    {
        drawFactionQuests();
    }

    drawButton("Return", currentFaction->ui.returnButton,
               currentFaction->ui.returnHovered);
}

void handleFactionInput(void)
{
    if (!currentFaction)
        return;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Check tab clicks
    SDL_Rect storeTab = {
        currentFaction->ui.contentRect.x,
        currentFaction->ui.contentRect.y - 40,
        100,
        40};
    SDL_Rect questTab = {
        currentFaction->ui.contentRect.x + 110,
        currentFaction->ui.contentRect.y - 40,
        100,
        40};

    currentFaction->ui.returnHovered = isMouseOverButton(mouseX, mouseY,
                                                         currentFaction->ui.returnButton);

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (isMouseOverButton(mouseX, mouseY, storeTab))
        {
            currentFaction->ui.isStoreTab = true;
        }
        else if (isMouseOverButton(mouseX, mouseY, questTab))
        {
            currentFaction->ui.isStoreTab = false;
        }

        if (currentFaction->ui.returnHovered)
        {
            gameContext->currentState = STATE_HUB;
            currentFaction = NULL;
            return;
        }
    }

    if (event.type == SDL_MOUSEWHEEL)
    {
        if (currentFaction && currentFaction->ui.isStoreTab)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Only scroll if mouse is over content area
            if (mouseX >= currentFaction->ui.contentRect.x &&
                mouseX <= currentFaction->ui.contentRect.x + currentFaction->ui.contentRect.w &&
                mouseY >= currentFaction->ui.contentRect.y &&
                mouseY <= currentFaction->ui.contentRect.y + currentFaction->ui.contentRect.h)
            {

                currentFaction->ui.scrollOffset -= event.wheel.y * 30; // Adjust 30 for scroll speed

                // Keep scroll offset in bounds
                if (currentFaction->ui.scrollOffset < 0)
                {
                    currentFaction->ui.scrollOffset = 0;
                }
                if (currentFaction->ui.scrollOffset > currentFaction->ui.maxScrollOffset)
                {
                    currentFaction->ui.scrollOffset = currentFaction->ui.maxScrollOffset;
                }
            }
        }
    }

    // Adjust the button click handling to account for scroll offset
    if (event.type == SDL_MOUSEBUTTONDOWN && currentFaction->ui.isStoreTab)
    {
        int startY = currentFaction->ui.contentRect.y + 20 - currentFaction->ui.scrollOffset;
        int itemHeight = 100;

        for (int i = 0; i < currentFaction->upgradeCount; i++)
        {
            SDL_Rect buyButton = {
                currentFaction->ui.contentRect.x + currentFaction->ui.contentRect.w - 120,
                startY + (i * (itemHeight + 10)) + itemHeight - 35,
                100,
                25};

            // Only check buttons that are visible
            if (buyButton.y + buyButton.h >= currentFaction->ui.contentRect.y &&
                buyButton.y <= currentFaction->ui.contentRect.y + currentFaction->ui.contentRect.h)
            {

                if (isMouseOverButton(mouseX, mouseY, buyButton))
                {
                    if (purchaseUpgrade(currentFaction, i))
                    {
                        FactionUpgrade *upgrade = &currentFaction->upgrades[i];
                        if (currentFaction->type == FACTION_VANGUARD)
                        {
                            // Check if upgrade already exists
                            int existingIndex = -1;
                            for (int i = 0; i < progress->vanguardUpgrades.count; i++)
                            {
                                if (strcmp(progress->vanguardUpgrades.upgrades[i].name, upgrade->name) == 0)
                                {
                                    existingIndex = i;
                                    break;
                                }
                            }

                            if (existingIndex >= 0)
                            {
                                // Increase level of existing upgrade
                                progress->vanguardUpgrades.upgrades[existingIndex].currentLevel++;
                            }
                            else
                            {
                                // Add new upgrade
                                progress->vanguardUpgrades.upgrades[progress->vanguardUpgrades.count] = *upgrade;
                                progress->vanguardUpgrades.upgrades[progress->vanguardUpgrades.count].currentLevel = 1;
                                progress->vanguardUpgrades.count++;
                            }
                        }
                        else if (currentFaction->type == FACTION_CRIMSON_PATH)
                        {
                            // Same logic for Crimson Path
                            int existingIndex = -1;
                            for (int i = 0; i < progress->crimsonUpgrades.count; i++)
                            {
                                if (strcmp(progress->crimsonUpgrades.upgrades[i].name, upgrade->name) == 0)
                                {
                                    existingIndex = i;
                                    break;
                                }
                            }

                            if (existingIndex >= 0)
                            {
                                progress->crimsonUpgrades.upgrades[existingIndex].currentLevel++;
                            }
                            else
                            {
                                progress->crimsonUpgrades.upgrades[progress->crimsonUpgrades.count] = *upgrade;
                                progress->crimsonUpgrades.upgrades[progress->crimsonUpgrades.count].currentLevel = 1;
                                progress->crimsonUpgrades.count++;
                            }
                        }
                        saveProgress(saveSelectUI->selectedFile);
                        printf("Upgrade purchased successfully!\n");
                    }
                    break;
                }
            }
        }
    }
}

void drawFactionStore(void)
{
    if (!currentFaction)
        return;

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color lockedColor = {128, 128, 128, 255};

    int itemHeight = 100; // Height of each upgrade item
    int contentHeight = currentFaction->upgradeCount * (itemHeight + 10) + 30;
    int visibleHeight = currentFaction->ui.contentRect.h;

    // Calculate max scroll offset
    currentFaction->ui.maxScrollOffset = contentHeight - visibleHeight;
    if (currentFaction->ui.maxScrollOffset < 0)
        currentFaction->ui.maxScrollOffset = 0;

    // Keep scroll offset in bounds
    if (currentFaction->ui.scrollOffset > currentFaction->ui.maxScrollOffset)
    {
        currentFaction->ui.scrollOffset = currentFaction->ui.maxScrollOffset;
    }
    if (currentFaction->ui.scrollOffset < 0)
    {
        currentFaction->ui.scrollOffset = 0;
    }

    // Create a clip rectangle for scrolling
    SDL_Rect clipRect = currentFaction->ui.contentRect;
    SDL_RenderSetClipRect(renderer, &clipRect);

    int startY = currentFaction->ui.contentRect.y + 20 - currentFaction->ui.scrollOffset;

    for (int i = 0; i < currentFaction->upgradeCount; i++)
    {
        FactionUpgrade *upgrade = &currentFaction->upgrades[i];
        SDL_Rect upgradeRect = {
            currentFaction->ui.contentRect.x + 10,
            startY + (i * (itemHeight + 10)),
            currentFaction->ui.contentRect.w - 20,
            itemHeight};

        // Only draw if the upgrade is visible
        if (upgradeRect.y + upgradeRect.h >= currentFaction->ui.contentRect.y &&
            upgradeRect.y <= currentFaction->ui.contentRect.y + currentFaction->ui.contentRect.h)
        {

            // Draw background
            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            SDL_RenderFillRect(renderer, &upgradeRect);

            // Draw name and level
            char levelText[128];
            snprintf(levelText, sizeof(levelText), "%s (Level %d/%d)",
                     upgrade->name, upgrade->currentLevel, upgrade->maxLevel);

            SDL_Color *color = (currentFaction->playerRank >= upgrade->requiredRank)
                                   ? &textColor
                                   : &lockedColor;

            renderText(levelText, upgradeRect.x + 10, upgradeRect.y + 10, *color);

            // Draw description
            renderText(upgrade->description,
                       upgradeRect.x + 10, upgradeRect.y + 30, *color);

            // Draw costs
            char costText[256];
            int goldCost = upgrade->baseCost * (upgrade->currentLevel + 1);

            // Build gem cost string
            char gemCosts[128] = "";
            for (int j = 0; j < 5; j++)
            {
                if (upgrade->gemCosts[j] > 0)
                {
                    char gemText[32];
                    const char *rarityNames[] = {"Common", "Uncommon", "Rare", "Epic", "Legendary"};
                    snprintf(gemText, sizeof(gemText), "%d %s%s",
                             upgrade->gemCosts[j],
                             rarityNames[j],
                             (strlen(gemCosts) == 0) ? "" : ",");
                    strcat(gemCosts, gemText);
                }
            }

            // Combine gold and gem costs
            snprintf(costText, sizeof(costText), "Cost: %d gold%s%s",
                     goldCost,
                     strlen(gemCosts) > 0 ? " + " : "",
                     gemCosts);

            renderText(costText, upgradeRect.x + 10, upgradeRect.y + 50, *color);

            // Draw rank requirement
            char rankText[64];
            snprintf(rankText, sizeof(rankText), "Requires Rank %d",
                     upgrade->requiredRank);
            renderText(rankText, upgradeRect.x + 10, upgradeRect.y + 70, *color);

            // Draw buy button if available
            if (currentFaction->playerRank >= upgrade->requiredRank &&
                upgrade->currentLevel < upgrade->maxLevel)
            {

                // Check if player can afford all costs
                bool canAfford = (progress->totalCoins >= goldCost);
                for (int j = 0; j < 5 && canAfford; j++)
                {
                    if (upgrade->gemCosts[j] > 0 && progress->gems[j] < upgrade->gemCosts[j])
                    {
                        canAfford = false;
                    }
                }

                SDL_Rect buyButton = {
                    upgradeRect.x + upgradeRect.w - 110,
                    upgradeRect.y + upgradeRect.h - 30,
                    100,
                    25};

                // SDL_Color buttonColor = canAfford ? textColor : lockedColor;
                drawButton("Buy", buyButton, false); // We'll add hover state later
            }
        }
    }

    // Reset clip rectangle
    SDL_RenderSetClipRect(renderer, NULL);

    // Draw scrollbar if needed
    if (currentFaction->ui.maxScrollOffset > 0)
    {
        int scrollbarHeight = visibleHeight * (visibleHeight / (float)contentHeight);
        int scrollbarY = currentFaction->ui.contentRect.y +
                         (currentFaction->ui.scrollOffset * (visibleHeight - scrollbarHeight) /
                          currentFaction->ui.maxScrollOffset);

        SDL_Rect scrollbar = {
            currentFaction->ui.contentRect.x + currentFaction->ui.contentRect.w - 8,
            scrollbarY,
            4,
            scrollbarHeight};

        // Draw scrollbar background
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_Rect scrollbarBg = {
            scrollbar.x,
            currentFaction->ui.contentRect.y,
            scrollbar.w,
            visibleHeight};
        SDL_RenderFillRect(renderer, &scrollbarBg);

        // Draw scrollbar
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &scrollbar);
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
