#ifndef TYPES_H
#define TYPES_H

#include <constants.h>

typedef struct {
    int characterSelected;
    int score;         
    GameState currentState;
} GameContext;

typedef struct {
    int upgradeId;
    int currentLevel;
} SavedUpgrade;

typedef struct {
    int totalCoins;
    int gems[5];

    struct {
        SavedUpgrade upgrades[MAX_FACTION_UPGRADES];
        int count;
    } vanguardUpgrades;

    struct {
        SavedUpgrade upgrades[MAX_FACTION_UPGRADES];
        int count;
    } crimsonUpgrades;
} PlayerProgress;

typedef struct {
    const char *name;
    const char *description;
    int baseCost;
    int maxLevel;
    void (*applyUpgrade)(void);
} StoreItem;

typedef struct {
    StoreItem *items;
    int itemCount;
    int selectedItem;
    SDL_Rect buyButton;
    SDL_Rect backButton;
} Store;

typedef struct {
    char *texturePath;
    SDL_Texture *texture;
    SDL_Rect clipRect;
    int frameCount;
    int currentFrame;
    int frameGap;
    bool alignment;
} Texture;

typedef struct {
    int x;
    int y;
    int prevX;
    int prevY;


    bool isAlive;

    struct {
        int meleeDamage;
        int damage;
        int ammo;
        int range;
        float critChance;
        float multiShotChance;
    } attack;

    // Defense-related properties
    struct {
        int hp;   
        int maxHp; 
        int armor; 
        ArmorType armorType;
        bool isInvulnerable;
    } defense;

    struct {
        int damage;
        int armor;
    } base;

    // Inventory/Resources
    int gold;
    int gems[5];

    // Animation data
    struct {
        float currentX;
        float currentY;
        float animationProgress;
        float animationDuration;
        bool isAnimating;
        MoveDirection currentMove;
    } animation;

    // Rendering-related
    Texture *texture; 
} Player;

typedef struct {
    CardType type;
    float weight;
} CardWeight;

// Card struct
typedef struct Card {
    char *id;       
    CardType type;   
    CardRarity rarity;

    struct {
        int damage;
        int range;
        int ammo;
        float critChance;
        float multiShotChance;
    } attack;

    struct {
        int hp;
        int maxHp;
        int armor;
        int maxArmor;
        ArmorType armorType;
        bool isInvulnerable;
    } defense;

    struct {
        int x, y;
    } animation;

    struct {
        int value;
        float multiplier;
    } value;

    void *customData;
    Texture *texture;

    Uint32 lastFrameTime;
} Card;

// Animated card struct
typedef struct
{
    float startX, startY;
    float currentX, currentY;
    float targetX, targetY;
    Card card;
    float animationProgress; 
} AnimatedCard;

typedef struct
{
    AnimatedCard *cards;
    int cardCount;
    int isAnimating;
    float animationDuration;
    float currentTime;
    MoveDirection direction;
} CardAnimation;

typedef struct
{
    int rows;
    int cols;
    Card **cells; 
} Grid;

extern Texture *eventObserverTextures;
extern Texture *enemyTextures;
extern Texture *coinTextures;
extern Texture *heroTextures;
extern Texture *potionTextures;
extern Texture *uiTextures;

extern size_t eventObserverTexturesCount;
extern size_t enemyTexturesCount;
extern size_t coinTexturesCount;
extern size_t heroTexturesCount;
extern size_t potionTexturesCount;
extern size_t uiTexturesCount;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *font;
extern SDL_Event event;

extern Player *player;
extern Grid *grid;
extern GameContext *gameContext;
extern PlayerProgress *progress;

extern Store *store;
extern CardAnimation *cardAnim;

extern bool isShooting;
extern bool isPlayerTurn;
extern AnimatedCard *enemiesToShoot;

typedef struct
{
    float targetX, targetY;
    float x, y;
    float vx, vy;
    bool active;
    float elapsedTime;
    float duration;

    Texture *texture;
} Projectile;

extern Projectile *projectiles;
extern int projectileCount;

extern int turn;

typedef enum
{
    EVENT_GOLDMANIA,
    EVENT_PLAGUE,
} EventType;

typedef struct
{
    float x, y;
    float vx, vy;
    float targetX, targetY;
    bool active;
    bool isAnimating;

    Texture *texture;
} EventObserver;

extern EventObserver *eventObserver;

// Forward declaration for Animation structure
struct Animation;

// Define the update and callback function types
typedef void (*UpdateAnimationFunc)(struct Animation *animation, float deltaTime);
typedef void (*OnAnimationEndFunc)(struct Animation *animation);

// Animation structure
typedef struct Animation {
    UpdateAnimationFunc updateAnimation;
    OnAnimationEndFunc onAnimationEnd; 
    bool isRunning;
    void *userData;
} Animation;

// Global animation manager
typedef struct {
    Animation *animations[MAX_ANIMATIONS]; 
    int count; 
} AnimationManager;

extern AnimationManager *animationManager;

typedef struct {
    SDL_Rect rect;
    ButtonState state;
    Texture *texture;
    const char *identifier;
} MenuButton;

extern MenuButton *menuButtons;

// New structure for fraction data
// Add to existing struct Banner in types.h:
typedef struct
{
    char *name;
    int relationship;
    FactionType type;
    SDL_Texture *banner;
    SDL_Rect bannerRect;
    bool isHovered; 
} Banner;

// New structure for hub interface
// Update the HubInterface struct in types.h
typedef struct {
    SDL_Rect playerPhotoRect;
    SDL_Rect statsRect;
    SDL_Rect godsRect;
    SDL_Rect resourceRect;
    SDL_Rect quitButton;
    SDL_Rect playButton;
    bool quitHovered; 
    bool playHovered; 
    Banner banners[4];
    SDL_Texture *playerPhoto;
} HubInterface;

// Resource types for the resource display
typedef enum {
    RESOURCE_GOLD,
    RESOURCE_GEM_COMMON,
    RESOURCE_GEM_RARE,
    RESOURCE_GEM_EPIC,
    RESOURCE_GEM_LEGENDARY
} ResourceType;

// Resource structure
typedef struct {
    ResourceType type;
    int amount;
    SDL_Texture *icon;
} Resource;

extern HubInterface *hubInterface;

typedef struct {
    uint32_t magicNumber;
    uint32_t version;
    time_t timestamp;
    PlayerProgress data;
    uint32_t checksum;
} SaveFileHeader;

typedef struct {
    char filename[256];
    time_t lastModified;
    bool isValid;
    PlayerProgress preview;
} SaveFileInfo;

extern SaveFileInfo saveFiles[MAX_SAVE_FILES];
extern int numSaveFiles;

typedef struct {
    SDL_Rect rect;
    SaveFileInfo info;
    bool isHovered;
} SaveFileButton;

typedef struct {
    SaveFileButton *saveButtons;
    int saveButtonCount;
    SDL_Rect newGameButton;
    SDL_Rect importButton;
    SDL_Rect backButton;
    bool newGameHovered;
    bool importHovered;
    bool backHovered;
    const char *selectedFile;
} SaveSelectUI;

extern SaveSelectUI *saveSelectUI;

typedef struct {
    char name[256];
    char fullPath[MAX_PATH_LENGTH];
    bool isDirectory;
} FileEntry;

typedef struct {
    FileEntry entries[MAX_FILE_ENTRIES];
    int entryCount;
    int currentPage;
    int totalPages;
    char currentPath[MAX_PATH_LENGTH];
    SDL_Rect browserRect;
    SDL_Rect upButtonRect;
    SDL_Rect confirmButtonRect;
    SDL_Rect cancelButtonRect;
    SDL_Rect nextPageRect;
    SDL_Rect prevPageRect;
    SDL_Rect pathRect;
    FileEntry *selectedFile;
    bool isActive;
    bool upHovered;
    bool confirmHovered;
    bool cancelHovered;
    bool nextHovered;
    bool prevHovered;
} FileBrowser;

extern FileBrowser *fileBrowser;

typedef struct {
    const char *name;
    const char *description;
    int maxLevel;
    int currentLevel;
    int baseCost;   
    int gemCosts[5]; 
    int requiredRank;
    ApplyOn applyOn;
    void (*applyEffect)(int currentLevel);
} FactionUpgrade;

typedef struct {
    const char *name;
    const char *description;
    bool isRepeatable;
    int cooldown;
    int requiredRank;
    bool isActive;
    bool isCompleted;
    int runsUntilCooldown;
    void (*checkCondition)(void);
    void (*grantReward)(void);
} FactionQuest;

typedef struct {
    SDL_Rect leaderArtRect;
    SDL_Rect dialogRect;
    SDL_Rect contentRect;
    SDL_Rect returnButton;
    bool isStoreTab;
    bool returnHovered;
    DialogState dialogState;
    int scrollOffset;  
    int maxScrollOffset;
} FactionUI;

typedef struct {
    FactionType type;
    const char *name;
    int playerRank;
    Texture *leaderArt;
    FactionUpgrade upgrades[MAX_FACTION_UPGRADES];
    int upgradeCount;
    FactionQuest quests[MAX_FACTION_QUESTS];
    int questCount;
    FactionQuest *activeQuests[MAX_ACTIVE_QUESTS];
    int activeQuestCount;
    FactionUI ui;
    const char *currentDialog;
} Faction;


extern Faction *vanguardFaction;
extern Faction *crimsonFaction;
extern Faction *currentFaction;

extern FactionUpgrade vanguardUpgrades[MAX_FACTION_UPGRADES];

extern FactionUpgrade crimsonUpgrades[MAX_FACTION_UPGRADES];

extern bool isDev;

typedef struct {
    SDL_Texture* renderTarget;
    float scaleX;
    float scaleY;
    SDL_Rect outputRect;
} RenderContext;

extern RenderContext *renderCtx;

#endif

