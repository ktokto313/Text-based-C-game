#include <stdio.h>
#include <stdlib.h>
#include "game_object.h"
#include "combat.h"
#include "shop.h"
#include "sideEvent.h"
// #include "Shop.c"

int getWeightedScenario(int weights[], int size) {
    int total = 0;
    for (int i = 0; i < size; i++) {
        total += weights[i];
    }
    int r = rand() % total;
    int cumulative = 0;
    for (int i = 0; i < size; i++) {
        cumulative += weights[i];
        if (r < cumulative) return i;
    }
    return size - 1;
}

int explore(Game* game) {
    printf("\n=== EXPLORATION ===\n");
    printf("Exploring the area...\n");
    for (int i = 1; i <= game->config.explorationTurn; i++) {
        printf("\n--- Exploration Step %d/%d ---\n", i, game->config.explorationTurn);
        int scenario = getWeightedScenario(game->config.scenarioWeights, 3);
        switch (scenario) {
            case 0:
                printf("*** COMBAT ENCOUNTER ***\n");
                createCombat(game);
                break;
            case 1:
                printf("*** MERCHANT ENCOUNTER ***\n");
                openShop(game);
                break;
            case 2:
                printf("*** SIDE EVENT ***\n");
                handleSideEvent(game);
                break;
        }
        if (i < game->config.explorationTurn) {
            printf("\nPress any key to continue...");
            getchar();
        }
    }
    printf("\nExploration complete!\n");
    return 0;
}