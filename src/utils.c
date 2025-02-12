#include "header.h"

bool isInRange(int x1, int y1, int x2, int y2, int range) {
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);

    // Check if enemy is within attack range
    return (dx <= range && dy == 0) || 
           (dy <= range && dx == 0) ||
           (dx <= range && dy <= range);
}

float frandomInRange(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

int randomInRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}

char* mx_strdup(const char* str) {
    size_t len = strlen(str) + 1;
    char* new_str = malloc(len);
    if (new_str) {
        memcpy(new_str, str, len);
    }
    return new_str;
}
