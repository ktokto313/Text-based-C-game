// game.h
typedef struct {
    int level;
    int day;
    int timeOfTheDay;
    Champion champion[3];
    int *map;
    int initialized;
    Config config;
} Game;

typedef struct {
    int allowSaveInCombat;
    int difficulty;

    int explorationTurn;
    int combatChance;
    int tradeChance;
    int minigameChance;

    int maxTimeOfTheDay;
} Config;

typedef struct {
    int health;
    int maxHealth;
    int damage;

} Champion;