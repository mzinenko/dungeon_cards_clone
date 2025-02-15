#include "../inc/header.h"

void initAudioManager(void) {
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }

    audioManager = malloc(sizeof(AudioManager));
    audioManager->soundCount = 0;
    audioManager->musicVolume = DEFAULT_MUSIC_VOLUME;
    audioManager->isMuted = false;

    // Set initial volume
    Mix_Volume(-1, audioManager->musicVolume);
}

void playSound(const char *path) {
    if (audioManager->isMuted) {
        return;
    }

    // Check if sound is already loaded
    for (int i = 0; i < audioManager->soundCount; i++) {
        if (strcmp(audioManager->sounds[i].path, path) == 0) {
            if (!audioManager->sounds[i].isPlaying) {
                audioManager->sounds[i].channel = Mix_PlayChannel(-1, audioManager->sounds[i].sound, 0);
                audioManager->sounds[i].isPlaying = true;
            }
            return;
        }
    }

    // If we have room for a new sound
    if (audioManager->soundCount < MAX_SOUNDS) {
        Sound *newSound = &audioManager->sounds[audioManager->soundCount];
        newSound->path = strdup(path);
        newSound->sound = Mix_LoadWAV(path);
        
        if (newSound->sound) {
            newSound->channel = Mix_PlayChannel(-1, newSound->sound, 0);
            newSound->isPlaying = true;
            audioManager->soundCount++;
        } else {
            printf("Failed to load sound: %s\n", Mix_GetError());
            free(newSound->path);
        }
    }
}

void stopSound(const char *path) {
    for (int i = 0; i < audioManager->soundCount; i++) {
        if (strcmp(audioManager->sounds[i].path, path) == 0) {
            if (audioManager->sounds[i].isPlaying) {
                Mix_HaltChannel(audioManager->sounds[i].channel);
                audioManager->sounds[i].isPlaying = false;
            }
            break;
        }
    }
}

void stopAllSounds(void) {
    Mix_HaltChannel(-1);  // Stop all channels
    for (int i = 0; i < audioManager->soundCount; i++) {
        audioManager->sounds[i].isPlaying = false;
    }
}

void setMusicVolume(int volume) {
    audioManager->musicVolume = volume;
    if (!audioManager->isMuted) {
        Mix_Volume(-1, volume);
    }
}

void toggleMute(void) {
    audioManager->isMuted = !audioManager->isMuted;
    Mix_Volume(-1, audioManager->isMuted ? 0 : audioManager->musicVolume);
}

void cleanupAudioManager(void) {
    if (audioManager) {
        for (int i = 0; i < audioManager->soundCount; i++) {
            Mix_FreeChunk(audioManager->sounds[i].sound);
            free(audioManager->sounds[i].path);
        }
        free(audioManager);
        Mix_CloseAudio();
    }
}
