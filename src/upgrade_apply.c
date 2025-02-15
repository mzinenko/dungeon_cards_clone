#include "../inc/header.h"

void applyCombatTraining(int currentLevel) {
    player->attack.meleeDamage += 1 * currentLevel;
}

void applyBallisticExpertise(int currentLevel) {
    player->base.damage += 1 * currentLevel;
}