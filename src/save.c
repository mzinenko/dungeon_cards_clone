#include "header.h"

// Calculate a simple checksum
static uint32_t calculateChecksum(const PlayerProgress* data) {
    uint32_t checksum = 0;
    const uint8_t* bytes = (const uint8_t*)data;
    for(size_t i = 0; i < sizeof(PlayerProgress); i++) {
        checksum = (checksum << 1) | (checksum >> 31);  // Rotate left
        checksum ^= bytes[i];
    }
    return checksum;
}

// Create saves directory if it doesn't exist
static void ensureSaveDirectory() {
    #ifdef _WIN32
        _mkdir(SAVE_DIR);
    #else
        mkdir(SAVE_DIR, 0777);
    #endif
}

static bool validateSaveFile(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return false;

    SaveFileHeader header;
    size_t read = fread(&header, sizeof(header), 1, file);
    fclose(file);

    if (read != 1) return false;
    if (header.magicNumber != SAVE_FILE_MAGIC) return false;
    if (header.version != 1 && header.version != SAVE_VERSION) return false;  // Accept both versions

    // Only check checksum for current version
    if (header.version == SAVE_VERSION) {
        uint32_t checksum = calculateChecksum(&header.data);
        return checksum == header.checksum;
    }

    return true;  // Accept old version without checksum validation
}

// Scan for save files
void scanSaveFiles() {
    printf("Scanning for save files...\n");
    ensureSaveDirectory();
    numSaveFiles = 0;

    DIR* dir = opendir(SAVE_DIR);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && numSaveFiles < MAX_SAVE_FILES) {
        if (entry->d_type == DT_REG) {  // Regular file
            char fullPath[512];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, entry->d_name);

            struct stat st;
            if (stat(fullPath, &st) == 0) {
                SaveFileInfo* info = &saveFiles[numSaveFiles];
                strncpy(info->filename, entry->d_name, sizeof(info->filename) - 1);
                info->lastModified = st.st_mtime;
                info->isValid = validateSaveFile(fullPath);

                if (info->isValid) {
                    // Load preview data
                    FILE* file = fopen(fullPath, "rb");
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

bool createNewSave(const char* filename) {
    ensureSaveDirectory();
    
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, filename);

    FILE* file = fopen(fullPath, "wb");
    if (!file) return false;

    // Initialize new progress with gems
    PlayerProgress newProgress = {
        .totalCoins = 0,
        .maxHpUpgrade = 0,
        .attackUpgrade = 0,
        .defenseUpgrade = 0
    };
    
    // Initialize all gems to 0
    for (int i = 0; i < 5; i++) {
        newProgress.gems[i] = 0;
    }

    // Create header
    SaveFileHeader header = {
        .magicNumber = SAVE_FILE_MAGIC,
        .version = SAVE_VERSION,
        .timestamp = time(NULL),
        .data = newProgress
    };
    header.checksum = calculateChecksum(&header.data);

    bool success = fwrite(&header, sizeof(header), 1, file) == 1;
    fclose(file);

    if (success) {
        progress = malloc(sizeof(PlayerProgress));
        *progress = newProgress;
    }

    return success;
}

// Add version handling to loadSave function
bool loadSave(const char* filename) {
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, filename);

    FILE* file = fopen(fullPath, "rb");
    if (!file) return false;

    SaveFileHeader header;
    bool success = false;

    if (fread(&header, sizeof(header), 1, file) == 1) {
        if (header.magicNumber == SAVE_FILE_MAGIC) {
            // Handle different versions
            if (header.version == 1) {
                // Convert version 1 to version 2
                PlayerProgress newProgress = {
                    .totalCoins = header.data.totalCoins,
                    .maxHpUpgrade = header.data.maxHpUpgrade,
                    .attackUpgrade = header.data.attackUpgrade,
                    .defenseUpgrade = header.data.defenseUpgrade
                };
                // Initialize new fields
                for (int i = 0; i < 5; i++) {
                    newProgress.gems[i] = 0;
                }
                progress = malloc(sizeof(PlayerProgress));
                *progress = newProgress;
                success = true;
            } else if (header.version == SAVE_VERSION) {
                // Current version
                if (calculateChecksum(&header.data) == header.checksum) {
                    progress = malloc(sizeof(PlayerProgress));
                    *progress = header.data;
                    success = true;
                }
            }
        }
    }

    fclose(file);
    return success;
}

// Update saveProgress function to use new version
bool saveProgress(const char* filename) {
    if (!progress) return false;

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, filename);

    FILE* file = fopen(fullPath, "wb");
    if (!file) return false;

    SaveFileHeader header = {
        .magicNumber = SAVE_FILE_MAGIC,
        .version = SAVE_VERSION,  // Use new version
        .timestamp = time(NULL),
        .data = *progress
    };
    header.checksum = calculateChecksum(&header.data);

    bool success = fwrite(&header, sizeof(header), 1, file) == 1;
    fclose(file);

    printf("Progress saved to %s\n", filename);
    printf("Checksum: %u\n", header.checksum);

    return success;
}

// Get all save files
const SaveFileInfo* getSaveFiles(int* count) {
    *count = numSaveFiles;
    return saveFiles;
}

// Import an external save file
bool importSaveFile(const char* sourcePath) {
    // First validate the file
    if (!validateSaveFile(sourcePath)) {
        return false;
    }

    // Generate a unique filename
    char filename[256];
    time_t now = time(NULL);
    strftime(filename, sizeof(filename), "imported_%Y%m%d_%H%M%S.sav", localtime(&now));

    // Copy file to saves directory
    char destPath[512];
    snprintf(destPath, sizeof(destPath), "%s/%s", SAVE_DIR, filename);

    FILE* source = fopen(sourcePath, "rb");
    FILE* dest = fopen(destPath, "wb");

    if (!source || !dest) {
        if (source) fclose(source);
        if (dest) fclose(dest);
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
        scanSaveFiles();  // Refresh save file list
        
        // Force refresh of save selection UI
        if (saveSelectUI) {
            // Free existing buttons
            free(saveSelectUI->saveButtons);
            
            // Get updated save files
            int count;
            const SaveFileInfo* saves = getSaveFiles(&count);
            
            // Recreate buttons with new data
            saveSelectUI->saveButtons = malloc(sizeof(SaveFileButton) * count);
            saveSelectUI->saveButtonCount = count;
            
            for (int i = 0; i < count; i++) {
                saveSelectUI->saveButtons[i].rect = (SDL_Rect){
                    WINDOW_WIDTH/2 - 200,
                    100 + i * 60,
                    400,
                    50
                };
                saveSelectUI->saveButtons[i].info = saves[i];
                saveSelectUI->saveButtons[i].isHovered = false;
            }
        }
    } else {
        remove(destPath);  // Clean up failed import
    }

    return success;
}

// In save.c, add new function for deleting saves
bool deleteSave(const char* filename) {
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVE_DIR, filename);
    
    if (remove(fullPath) == 0) {
        // Refresh save files list after deletion
        scanSaveFiles();
        return true;
    }
    return false;
}

static int compareFileEntries(const void* a, const void* b) {
    const FileEntry* fa = (const FileEntry*)a;
    const FileEntry* fb = (const FileEntry*)b;
    
    // Directories come first
    if (fa->isDirectory && !fb->isDirectory) return -1;
    if (!fa->isDirectory && fb->isDirectory) return 1;
    
    // Then sort alphabetically
    return strcasecmp(fa->name, fb->name);
}


void initFileBrowser() {
    fileBrowser = malloc(sizeof(FileBrowser));
    memset(fileBrowser, 0, sizeof(FileBrowser));
    
    // Set up the browser window in the center of the screen
    fileBrowser->browserRect = (SDL_Rect){
        WINDOW_WIDTH/2 - 400,
        WINDOW_HEIGHT/2 - 300,
        800,
        600
    };
    
    // Set up buttons
    fileBrowser->upButtonRect = (SDL_Rect){
        fileBrowser->browserRect.x + 10,
        fileBrowser->browserRect.y + 50,
        80,
        30
    };
    
    fileBrowser->confirmButtonRect = (SDL_Rect){
        fileBrowser->browserRect.x + fileBrowser->browserRect.w - 220,
        fileBrowser->browserRect.y + fileBrowser->browserRect.h - 40,
        100,
        30
    };
    
    fileBrowser->cancelButtonRect = (SDL_Rect){
        fileBrowser->browserRect.x + fileBrowser->browserRect.w - 110,
        fileBrowser->browserRect.y + fileBrowser->browserRect.h - 40,
        100,
        30
    };
    
    fileBrowser->nextPageRect = (SDL_Rect){
        fileBrowser->browserRect.x + fileBrowser->browserRect.w - 90,
        fileBrowser->browserRect.y + fileBrowser->browserRect.h - 80,
        80,
        30
    };
    
    fileBrowser->prevPageRect = (SDL_Rect){
        fileBrowser->browserRect.x + 10,
        fileBrowser->browserRect.y + fileBrowser->browserRect.h - 80,
        80,
        30
    };
    
    fileBrowser->pathRect = (SDL_Rect){
        fileBrowser->browserRect.x + 100,
        fileBrowser->browserRect.y + 50,
        fileBrowser->browserRect.w - 120,
        30
    };

    // Get starting directory
    #ifdef _WIN32
        strcpy(fileBrowser->currentPath, "C:\\");
    #else
        strcpy(fileBrowser->currentPath, getenv("HOME"));
    #endif
    
    fileBrowser->isActive = true;
}

// Modify scanDirectory to handle root directory and ".." navigation properly
void scanDirectory() {
    DIR* dir = opendir(fileBrowser->currentPath);
    if (!dir) return;

    fileBrowser->entryCount = 0;
    struct dirent* entry;

    // Add ".." entry if not in root directory
    if (strcmp(fileBrowser->currentPath, "/") != 0) {
        FileEntry* fileEntry = &fileBrowser->entries[fileBrowser->entryCount];
        strcpy(fileEntry->name, "..");
        strcpy(fileEntry->fullPath, "..");
        fileEntry->isDirectory = true;
        fileBrowser->entryCount++;
    }

    while ((entry = readdir(dir)) && fileBrowser->entryCount < MAX_FILE_ENTRIES) {
        // Skip "." and ".." (we handle ".." separately)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) 
            continue;

        FileEntry* fileEntry = &fileBrowser->entries[fileBrowser->entryCount];
        strncpy(fileEntry->name, entry->d_name, 255);
        
        // Construct full path
        if (strcmp(fileBrowser->currentPath, "/") == 0) {
            snprintf(fileEntry->fullPath, MAX_PATH_LENGTH, "/%s", entry->d_name);
        } else {
            snprintf(fileEntry->fullPath, MAX_PATH_LENGTH, "%s/%s", 
                    fileBrowser->currentPath, entry->d_name);
        }
        
        // Check if it's a directory
        struct stat st;
        if (stat(fileEntry->fullPath, &st) == 0) {
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
    if (fileBrowser->currentPage >= fileBrowser->totalPages) {
        fileBrowser->currentPage = fileBrowser->totalPages - 1;
    }
    if (fileBrowser->currentPage < 0) fileBrowser->currentPage = 0;
}

void drawFileBrowser() {
    if (!fileBrowser->isActive) return;

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
              fileBrowser->browserRect.x + 10,
              fileBrowser->browserRect.y + 10,
              textColor);

    // Draw current path
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &fileBrowser->pathRect);
    renderText(fileBrowser->currentPath,
              fileBrowser->pathRect.x + 5,
              fileBrowser->pathRect.y + 5,
              textColor);

    // Draw Up button
    drawButton("Up", fileBrowser->upButtonRect, fileBrowser->upHovered);

    // Draw file entries
    int startY = fileBrowser->browserRect.y + 100;
    int startIdx = fileBrowser->currentPage * MAX_FILES_PER_PAGE;
    int endIdx = startIdx + MAX_FILES_PER_PAGE;
    if (endIdx > fileBrowser->entryCount) endIdx = fileBrowser->entryCount;

    for (int i = startIdx; i < endIdx; i++) {
        SDL_Rect entryRect = {
            fileBrowser->browserRect.x + 10,
            startY + (i - startIdx) * 40,
            fileBrowser->browserRect.w - 20,
            35
        };

        // Highlight if selected
        if (&fileBrowser->entries[i] == fileBrowser->selectedFile) {
            SDL_SetRenderDrawColor(renderer, 70, 70, 100, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        }
        SDL_RenderFillRect(renderer, &entryRect);

        // Draw icon/type indicator and name
        char display[256];
        if (fileBrowser->entries[i].isDirectory) {
            snprintf(display, sizeof(display), "📁 %s", fileBrowser->entries[i].name);
        } else {
            snprintf(display, sizeof(display), "📄 %s", fileBrowser->entries[i].name);
        }
        renderText(display, entryRect.x + 5, entryRect.y + 8, textColor);
    }

    // Draw navigation buttons
    if (fileBrowser->totalPages > 1) {
        if (fileBrowser->currentPage > 0) {
            drawButton("Previous", fileBrowser->prevPageRect, fileBrowser->prevHovered);
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
bool handleFileBrowserInput() {
    if (!fileBrowser->isActive) return false;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Update hover states
    fileBrowser->upHovered = isMouseOverButton(mouseX, mouseY, fileBrowser->upButtonRect);
    fileBrowser->confirmHovered = isMouseOverButton(mouseX, mouseY, fileBrowser->confirmButtonRect);
    fileBrowser->cancelHovered = isMouseOverButton(mouseX, mouseY, fileBrowser->cancelButtonRect);
    fileBrowser->nextHovered = isMouseOverButton(mouseX, mouseY, fileBrowser->nextPageRect);
    fileBrowser->prevHovered = isMouseOverButton(mouseX, mouseY, fileBrowser->prevPageRect);

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        // Handle Up button
        if (fileBrowser->upHovered) {
            if (strcmp(fileBrowser->currentPath, "/") != 0) {
                char* lastSlash = strrchr(fileBrowser->currentPath, '/');
                if (lastSlash && lastSlash != fileBrowser->currentPath) {
                    *lastSlash = '\0';
                } else {
                    // We're going to root
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
                    // Get the most recently added save file
                    int count;
                    const SaveFileInfo* saves = getSaveFiles(&count);
                    if (count > 0) {
                        // Load the last save file (the one we just imported)
                        const SaveFileInfo* lastSave = &saves[count - 1];
                        if (loadSave(lastSave->filename)) {
                            fileBrowser->isActive = false;
                            gameContext->currentState = STATE_HUB;
                        }
                    }
                    return true;
                }
            }
        }

        // Handle file/directory selection
        int startY = fileBrowser->browserRect.y + 100;
        int startIdx = fileBrowser->currentPage * MAX_FILES_PER_PAGE;
        int endIdx = startIdx + MAX_FILES_PER_PAGE;
        if (endIdx > fileBrowser->entryCount) endIdx = fileBrowser->entryCount;

        for (int i = startIdx; i < endIdx; i++) {
            SDL_Rect entryRect = {
                fileBrowser->browserRect.x + 10,
                startY + (i - startIdx) * 40,
                fileBrowser->browserRect.w - 20,
                35
            };

            if (isMouseOverButton(mouseX, mouseY, entryRect)) {
                if (fileBrowser->entries[i].isDirectory) {
                    if (strcmp(fileBrowser->entries[i].name, "..") == 0) {
                        // Handle ".." directory
                        if (strcmp(fileBrowser->currentPath, "/") != 0) {
                            char* lastSlash = strrchr(fileBrowser->currentPath, '/');
                            if (lastSlash && lastSlash != fileBrowser->currentPath) {
                                *lastSlash = '\0';
                            } else {
                                strcpy(fileBrowser->currentPath, "/");
                            }
                        }
                    } else {
                        // Handle regular directory
                        // Properly handle directory path concatenation
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

void cleanupFileBrowser() {
    if (fileBrowser) {
        free(fileBrowser);
        fileBrowser = NULL;
    }
}
