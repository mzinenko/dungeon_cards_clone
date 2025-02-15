#include "../inc/header.h"

void initAudioManager(void) {
    // Initialize SDL_mixer with optimal settings
    if (Mix_OpenAudio(AUDIO_FREQUENCY, AUDIO_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNKSIZE) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }

    audioManager = malloc(sizeof(AudioManager));
    if (!audioManager) {
        printf("Failed to allocate memory for audio manager\n");
        return;
    }

    // Initialize sound effects system
    audioManager->soundCount = 0;
    audioManager->soundVolume = DEFAULT_SOUND_VOLUME;
    
    // Initialize music system
    audioManager->currentMusic = NULL;
    audioManager->currentMusicPath = NULL;
    audioManager->musicVolume = DEFAULT_MUSIC_VOLUME;
    audioManager->isMuted = false;

    // Allocate mixing channels
    Mix_AllocateChannels(16);
    
    // Set initial volumes
    Mix_Volume(-1, audioManager->soundVolume);  // Set sound effect volume
    Mix_VolumeMusic(audioManager->musicVolume); // Set music volume
}

// Sound effects functions
Sound* findSound(const char* path) {
    for (int i = 0; i < audioManager->soundCount; i++) {
        if (strcmp(audioManager->sounds[i].path, path) == 0) {
            return &audioManager->sounds[i];
        }
    }
    return NULL;
}

void playSound(const char *path) {
    if (!audioManager || audioManager->isMuted) {
        return;
    }

    Sound* existingSound = findSound(path);
    if (existingSound) {
        // Reuse existing sound
        existingSound->channel = Mix_PlayChannel(-1, existingSound->sound, 0);
        existingSound->isPlaying = true;
        return;
    }

    // Load new sound if we have room
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
    Sound* sound = findSound(path);
    if (sound && sound->isPlaying) {
        Mix_HaltChannel(sound->channel);
        sound->isPlaying = false;
    }
}

void stopAllSounds(void) {
    Mix_HaltChannel(-1);  // Stop all channels
    for (int i = 0; i < audioManager->soundCount; i++) {
        audioManager->sounds[i].isPlaying = false;
    }
}

void setSoundVolume(int volume) {
    if (!audioManager) return;
    
    // Clamp volume to valid range
    volume = (volume < 0) ? 0 : (volume > 128) ? 128 : volume;
    
    audioManager->soundVolume = volume;
    if (!audioManager->isMuted) {
        Mix_Volume(-1, volume);
    }
}

// Background music functions
void playMusic(const char *path, bool loop) {
    if (!audioManager) return;

    // Stop any currently playing music
    if (audioManager->currentMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(audioManager->currentMusic);
        free(audioManager->currentMusicPath);
        audioManager->currentMusic = NULL;
        audioManager->currentMusicPath = NULL;
    }

    // Load and play new music
    audioManager->currentMusic = Mix_LoadMUS(path);
    if (audioManager->currentMusic) {
        audioManager->currentMusicPath = strdup(path);
        Mix_PlayMusic(audioManager->currentMusic, loop ? -1 : 0);
        Mix_VolumeMusic(audioManager->isMuted ? 0 : audioManager->musicVolume);
    } else {
        printf("Failed to load music: %s\n", Mix_GetError());
    }
}

void stopMusic(void) {
    if (!audioManager || !audioManager->currentMusic) return;
    
    Mix_HaltMusic();
    Mix_FreeMusic(audioManager->currentMusic);
    free(audioManager->currentMusicPath);
    audioManager->currentMusic = NULL;
    audioManager->currentMusicPath = NULL;
}

void pauseMusic(void) {
    if (Mix_PlayingMusic()) {
        Mix_PauseMusic();
    }
}

void resumeMusic(void) {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

void setMusicVolume(int volume) {
    if (!audioManager) return;
    
    // Clamp volume to valid range
    volume = (volume < 0) ? 0 : (volume > 128) ? 128 : volume;
    
    audioManager->musicVolume = volume;
    if (!audioManager->isMuted) {
        Mix_VolumeMusic(volume);
    }
}

// Global audio controls
void toggleMute(void) {
    if (!audioManager) return;
    
    audioManager->isMuted = !audioManager->isMuted;
    Mix_Volume(-1, audioManager->isMuted ? 0 : audioManager->soundVolume);
    Mix_VolumeMusic(audioManager->isMuted ? 0 : audioManager->musicVolume);
}

void cleanupAudioManager(void) {
    if (audioManager) {
        stopAllSounds();
        stopMusic();
        
        // Free all loaded sounds
        for (int i = 0; i < audioManager->soundCount; i++) {
            if (audioManager->sounds[i].sound) {
                Mix_FreeChunk(audioManager->sounds[i].sound);
            }
            if (audioManager->sounds[i].path) {
                free(audioManager->sounds[i].path);
            }
        }
        
        free(audioManager);
        audioManager = NULL;
        
        Mix_CloseAudio();
    }
}
