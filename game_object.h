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
    //first element is combat chance, second element is trade chance, third is minigameChance
    int scenarioWeights[3];
    int maxTimeOfTheDay;
} Config;

typedef struct {
    int health;
    int maxHealth;
    int damage;

} Champion;