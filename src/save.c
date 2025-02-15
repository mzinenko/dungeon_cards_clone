#include "../inc/header.h"

static uint32_t calculateChecksum(const PlayerProgress *data) {
    uint32_t checksum = 0;
    const uint8_t *bytes = (const uint8_t *)data;
    for (size_t i = 0; i < sizeof(PlayerProgress); i++) {
        checksum = (checksum << 1) | (checksum >> 31); 
        checksum ^= bytes[i];
    }
    return checksum;
}

static void ensureSaveDirectory(void) {
#ifdef _WIN32
    _mkdir(SAVE_DIR);
#else
    mkdir(SAVE_DIR, 0777);
#endif
}

static bool validateSaveFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file)
        return false;

    SaveFileHeader header;
    size_t read = fread(&header, sizeof(header), 1, file);
    fclose(file);

    if (read != 1)
        return false;
    if (header.magicNumber != SAVE_FILE_MAGIC)
        return false;
    if (header.version != 1 && header.version != SAVE_VERSION)
        return false; 

    if (header.version == SAVE_VERSION) {
        uint32_t checksum = calculateChecksum(&header.data);
        return checksum == header.checksum;
    }

    return true;
}

void scanSaveFiles(void) {
    printf("Scanning for save files...\n");
    ensureSaveDirectory();
    numSaveFiles = 0;

    DIR *dir = opendir(SAVE_DIR);
    if (!dir)
        return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && numSaveFiles < MAX_SAVE_FILES) {
        if (entry->d_type == DT_REG) { 
            char fullPath[512];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, entry->d_name);

            struct stat st;
            if (stat(fullPath, &st) == 0) {
                SaveFileInfo *info = &saveFiles[numSaveFiles];
                strncpy(info->filename, entry->d_name, sizeof(info->filename) - 1);
                info->lastModified = st.st_mtime;
                info->isValid = validateSaveFile(fullPath);

                if (info->isValid) {
                    FILE *file = fopen(fullPath, "rb");
                    if (file) {
                        SaveFileHeader header;
                        if (fread(&header, sizeof(header), 1, file) == 1) {
                            info->preview = header.data;
                        }
                        fclose(file);
                    }
                }
                numSaveFiles++;
            }
        }
    }
    closedir(dir);
}

bool loadSave(const char *filename) {
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, filename);

    FILE *file = fopen(fullPath, "rb");
    if (!file)
        return false;

    SaveFileHeader header;
    bool success = false;

    if (fread(&header, sizeof(header), 1, file) == 1) {
        if (header.magicNumber == SAVE_FILE_MAGIC) {
            if (header.version == 1) {
                PlayerProgress newProgress = {
                    .totalCoins = header.data.totalCoins,
                    .gems = {0},
                    .vanguardUpgrades = {0},
                    .crimsonUpgrades = {0}};
                progress = malloc(sizeof(PlayerProgress));
                *progress = newProgress;
                success = true;
            }
            else if (header.version == 2) {
                PlayerProgress newProgress = {
                    .totalCoins = header.data.totalCoins,
                    .vanguardUpgrades = {0},
                    .crimsonUpgrades = {0}};
                memcpy(newProgress.gems, header.data.gems, sizeof(header.data.gems));

                progress = malloc(sizeof(PlayerProgress));
                *progress = newProgress;
                success = true;
            }
            else if (header.version == SAVE_VERSION) {
                if (calculateChecksum(&header.data) == header.checksum) {
                    progress = malloc(sizeof(PlayerProgress));
                    *progress = header.data;
                    success = true;
                }
            }
        }
    }

    fclose(file);

    printf("\n\n%d\n\n", progress->vanguardUpgrades.upgrades[0].currentLevel);

    if (success) {
        syncLoadedUpgrades();
    }

    return success;
}

void syncLoadedUpgrades(void) {
    for (int i = 0; i < vanguardFaction->upgradeCount; i++) {
        vanguardFaction->upgrades[i].currentLevel = 0;
    }
    for (int i = 0; i < crimsonFaction->upgradeCount; i++) {
        crimsonFaction->upgrades[i].currentLevel = 0;
    }

    for (int i = 0; i < progress->vanguardUpgrades.count; i++) {
        int upgradeId = progress->vanguardUpgrades.upgrades[i].upgradeId;
        if (upgradeId >= 0 && upgradeId < vanguardFaction->upgradeCount) {
            vanguardFaction->upgrades[upgradeId].currentLevel =
                progress->vanguardUpgrades.upgrades[i].currentLevel;
        }
    }

    for (int i = 0; i < progress->crimsonUpgrades.count; i++) {
        int upgradeId = progress->crimsonUpgrades.upgrades[i].upgradeId;
        if (upgradeId >= 0 && upgradeId < crimsonFaction->upgradeCount) {
            crimsonFaction->upgrades[upgradeId].currentLevel =
                progress->crimsonUpgrades.upgrades[i].currentLevel;
        }
    }
}

bool createNewSave(const char *filename) {
    ensureSaveDirectory();

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, filename);

    FILE *file = fopen(fullPath, "wb");
    if (!file)
        return false;

    PlayerProgress newProgress = {
        .totalCoins = 0,
        .gems = {0}, 
        .vanguardUpgrades = {0},
        .crimsonUpgrades = {0}};

    SaveFileHeader header = {
        .magicNumber = SAVE_FILE_MAGIC,
        .version = SAVE_VERSION,
        .timestamp = time(NULL),
        .data = newProgress};
    header.checksum = calculateChecksum(&header.data);

    bool success = fwrite(&header, sizeof(header), 1, file) == 1;
    fclose(file);

    if (success) {
        progress = malloc(sizeof(PlayerProgress));
        *progress = newProgress;
    }

    return success;
}

bool saveProgress(const char *filename) {
    if (!progress)
        return false;

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, filename);

    FILE *file = fopen(fullPath, "wb");
    if (!file)
        return false;

    SaveFileHeader header = {
        .magicNumber = SAVE_FILE_MAGIC,
        .version = SAVE_VERSION,
        .timestamp = time(NULL),
        .data = *progress};
    header.checksum = calculateChecksum(&header.data);

    bool success = fwrite(&header, sizeof(header), 1, file) == 1;
    fclose(file);

    printf("Progress saved to %s\n", filename);
    printf("Checksum: %u\n", header.checksum);

    return success;
}

const SaveFileInfo *getSaveFiles(int *count) {
    *count = numSaveFiles;
    return saveFiles;
}

bool importSaveFile(const char *sourcePath) {
    if (!validateSaveFile(sourcePath)) {
        return false;
    }

    char filename[256];
    time_t now = time(NULL);
    strftime(filename, sizeof(filename), "imported_%Y%m%d_%H%M%S.sav", localtime(&now));

    char destPath[512];
    snprintf(destPath, sizeof(destPath), "%s/%s", SAVE_DIR, filename);

    FILE *source = fopen(sourcePath, "rb");
    FILE *dest = fopen(destPath, "wb");

    if (!source || !dest) {
        if (source)
            fclose(source);
        if (dest)
            fclose(dest);
        return false;
    }

    char buffer[4096];
    size_t bytesRead;
    bool success = true;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        if (fwrite(buffer, 1, bytesRead, dest) != bytesRead) {
            success = false;
            break;
        }
    }

    fclose(source);
    fclose(dest);

    if (success) {
        scanSaveFiles();

        if (saveSelectUI) {
            free(saveSelectUI->saveButtons);
            int count;
            const SaveFileInfo *saves = getSaveFiles(&count);

            // Recreate buttons with new data
            saveSelectUI->saveButtons = malloc(sizeof(SaveFileButton) * count);
            saveSelectUI->saveButtonCount = count;

            for (int i = 0; i < count; i++)
            {
                saveSelectUI->saveButtons[i].rect = (SDL_Rect){
                    VIRTUAL_WIDTH / 2 - 200,
                    100 + i * 60,
                    400,
                    50};
                saveSelectUI->saveButtons[i].info = saves[i];
                saveSelectUI->saveButtons[i].isHovered = false;
            }
        }
    }
    else
    {
        remove(destPath); // Clean up failed import
    }

    return success;
}

// In save.c, add new function for deleting saves
bool deleteSave(const char *filename)
{
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, filename);

    if (remove(fullPath) == 0)
    {
        // Refresh save files list after deletion
        scanSaveFiles();
        return true;
    }
    return false;
}

static int compareFileEntries(const void *a, const void *b)
{
    const FileEntry *fa = (const FileEntry *)a;
    const FileEntry *fb = (const FileEntry *)b;

    // Directories come first
    if (fa->isDirectory && !fb->isDirectory)
        return -1;
    if (!fa->isDirectory && fb->isDirectory)
        return 1;

    // Then sort alphabetically
    return strcasecmp(fa->name, fb->name);
}

void initFileBrowser(void) {
    fileBrowser = malloc(sizeof(FileBrowser));
    memset(fileBrowser, 0, sizeof(FileBrowser));

    // Calculate browser window position (centered)
    fileBrowser->browserRect = (SDL_Rect){
        (VIRTUAL_WIDTH - BROWSER_WIDTH) / 2,
        (VIRTUAL_HEIGHT - BROWSER_HEIGHT) / 2,
        BROWSER_WIDTH,
        BROWSER_HEIGHT
    };

    // Calculate button positions relative to browser window
    fileBrowser->upButtonRect = (SDL_Rect){
        fileBrowser->browserRect.x + BROWSER_PADDING,
        fileBrowser->browserRect.y + BROWSER_PADDING + 20, // Space for title
        BROWSER_BUTTON_WIDTH,
        BROWSER_BUTTON_HEIGHT
    };

    fileBrowser->confirmButtonRect = (SDL_Rect){
        fileBrowser->browserRect.x + fileBrowser->browserRect.w - BROWSER_BUTTON_WIDTH * 2 - BROWSER_PADDING * 3,
        fileBrowser->browserRect.y + fileBrowser->browserRect.h - BROWSER_BUTTON_HEIGHT - BROWSER_PADDING,
        BROWSER_BUTTON_WIDTH,
        BROWSER_BUTTON_HEIGHT
    };

    fileBrowser->cancelButtonRect = (SDL_Rect){
        fileBrowser->browserRect.x + fileBrowser->browserRect.w - BROWSER_BUTTON_WIDTH - BROWSER_PADDING,
        fileBrowser->browserRect.y + fileBrowser->browserRect.h - BROWSER_BUTTON_HEIGHT - BROWSER_PADDING,
        BROWSER_BUTTON_WIDTH,
        BROWSER_BUTTON_HEIGHT
    };

    fileBrowser->nextPageRect = (SDL_Rect){
        fileBrowser->browserRect.x + fileBrowser->browserRect.w - BROWSER_BUTTON_WIDTH - BROWSER_PADDING,
        fileBrowser->browserRect.y + fileBrowser->browserRect.h - BROWSER_BUTTON_HEIGHT * 2 - BROWSER_PADDING * 2,
        BROWSER_BUTTON_WIDTH,
        BROWSER_BUTTON_HEIGHT
    };

    fileBrowser->prevPageRect = (SDL_Rect){
        fileBrowser->browserRect.x + BROWSER_PADDING,
        fileBrowser->browserRect.y + fileBrowser->browserRect.h - BROWSER_BUTTON_HEIGHT * 2 - BROWSER_PADDING * 2,
        BROWSER_BUTTON_WIDTH,
        BROWSER_BUTTON_HEIGHT
    };

    fileBrowser->pathRect = (SDL_Rect){
        fileBrowser->browserRect.x + BROWSER_BUTTON_WIDTH + BROWSER_PADDING * 2,
        fileBrowser->browserRect.y + BROWSER_PADDING + 20,
        fileBrowser->browserRect.w - BROWSER_BUTTON_WIDTH - BROWSER_PADDING * 3,
        BROWSER_BUTTON_HEIGHT
    };

#ifdef _WIN32
    strcpy(fileBrowser->currentPath, "C:\\");
#else
    strcpy(fileBrowser->currentPath, getenv("HOME"));
#endif

    fileBrowser->isActive = true;
}

// Modify scanDirectory to handle root directory and ".." navigation properly
void scanDirectory(void)
{
    DIR *dir = opendir(fileBrowser->currentPath);
    if (!dir)
        return;

    fileBrowser->entryCount = 0;
    struct dirent *entry;

    // Add ".." entry if not in root directory
    if (strcmp(fileBrowser->currentPath, "/") != 0)
    {
        FileEntry *fileEntry = &fileBrowser->entries[fileBrowser->entryCount];
        strcpy(fileEntry->name, "..");
        strcpy(fileEntry->fullPath, "..");
        fileEntry->isDirectory = true;
        fileBrowser->entryCount++;
    }

    while ((entry = readdir(dir)) && fileBrowser->entryCount < MAX_FILE_ENTRIES)
    {
        // Skip "." and ".." (we handle ".." separately)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        FileEntry *fileEntry = &fileBrowser->entries[fileBrowser->entryCount];
        strncpy(fileEntry->name, entry->d_name, 255);

        // Construct full path
        if (strcmp(fileBrowser->currentPath, "/") == 0)
        {
            snprintf(fileEntry->fullPath, MAX_PATH_LENGTH, "/%s", entry->d_name);
        }
        else
        {
            snprintf(fileEntry->fullPath, MAX_PATH_LENGTH, "%s/%s",
                     fileBrowser->currentPath, entry->d_name);
        }

        // Check if it's a directory
        struct stat st;
        if (stat(fileEntry->fullPath, &st) == 0)
        {
            fileEntry->isDirectory = S_ISDIR(st.st_mode);
        }

        fileBrowser->entryCount++;
    }

    closedir(dir);

    // Sort entries
    qsort(fileBrowser->entries, fileBrowser->entryCount,
          sizeof(FileEntry), compareFileEntries);

    // Calculate total pages
    fileBrowser->totalPages = (fileBrowser->entryCount + MAX_FILES_PER_PAGE - 1) / MAX_FILES_PER_PAGE;
    if (fileBrowser->currentPage >= fileBrowser->totalPages)
    {
        fileBrowser->currentPage = fileBrowser->totalPages - 1;
    }
    if (fileBrowser->currentPage < 0)
        fileBrowser->currentPage = 0;
}

void drawFileBrowser(void) {
    if (!fileBrowser->isActive)
        return;

    // Draw semi-transparent background overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192);
    SDL_RenderFillRect(renderer, NULL);

    // Draw browser window background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &fileBrowser->browserRect);

    // Draw title
    SDL_Color textColor = {255, 255, 255, 255};
    renderText("Select Save File to Import",
        fileBrowser->browserRect.x + BROWSER_PADDING,
        fileBrowser->browserRect.y + BROWSER_PADDING,
        textColor
    );

    // Draw current path background
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &fileBrowser->pathRect);
    
    // Draw current path with text clipping if needed
    int maxPathChars = (fileBrowser->pathRect.w - BROWSER_PADDING * 2) / 6; // Approximate width per character
    char displayPath[256];
    if ((int)strlen(fileBrowser->currentPath) > maxPathChars) {
        snprintf(displayPath, sizeof(displayPath), "...%s", 
            fileBrowser->currentPath + strlen(fileBrowser->currentPath) - maxPathChars + 3);
    } else {
        strncpy(displayPath, fileBrowser->currentPath, sizeof(displayPath));
    }
    
    renderText(displayPath,
        fileBrowser->pathRect.x + BROWSER_PADDING,
        fileBrowser->pathRect.y + (BROWSER_BUTTON_HEIGHT - 8) / 2, // Center text vertically
        textColor
    );

    // Draw Up button
    drawButton("Up", fileBrowser->upButtonRect, fileBrowser->upHovered);

    // Calculate file list area
    int listStartY = fileBrowser->browserRect.y + BROWSER_PADDING * 2 + BROWSER_BUTTON_HEIGHT + 20;
    int listHeight = fileBrowser->browserRect.h - BROWSER_BUTTON_HEIGHT * 3 - BROWSER_PADDING * 6 - 20;
    int entriesPerPage = listHeight / (FILE_ENTRY_HEIGHT + FILE_ENTRY_SPACING);
    
    // Draw file entries
    int startIdx = fileBrowser->currentPage * entriesPerPage;
    int endIdx = startIdx + entriesPerPage;
    if (endIdx > fileBrowser->entryCount) {
        endIdx = fileBrowser->entryCount;
    }

    for (int i = startIdx; i < endIdx; i++) {
        SDL_Rect entryRect = {
            fileBrowser->browserRect.x + BROWSER_PADDING,
            listStartY + (i - startIdx) * (FILE_ENTRY_HEIGHT + FILE_ENTRY_SPACING),
            fileBrowser->browserRect.w - BROWSER_PADDING * 2,
            FILE_ENTRY_HEIGHT
        };

        // Highlight if selected
        if (&fileBrowser->entries[i] == fileBrowser->selectedFile) {
            SDL_SetRenderDrawColor(renderer, 70, 70, 100, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        }
        SDL_RenderFillRect(renderer, &entryRect);

        // Draw icon and name
        char display[256];
        if (fileBrowser->entries[i].isDirectory) {
            snprintf(display, sizeof(display), "📁 %s", fileBrowser->entries[i].name);
        } else {
            snprintf(display, sizeof(display), "📄 %s", fileBrowser->entries[i].name);
        }
        renderText(display, 
            entryRect.x + BROWSER_PADDING,
            entryRect.y + (FILE_ENTRY_HEIGHT - 8) / 2, // Center text vertically
            textColor
        );
    }

    // Draw navigation buttons if needed
    if (fileBrowser->totalPages > 1) {
        if (fileBrowser->currentPage > 0) {
            drawButton("Prev", fileBrowser->prevPageRect, fileBrowser->prevHovered);
        }
        if (fileBrowser->currentPage < fileBrowser->totalPages - 1) {
            drawButton("Next", fileBrowser->nextPageRect, fileBrowser->nextHovered);
        }
    }

    // Draw action buttons
    drawButton("Import", fileBrowser->confirmButtonRect, fileBrowser->confirmHovered);
    drawButton("Cancel", fileBrowser->cancelButtonRect, fileBrowser->cancelHovered);
}

// Modify handleFileBrowserInput to handle improved directory navigation
bool handleFileBrowserInput(void) {
    if (!fileBrowser->isActive)
        return false;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Convert window coordinates to virtual coordinates
    int virtualMouseX, virtualMouseY;
    windowToVirtual(mouseX, mouseY, &virtualMouseX, &virtualMouseY);

    // Update hover states using virtual coordinates
    fileBrowser->upHovered = isMouseOverButton(virtualMouseX, virtualMouseY, fileBrowser->upButtonRect);
    fileBrowser->confirmHovered = isMouseOverButton(virtualMouseX, virtualMouseY, fileBrowser->confirmButtonRect);
    fileBrowser->cancelHovered = isMouseOverButton(virtualMouseX, virtualMouseY, fileBrowser->cancelButtonRect);
    fileBrowser->nextHovered = isMouseOverButton(virtualMouseX, virtualMouseY, fileBrowser->nextPageRect);
    fileBrowser->prevHovered = isMouseOverButton(virtualMouseX, virtualMouseY, fileBrowser->prevPageRect);

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        // Handle Up button
        if (fileBrowser->upHovered) {
            if (strcmp(fileBrowser->currentPath, "/") != 0) {
                char *lastSlash = strrchr(fileBrowser->currentPath, '/');
                if (lastSlash && lastSlash != fileBrowser->currentPath) {
                    *lastSlash = '\0';
                } else {
                    strcpy(fileBrowser->currentPath, "/");
                }
                fileBrowser->currentPage = 0;
                scanDirectory();
            }
            return true;
        }

        // Handle navigation buttons
        if (fileBrowser->prevHovered && fileBrowser->currentPage > 0) {
            fileBrowser->currentPage--;
            return true;
        }
        if (fileBrowser->nextHovered && fileBrowser->currentPage < fileBrowser->totalPages - 1) {
            fileBrowser->currentPage++;
            return true;
        }

        // Handle Cancel button
        if (fileBrowser->cancelHovered) {
            fileBrowser->isActive = false;
            return true;
        }

        // Handle Confirm button
        if (fileBrowser->confirmHovered && fileBrowser->selectedFile) {
            if (!fileBrowser->selectedFile->isDirectory) {
                if (importSaveFile(fileBrowser->selectedFile->fullPath)) {
                    int count;
                    const SaveFileInfo *saves = getSaveFiles(&count);
                    if (count > 0) {
                        const SaveFileInfo *lastSave = &saves[count - 1];
                        if (loadSave(lastSave->filename)) {
                            fileBrowser->isActive = false;
                            gameContext->currentState = STATE_HUB;
                        }
                    }
                    return true;
                }
            }
        }

        // Calculate file list area
        int listStartY = fileBrowser->browserRect.y + BROWSER_PADDING * 2 + BROWSER_BUTTON_HEIGHT + 20;
        int listHeight = fileBrowser->browserRect.h - BROWSER_BUTTON_HEIGHT * 3 - BROWSER_PADDING * 6 - 20;
        int entriesPerPage = listHeight / (FILE_ENTRY_HEIGHT + FILE_ENTRY_SPACING);
        
        int startIdx = fileBrowser->currentPage * entriesPerPage;
        int endIdx = startIdx + entriesPerPage;
        if (endIdx > fileBrowser->entryCount) {
            endIdx = fileBrowser->entryCount;
        }

        // Handle file/directory selection
        for (int i = startIdx; i < endIdx; i++) {
            SDL_Rect entryRect = {
                fileBrowser->browserRect.x + BROWSER_PADDING,
                listStartY + (i - startIdx) * (FILE_ENTRY_HEIGHT + FILE_ENTRY_SPACING),
                fileBrowser->browserRect.w - BROWSER_PADDING * 2,
                FILE_ENTRY_HEIGHT
            };

            if (isMouseOverButton(virtualMouseX, virtualMouseY, entryRect)) {
                if (fileBrowser->entries[i].isDirectory) {
                    if (strcmp(fileBrowser->entries[i].name, "..") == 0) {
                        // Handle ".." directory
                        if (strcmp(fileBrowser->currentPath, "/") != 0) {
                            char *lastSlash = strrchr(fileBrowser->currentPath, '/');
                            if (lastSlash && lastSlash != fileBrowser->currentPath) {
                                *lastSlash = '\0';
                            } else {
                                strcpy(fileBrowser->currentPath, "/");
                            }
                        }
                    } else {
                        // Handle regular directory navigation
                        if (strcmp(fileBrowser->currentPath, "/") == 0) {
                            snprintf(fileBrowser->currentPath, MAX_PATH_LENGTH, "/%s",
                                fileBrowser->entries[i].name);
                        } else {
                            char tempPath[MAX_PATH_LENGTH];
                            strncpy(tempPath, fileBrowser->currentPath, MAX_PATH_LENGTH - 1);
                            snprintf(fileBrowser->currentPath, MAX_PATH_LENGTH, "%s/%s",
                                tempPath, fileBrowser->entries[i].name);
                        }
                    }
                    fileBrowser->currentPage = 0;
                    scanDirectory();
                } else {
                    fileBrowser->selectedFile = &fileBrowser->entries[i];
                }
                return true;
            }
        }
    }

    return false;
}

void cleanupFileBrowser(void)
{
    if (fileBrowser)
    {
        free(fileBrowser);
        fileBrowser = NULL;
    }
}
