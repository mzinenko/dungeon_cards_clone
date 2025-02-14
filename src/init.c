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
