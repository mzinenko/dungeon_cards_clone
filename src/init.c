#include "../inc/header.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

Player *player = NULL;
Grid *grid = NULL;
GameContext *gameContext = NULL;
Store *store = NULL;
PlayerProgress *progress = NULL;
CardAnimation *cardAnim = NULL;

SDL_Event event;

Texture *eventObserverTextures;
Texture *enemyTextures;
Texture *coinTextures;
Texture *heroTextures;
Texture *potionTextures;
Texture *uiTextures;

size_t eventObserverTexturesCount = 0;
size_t enemyTexturesCount = 0;
size_t coinTexturesCount = 0;
size_t heroTexturesCount = 0;
size_t potionTexturesCount = 0;
size_t uiTexturesCount = 0;

bool isShooting = false;
bool isPlayerTurn = true;

Projectile *projectiles = NULL;
int projectileCount = 0;

int turn = 0;

EventObserver *eventObserver = NULL;

AnimationManager *animationManager = NULL;

MenuButton *menuButtons = NULL;

HubInterface *hubInterface = NULL;

SaveFileInfo saveFiles[MAX_SAVE_FILES];
int numSaveFiles = 0;

SaveSelectUI *saveSelectUI = NULL;

FileBrowser *fileBrowser = NULL;

Faction *vanguardFaction = NULL;
Faction *crimsonFaction = NULL;
Faction *currentFaction = NULL;

bool isDev = false;

RenderContext* renderCtx = NULL;

FactionUpgrade vanguardUpgrades[MAX_FACTION_UPGRADES] = {
    {.name = "Grid Lockdown Protocol",
     .description = "Enemy cards cannot spawn adjacent to you after movement",
     .maxLevel = 1,
     .currentLevel = 0,
     .baseCost = 2000,
     .gemCosts = {0, 0, 0, 0, 5},
     .requiredRank = 80,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Tactical Retreat System",
     .description = "Once per run, can move without triggering card interactions",
     .maxLevel = 1,
     .currentLevel = 0,
     .baseCost = 1500,
     .gemCosts = {0, 0, 0, 3, 0},
     .requiredRank = 70,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Defense Matrix",
     .description = "When picking up a card, gain 1 extra armor",
     .maxLevel = 1,
     .currentLevel = 0,
     .baseCost = 1200,
     .gemCosts = {0, 0, 8, 0, 0},
     .requiredRank = 50,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Combat Analysis Protocol",
     .description = "After defeating an enemy, gain 1 armor if your HP is below 50%",
     .maxLevel = 1,
     .currentLevel = 0,
     .baseCost = 1000,
     .gemCosts = {0, 10, 0, 0, 0},
     .requiredRank = 40,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Combat Training",
     .description = "+1 melee damage",
     .maxLevel = 5,
     .currentLevel = 0,
     .baseCost = 200,
     .gemCosts = {3, 0, 0, 0, 0},
     .requiredRank = 10,
     .applyOn = APPLY_ON_START,
     .applyEffect = applyCombatTraining},
    {.name = "Ballistic Expertise",
     .description = "+1 ranged damage",
     .maxLevel = 3,
     .currentLevel = 0,
     .baseCost = 400,
     .gemCosts = {0, 4, 0, 0, 0},
     .requiredRank = 20,
     .applyOn = APPLY_ON_START,
     .applyEffect = applyBallisticExpertise},
    {.name = "Tactical Gear",
     .description = "+1 maximum armor",
     .maxLevel = 3,
     .currentLevel = 0,
     .baseCost = 300,
     .gemCosts = {0, 0, 3, 0, 0},
     .requiredRank = 15,
     .applyOn = APPLY_ON_START,
     .applyEffect = NULL},
    {.name = "Field Medic",
     .description = "+2 maximum HP",
     .maxLevel = 2,
     .currentLevel = 0,
     .baseCost = 500,
     .gemCosts = {0, 0, 0, 5, 0},
     .requiredRank = 25,
     .applyOn = APPLY_ON_START,
     .applyEffect = NULL},
    {.name = "Extended Mag",
     .description = "+1 starting ammo",
     .maxLevel = 2,
     .currentLevel = 0,
     .baseCost = 400,
     .gemCosts = {0, 0, 4, 0, 0},
     .requiredRank = 30,
     .applyOn = APPLY_ON_START,
     .applyEffect = NULL},
    {.name = "Gem Resonance",
     .description = "+5% chance to get an additional gem of the same rarity from coin cards",
     .maxLevel = 3,
     .currentLevel = 0,
     .baseCost = 300,
     .gemCosts = {5, 0, 0, 0, 0},
     .requiredRank = 20,
     .applyOn = APPLY_ON_START,
     .applyEffect = NULL}};

FactionUpgrade crimsonUpgrades[MAX_FACTION_UPGRADES] = {
    {.name = "Blood Bond Protocol",
     .description = "Once per run, when HP drops to 1, transform into a chaos beast for 3 turns",
     .maxLevel = 1,
     .currentLevel = 0,
     .baseCost = 2000,
     .gemCosts = {0, 0, 0, 0, 5},
     .requiredRank = 80,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Pack Hunter's Instinct",
     .description = "First attack against each new enemy card has +2 damage",
     .maxLevel = 1,
     .currentLevel = 0,
     .baseCost = 1500,
     .gemCosts = {0, 0, 0, 3, 0},
     .requiredRank = 70,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Predator's Reflexes",
     .description = "15% chance to dodge enemy attacks entirely",
     .maxLevel = 1,
     .currentLevel = 0,
     .baseCost = 1200,
     .gemCosts = {0, 0, 8, 0, 0},
     .requiredRank = 50,
     .applyOn = APPLY_ON_START,
     .applyEffect = NULL},
    {.name = "Blood Frenzy",
     .description = "Each kill grants +1 damage for 3 turns (stacks up to 3 times)",
     .maxLevel = 1,
     .currentLevel = 0,
     .baseCost = 1000,
     .gemCosts = {0, 10, 0, 0, 0},
     .requiredRank = 40,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Ritual Scarification",
     .description = "Gain +1 max HP, but start each run with -1 HP",
     .maxLevel = 5,
     .currentLevel = 0,
     .baseCost = 200,
     .gemCosts = {3, 0, 0, 0, 0},
     .requiredRank = 10,
     .applyOn = APPLY_ON_START,
     .applyEffect = NULL},
    {.name = "Chaos Infusion",
     .description = "10% chance to apply Bleeding to enemies (2 damage per turn for 2 turns)",
     .maxLevel = 3,
     .currentLevel = 0,
     .baseCost = 400,
     .gemCosts = {0, 4, 0, 0, 0},
     .requiredRank = 20,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Primal Resonance",
     .description = "Enemy cards suffer -1 damage after taking damage from you",
     .maxLevel = 3,
     .currentLevel = 0,
     .baseCost = 300,
     .gemCosts = {0, 0, 3, 0, 0},
     .requiredRank = 15,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Mutation Mastery",
     .description = "Healing potions heal +1 HP and grant +1 damage for 2 turns",
     .maxLevel = 2,
     .currentLevel = 0,
     .baseCost = 500,
     .gemCosts = {0, 0, 0, 5, 0},
     .requiredRank = 25,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Bloodscent",
     .description = "After killing an enemy, next card pickup gives +1 to its primary stat",
     .maxLevel = 2,
     .currentLevel = 0,
     .baseCost = 400,
     .gemCosts = {0, 0, 4, 0, 0},
     .requiredRank = 30,
     .applyOn = APPLY_ON_NONE,
     .applyEffect = NULL},
    {.name = "Blood Tithe",
     .description = "5% chance to gain double gems from kills",
     .maxLevel = 3,
     .currentLevel = 0,
     .baseCost = 300,
     .gemCosts = {5, 0, 0, 0, 0},
     .requiredRank = 20,
     .applyOn = APPLY_ON_START,
     .applyEffect = NULL}};
