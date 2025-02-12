#include "types.h"

/*
file structure:
- [name].c
  - init functions
  - render functions
  - update functions
  - input functions
  - util functions
  - free/destroy functions
*/

// animation.c
void initAnimationManager();

void addAnimation(Animation *animation);
void updateAnimations(float deltaTime);

bool isAnyAnimationRunning();

// card.c
void initCard(Card* card);

void drawCardContent(Card *card, SDL_Rect cardRect);

Card* createEmptyCard();
Card* createCard(CardType type, CardRarity rarity);
Card* generateCard();

void getCardColor(const Card* card, SDL_Color* color);
char *cardTypeToString(CardType type);
bool isTriggered(const Card* card);

void freeCard(Card* card);

// event_observer.c

void renderEventObserver();

void updateEventObserver(Animation *animation, float deltaTime);

void eventTurn();
void triggerEvent(Animation *animation);
void eventTurnResults(Animation *animation);

void freeObserver(Animation *animation);

// enemy.c 

void enemyTurn();
void enemyInteraction(int enemyX, int enemyY);

void enemyTurnResults(Animation *animation);
void enemyInteractionResults(Animation *animation);

// gamestate.c

void initGameContext();
void initMenu();
void initHubInterface();
void initSaveSelectUI();

void drawMainMenu();
void drawHubInterface();
void drawSaveSelectUI();
void drawQuitButton();
void drawStore();
void drawGameOver();

void handleMenuInput();
void handleSaveSelectInput();
void handleHubInput();
void handleQuitInput();
void handleStoreInput();
void handleGameOverInput();

void cleanupSaveSelectUI();

// generator.c

CardWeight* calculateCardWeights(int* weightCount);
CardType selectCardTypeByWeight(CardWeight* weights, int weightCount);
CardRarity selectRarityByWeight();

float calculateEnemyWeight();
float calculateWeaponWeight();
float calculateShieldWeight();
float calculatePotionWeight();
float calculateCoinWeight();

// grid.c

void initGrid(int rows, int cols);

void drawGridBackground();
void drawGrid();
void drawGridWithAnimation();

void populateGrid();
void shiftCards(int startRow, int startCol, MoveDirection direction);
void interaction();

void destroyGrid();

// init.c

// input.c

// player.c

void createPlayer(Texture *texture);

void drawPlayer();
void drawPlayerStats();

void updatePlayerAnimation(float deltaTime);

void handlePlayerInput(int gridRows, int gridCols);

void movePlayer(int dx, int dy, int gridRows, int gridCols, MoveDirection *outDirection);

bool canAttackEnemy(int enemyX, int enemyY);
void resetPlayerPosition();

void destroyPlayer();

// projectile.c ✅

void renderProjectiles();

void updateProjectiles(Animation *animation, float deltaTime);

void shootProjectile(float startX, float startY, float targetX, float targetY, float speed);

// render.c ✅

void drawButton(const char *text, SDL_Rect rect, SDL_bool isHovered);
void renderText(const char *text, int x, int y, SDL_Color color);

SDL_bool isMouseOverButton(int mouseX, int mouseY, SDL_Rect button);

// save.c ✅

void scanSaveFiles();
bool createNewSave(const char* filename);
bool loadSave(const char* filename);
bool saveProgress(const char* filename);
const SaveFileInfo* getSaveFiles(int* count);
bool importSaveFile(const char* sourcePath);
bool deleteSave(const char* filename);

void initFileBrowser();
void scanDirectory();
void drawFileBrowser();
bool handleFileBrowserInput();
void cleanupFileBrowser();

// store.c ✅

void initStore();

void upgradeMaxHP();
void upgradeAttack();
void upgradeDefense();

int getUpgradeCost(const StoreItem *item);

void destroyStore();

// texture.c ✅

void initTextures();

void loadTextures();
SDL_Texture *loadTexture(const char *filePath);

void destroyAllTextures();

// utils.c ✅

bool isInRange(int x1, int y1, int x2, int y2, int range);
float frandomInRange(float min, float max);
int randomInRange(int min, int max);
char* mx_strdup(const char* str);

//------------------------------------------------

void startCardAnimation(MoveDirection direction);
void updateCardAnimation(float deltaTime);

void initCardAnimation();

// Faction core functions
void initFactions();
void switchToFaction(FactionType type);
void updateFactionQuests();
void checkQuestCompletion(Faction* faction);
void applyFactionUpgrade(Faction* faction, int upgradeIndex);

// Faction UI functions
void drawFactionUI();
void handleFactionInput();
void drawFactionDialog();
void drawFactionStore();
void drawFactionQuests();
void updateFactionDialog();

// Upgrade functions
void initFactionUpgrades(Faction* faction);
bool purchaseUpgrade(Faction* faction, int upgradeIndex);

// Quest functions
void acceptQuest(Faction* faction, int questIndex);
void completeQuest(Faction* faction, FactionQuest* quest);
void updateQuestCooldowns();

// Mutation functions
void initMutations();
void applyMutation(MutationType type);
void updateMutationEffects();
