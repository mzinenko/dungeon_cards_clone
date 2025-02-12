#include <constants.h>

typedef struct {
    int characterSelected;  // For future character selection
    int score;             // Can be based on gold collected or other metrics
    GameState currentState;
} GameContext;

typedef struct {
    int totalCoins;          // Total coins collected across all games
    int gems[5];             // Gems collected
    int maxHpUpgrade;        // How many times HP was upgraded
    int attackUpgrade;       // How many times attack was upgraded
    int defenseUpgrade;      // How many times defense was upgraded
} PlayerProgress;

typedef struct {
    const char *name;
    const char *description;
    int baseCost;
    int maxLevel;
    void (*applyUpgrade)();
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
    bool alignment; // true for vertical, false for horizontal
} Texture;

typedef struct {
    // Position
    int x;
    int y;
    int prevX;
    int prevY;

    // Alive status
    bool isAlive;

    // Attack-related properties
    struct {
        int meleeDamage;        // Damage for melee attacks
        int damage;             // Damage for enemies and weapons
        int ammo;               // Ammo for weapons
        int range;              // Range of attack
        float critChance;       // Critical hit chance (0.0 to 1.0)
        float multiShotChance;  // Multi-shot chance (0.0 to 1.0)
    } attack;

    // Defense-related properties
    struct {
        int hp;                 // Health points
        int maxHp;              // Maximum health points   
        int armor;              // Armor value
        ArmorType armorType;    // Type of armor
        bool isInvulnerable;    // Additional flag for special states
    } defense;

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
    Texture* texture; // Ensure this is managed properly
} Player;

typedef struct {
    CardType type;
    float weight;
} CardWeight;

// Card struct
typedef struct Card {
    char* id;           // Dynamically allocated string identifier
    CardType type;      // Type of the card
    CardRarity rarity;  // Rarity of the card

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

    void* customData;       // Pointer for additional, context-specific data
    Texture* texture;   // Texture for rendering
} Card;

// Animated card struct
typedef struct {
    float startX, startY;
    float currentX, currentY;
    float targetX, targetY;
    Card card;
    float animationProgress; // 0.0 to 1.0
} AnimatedCard;

typedef struct {
    AnimatedCard *cards;
    int cardCount;
    int isAnimating;
    float animationDuration;
    float currentTime;
    MoveDirection direction;
} CardAnimation;

typedef struct {
    int rows;
    int cols;
    Card **cells;  // 2D array of cards
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

typedef struct {
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

typedef enum {
    EVENT_GOLDMANIA,
    EVENT_PLAGUE,
} EventType;

typedef struct {
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
typedef void (*UpdateAnimationFunc)(struct Animation* animation, float deltaTime);
typedef void (*OnAnimationEndFunc)(struct Animation *animation);

// Animation structure
typedef struct Animation {
    UpdateAnimationFunc updateAnimation; // Custom update function
    OnAnimationEndFunc onAnimationEnd;   // Callback when animation ends
    bool isRunning;                      // Whether the animation is active
    void* userData;                      // Pointer to the animated object or additional data
} Animation;

// Global animation manager
typedef struct {
    Animation* animations[MAX_ANIMATIONS]; // Array of animation pointers
    int count;                             // Number of active animations
} AnimationManager;

extern AnimationManager *animationManager;

typedef struct {
    SDL_Rect rect;
    ButtonState state;
    Texture *texture;
    const char* identifier;
} MenuButton;

extern MenuButton *menuButtons;

// New structure for fraction data
// Add to existing struct Banner in types.h:
typedef struct {
    char* name;
    int relationship;    // 0-100
    FactionType type;   // Add this
    SDL_Texture* banner;
    SDL_Rect bannerRect;
    bool isHovered;     // Add this for hover effects
} Banner;

// New structure for hub interface
// Update the HubInterface struct in types.h
typedef struct {
    SDL_Rect playerPhotoRect;
    SDL_Rect statsRect;
    SDL_Rect godsRect;
    SDL_Rect resourceRect;
    SDL_Rect quitButton;    // New quit button
    SDL_Rect playButton;    // New play button
    bool quitHovered;       // Hover state for quit button
    bool playHovered;       // Hover state for play button
    Banner banners[4];  // 4 banners
    SDL_Texture* playerPhoto;
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
    SDL_Texture* icon;
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
    SaveFileButton* saveButtons;
    int saveButtonCount;
    SDL_Rect newGameButton;
    SDL_Rect importButton;
    SDL_Rect backButton;
    bool newGameHovered;
    bool importHovered;
    bool backHovered;
    const char* selectedFile;
} SaveSelectUI;

extern SaveSelectUI* saveSelectUI;

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
    FileEntry* selectedFile;
    bool isActive;
    bool upHovered;
    bool confirmHovered;
    bool cancelHovered;
    bool nextHovered;
    bool prevHovered;
} FileBrowser;

extern FileBrowser* fileBrowser;

typedef struct {
    const char* name;
    const char* description;
    int maxLevel;
    int currentLevel;
    int baseCost;           // Gold cost
    int gemCosts[5];        // Array of gem costs for each rarity, index matches rarity enum
    int requiredRank;
    void (*applyEffect)(void);
} FactionUpgrade;

typedef struct {
    const char* name;
    const char* description;
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
    int scrollOffset;          // Add this: tracks scroll position
    int maxScrollOffset;       // Add this: maximum scroll value
} FactionUI;

typedef struct {
    FactionType type;
    const char* name;
    int playerRank;
    Texture* leaderArt;
    FactionUpgrade upgrades[MAX_FACTION_UPGRADES];
    int upgradeCount;
    FactionQuest quests[MAX_FACTION_QUESTS];
    int questCount;
    FactionQuest* activeQuests[MAX_ACTIVE_QUESTS];
    int activeQuestCount;
    FactionUI ui;
    const char* currentDialog;
} Faction;

// Global variables to be externed:
extern Faction* vanguardFaction;
extern Faction* crimsonFaction;
extern Faction* currentFaction;

static const FactionUpgrade vanguardUpgrades[MAX_FACTION_UPGRADES] = {
    {
        .name = "Grid Lockdown Protocol",
        .description = "Enemy cards cannot spawn adjacent to you after movement",
        .maxLevel = 1,
        .currentLevel = 0,
        .baseCost = 2000,
        .gemCosts = {0, 0, 0, 0, 5},  // 5 Legendary
        .requiredRank = 80,
        .applyEffect = NULL
    },
    {
        .name = "Tactical Retreat System",
        .description = "Once per run, can move without triggering card interactions",
        .maxLevel = 1,
        .currentLevel = 0,
        .baseCost = 1500,
        .gemCosts = {0, 0, 0, 3, 0},  // 3 Epic
        .requiredRank = 70,
        .applyEffect = NULL
    },
    {
        .name = "Defense Matrix",
        .description = "When picking up a shield card, gain 1 extra armor",
        .maxLevel = 1,
        .currentLevel = 0,
        .baseCost = 1200,
        .gemCosts = {0, 0, 8, 0, 0},  // 8 Rare
        .requiredRank = 50,
        .applyEffect = NULL
    },
    {
        .name = "Combat Analysis Protocol",
        .description = "After defeating an enemy, gain 1 armor if your HP is below 50%",
        .maxLevel = 1,
        .currentLevel = 0,
        .baseCost = 1000,
        .gemCosts = {0, 10, 0, 0, 0},  // 10 Uncommon
        .requiredRank = 40,
        .applyEffect = NULL
    },
    {
        .name = "Combat Training",
        .description = "+1 melee damage",
        .maxLevel = 5,
        .currentLevel = 0,
        .baseCost = 200,
        .gemCosts = {3, 0, 0, 0, 0},  // 3 Common
        .requiredRank = 10,
        .applyEffect = NULL
    },
    {
        .name = "Ballistic Expertise",
        .description = "+1 ranged damage",
        .maxLevel = 3,
        .currentLevel = 0,
        .baseCost = 400,
        .gemCosts = {0, 4, 0, 0, 0},  // 4 Uncommon
        .requiredRank = 20,
        .applyEffect = NULL
    },
    {
        .name = "Tactical Gear",
        .description = "+1 maximum armor",
        .maxLevel = 3,
        .currentLevel = 0,
        .baseCost = 300,
        .gemCosts = {0, 0, 3, 0, 0},  // 3 Rare
        .requiredRank = 15,
        .applyEffect = NULL
    },
    {
        .name = "Field Medic",
        .description = "+2 maximum HP",
        .maxLevel = 2,
        .currentLevel = 0,
        .baseCost = 500,
        .gemCosts = {0, 0, 0, 5, 0},  // 5 Epic
        .requiredRank = 25,
        .applyEffect = NULL
    },
    {
        .name = "Extended Mag",
        .description = "+1 starting ammo",
        .maxLevel = 2,
        .currentLevel = 0,
        .baseCost = 400,
        .gemCosts = {0, 0, 4, 0, 0},  // 4 Rare
        .requiredRank = 30,
        .applyEffect = NULL
    },
    {
        .name = "Gem Resonance",
        .description = "+5% chance to get an additional gem of the same rarity from coin cards",
        .maxLevel = 3,
        .currentLevel = 0,
        .baseCost = 300,
        .gemCosts = {5, 0, 0, 0, 0},  // 5 Common
        .requiredRank = 20,
        .applyEffect = NULL
    }
};

static const FactionUpgrade crimsonUpgrades[MAX_FACTION_UPGRADES] = {
    {
        .name = "Blood Bond Protocol",
        .description = "Once per run, when HP drops to 1, transform into a chaos beast for 3 turns",
        .maxLevel = 1,
        .currentLevel = 0,
        .baseCost = 2000,
        .gemCosts = {0, 0, 0, 0, 5},  // 5 Legendary
        .requiredRank = 80,
        .applyEffect = NULL
    },
    {
        .name = "Pack Hunter's Instinct",
        .description = "First attack against each new enemy card has +2 damage",
        .maxLevel = 1,
        .currentLevel = 0,
        .baseCost = 1500,
        .gemCosts = {0, 0, 0, 3, 0},  // 3 Epic
        .requiredRank = 70,
        .applyEffect = NULL
    },
    {
        .name = "Predator's Reflexes",
        .description = "15% chance to dodge enemy attacks entirely",
        .maxLevel = 1,
        .currentLevel = 0,
        .baseCost = 1200,
        .gemCosts = {0, 0, 8, 0, 0},  // 8 Rare
        .requiredRank = 50,
        .applyEffect = NULL
    },
    {
        .name = "Blood Frenzy",
        .description = "Each kill grants +1 damage for 3 turns (stacks up to 3 times)",
        .maxLevel = 1,
        .currentLevel = 0,
        .baseCost = 1000,
        .gemCosts = {0, 10, 0, 0, 0},  // 10 Uncommon
        .requiredRank = 40,
        .applyEffect = NULL
    },
    {
        .name = "Ritual Scarification",
        .description = "Gain +1 max HP, but start each run with -1 HP",
        .maxLevel = 5,
        .currentLevel = 0,
        .baseCost = 200,
        .gemCosts = {3, 0, 0, 0, 0},  // 3 Common
        .requiredRank = 10,
        .applyEffect = NULL
    },
    {
        .name = "Chaos Infusion",
        .description = "10% chance to apply Bleeding to enemies (2 damage per turn for 2 turns)",
        .maxLevel = 3,
        .currentLevel = 0,
        .baseCost = 400,
        .gemCosts = {0, 4, 0, 0, 0},  // 4 Uncommon
        .requiredRank = 20,
        .applyEffect = NULL
    },
    {
        .name = "Primal Resonance",
        .description = "Enemy cards suffer -1 damage after taking damage from you",
        .maxLevel = 3,
        .currentLevel = 0,
        .baseCost = 300,
        .gemCosts = {0, 0, 3, 0, 0},  // 3 Rare
        .requiredRank = 15,
        .applyEffect = NULL
    },
    {
        .name = "Mutation Mastery",
        .description = "Healing potions heal +1 HP and grant +1 damage for 2 turns",
        .maxLevel = 2,
        .currentLevel = 0,
        .baseCost = 500,
        .gemCosts = {0, 0, 0, 5, 0},  // 5 Epic
        .requiredRank = 25,
        .applyEffect = NULL
    },
    {
        .name = "Bloodscent",
        .description = "After killing an enemy, next card pickup gives +1 to its primary stat",
        .maxLevel = 2,
        .currentLevel = 0,
        .baseCost = 400,
        .gemCosts = {0, 0, 4, 0, 0},  // 4 Rare
        .requiredRank = 30,
        .applyEffect = NULL
    },
    {
        .name = "Blood Tithe",
        .description = "5% chance to gain double gems from kills",
        .maxLevel = 3,
        .currentLevel = 0,
        .baseCost = 300,
        .gemCosts = {5, 0, 0, 0, 0},  // 5 Common
        .requiredRank = 20,
        .applyEffect = NULL
    }
};
