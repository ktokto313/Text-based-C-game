#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_object.h"
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
int explorer(Game* game) {
    printf("\n=== EXPLORATION ===\n");
    printf("Exploring the area...\n");
    int steps = game->config.explorationTurn;
    int weights[3];
    for (int i = 0; i < 3; i++) {
        weights[i] = game->config.scenarioWeights[i];
    }
    for (int i = 1; i <= steps; i++) {
        printf("\n--- Exploration Step %d/%d ---\n", i, steps);
        int scenario = getWeightedScenario(weights, 3);
        switch (scenario) {
            case 0:
                printf("*** COMBAT ENCOUNTER ***\n");
                createcombat(game);
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
        if (i < steps) {
            printf("\nPress Enter to continue...");
            while (getchar() != '\n');
        }
    }
    printf("\nExploration complete!\n");
    return 0;
}