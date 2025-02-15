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
void initAnimationManager(void);

void addAnimation(Animation *animation);
void updateAnimations(float deltaTime);

bool isAnyAnimationRunning(void);

// card.c
void initCard(Card *card);

void drawCardContent(Card *card, SDL_Rect cardRect);

Card *createEmptyCard(void);
Card *createCard(CardType type, CardRarity rarity);
Card *generateCard(void);

void getCardColor(const Card *card, SDL_Color *color);
char *cardTypeToString(CardType type);
bool isTriggered(const Card *card);

void freeCard(Card *card);

// event_observer.c

void renderEventObserver(void);

void updateEventObserver(Animation *animation, float deltaTime);

void eventTurn(void);
void triggerEvent(Animation *animation);
void eventTurnResults(Animation *animation);

void freeObserver(Animation *animation);

// enemy.c

void enemyTurn(void);
void enemyInteraction(int enemyX, int enemyY);

void enemyTurnResults(Animation *animation);
void enemyInteractionResults(Animation *animation);

// gamestate.c

void initGameContext(void);
void initMenu(void);
void initHubInterface(void);
void initSaveSelectUI(void);

void drawMainMenu(void);
void drawHubInterface(void);
void drawSaveSelectUI(void);
void drawQuitButton(void);
void drawGameOver(void);

void handleMenuInput(void);
void handleSaveSelectInput(void);
void handleHubInput(void);
void handleQuitInput(void);
void handleGameOverInput(void);

void cleanupSaveSelectUI(void);

// generator.c

CardWeight *calculateCardWeights(int *weightCount);
CardType selectCardTypeByWeight(CardWeight *weights, int weightCount);
CardRarity selectRarityByWeight(void);

float calculateEnemyWeight(void);
float calculateWeaponWeight(void);
float calculateShieldWeight(void);
float calculatePotionWeight(void);
float calculateCoinWeight(void);

// grid.c

void initGrid(int rows, int cols);

void drawGridBackground(void);
void drawGrid(void);
void drawGridWithAnimation(void);

void populateGrid(void);
void shiftCards(int startRow, int startCol, MoveDirection direction);
void interaction(void);

void destroyGrid(void);

// init.c

// input.c

// player.c

void createPlayer(Texture *texture);

void drawPlayer(void);
void drawPlayerStats(void);

void updatePlayerAnimation(float deltaTime);

void handlePlayerInput(int gridRows, int gridCols);

void movePlayer(int dx, int dy, int gridRows, int gridCols, MoveDirection *outDirection);

bool canAttackEnemy(int enemyX, int enemyY);
void resetPlayerPosition(void);

void destroyPlayer(void);

// projectile.c ✅

void renderProjectiles(void);

void updateProjectiles(Animation *animation, float deltaTime);

void shootProjectile(float startX, float startY, float targetX, float targetY, float speed);

// render.c ✅

void initRenderContext(void);
void updateRenderScale(void);
void beginRender(void);
void endRender(void);
void windowToVirtual(int windowX, int windowY, int* virtualX, int* virtualY);
void virtualToWindow(int virtualX, int virtualY, int* windowX, int* windowY);
void destroyRenderContext(void);

void drawButton(const char *text, SDL_Rect rect, bool isHovered);
void renderText(const char *text, int x, int y, SDL_Color color);

bool isMouseOverButton(int mouseX, int mouseY, SDL_Rect button);

// save.c ✅

void scanSaveFiles(void);
bool createNewSave(const char *filename);
bool loadSave(const char *filename);
void syncLoadedUpgrades(void);
bool saveProgress(const char *filename);
const SaveFileInfo *getSaveFiles(int *count);
bool importSaveFile(const char *sourcePath);
bool deleteSave(const char *filename);

void initFileBrowser(void);
void scanDirectory(void);
void drawFileBrowser(void);
bool handleFileBrowserInput(void);
void cleanupFileBrowser(void);

// texture.c ✅

void initTextures(void);

void loadTextures(void);
SDL_Texture *loadTexture(const char *filePath);

void destroyAllTextures(void);

// utils.c ✅

bool isInRange(int x1, int y1, int x2, int y2, int range);
float frandomInRange(float min, float max);
int randomInRange(int min, int max);
char *mx_strdup(const char *str);

//------------------------------------------------

void startCardAnimation(MoveDirection direction);
void updateCardAnimation(float deltaTime);

void initCardAnimation(void);

// Faction core functions
void initFactions(void);
void initFactionUI(Faction *faction);
void switchToFaction(FactionType type);
void updateFactionQuests(void);
void checkQuestCompletion(Faction *faction);
void applyFactionUpgrade(Faction *faction, int upgradeIndex);

// Faction UI functions
void drawFactionUI(void);
void handleFactionInput(void);
void drawFactionDialog(void);
void drawFactionStore(void);
void drawFactionQuests(void);
void updateFactionDialog(void);

void handleUpgradePurchase(Faction* faction, int upgradeIndex);

// Upgrade functions
void initFactionUpgrades(Faction *faction);
bool purchaseUpgrade(Faction *faction, int upgradeIndex);

// Quest functions
void acceptQuest(Faction *faction, int questIndex);
void completeQuest(Faction *faction, FactionQuest *quest);
void updateQuestCooldowns(void);

// Mutation functions
void initMutations(void);
void applyMutation(MutationType type);
void updateMutationEffects(void);


