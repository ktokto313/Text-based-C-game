// game.h
#ifndef GAME_OBJECT
#define GAME_OBJECT 1

typedef struct node_t {
    void *value;
    struct node_t *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
} LinkedList;

#define FOREACH_CHAMPION(CHAMPION) \
        CHAMPION(WIZARD)   \
        CHAMPION(KNIGHT)   \
        CHAMPION(PALADIN)   \
        CHAMPION(ROGUE)   \
        CHAMPION(ELF)   \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum Class {
    FOREACH_CHAMPION(GENERATE_ENUM)
};

static const char *champion_string[] = {
    FOREACH_CHAMPION(GENERATE_STRING)
};

typedef struct {
    int health;
    int maxHealth;
    int level;
    int xp;
    int damage;
    enum Class class;
} Champion;

typedef struct {
    int health;
    int maxHealth;
    int damage;
    char name[30];
} Monster;

enum ItemType {
    WEAPON,
    HEALTH_POTION,
    SPECIAL
};

typedef struct {
    char name[50];
    LinkedList monsterList;
} LocationData;

typedef struct {
    enum ItemType type;
    char name[50];
    int value;
    int effectValue;
} Item;

typedef struct {
    LinkedList itemList;
} Shop;

typedef struct {
    int allowSaveInCombat;
    int difficulty;
    int baseLvUpXp;
    int lvUpXpMul;
    int chestGoldMin;
    int chestGoldMax;
    int chestItemChance;
    int lockpickChance;
    int trapDisarmChance;
    int trapDisarmExp;
    int trapDamage;

    int explorationTurn;
    //first element is combat chance, second element is trade chance, third is minigameChance
    int scenarioWeights[3];
    int maxTimeOfTheDay;
    int teleportPenalty;

    double sellValue;
} Config;

typedef struct {
    int level;
    int day;
    int timeOfTheDay;
    int gold;
    LinkedList itemList;    
    Champion champion[3];
    // Implement map using adjacentMatrix;
    int *map;
    LocationData *locationData;
    int mapSize;
    int initialized;
    Config config;
    Shop shop;
    int isTester;
} Game;

extern void init(LinkedList list);
extern void insert(LinkedList list, void *pt);
extern Node* getElementAt(LinkedList list, int index);
extern Node* removeAt(LinkedList list, int index);
extern void freeList(LinkedList list);
// Method to find the index'th true value in a binary map
// Return -1 if an error was found
extern int findBinaryMapping(int binaryMap[], int index, int length);
extern void addTimeOfTheDay(Game * game, int value);

#endif