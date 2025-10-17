
#include <stdlib.h>
#include <stdio.h>
#include "game_object.h"
#include "movement.h"

// Show movement interface with locations that 
// the player can move to
void move(Game * game) {
    while (1) {
        printf("Select locations you want to move to!");
        int *pt = game->map + game->mapSize*game->level;
        int counter = 0;
        for (int i = 0;i < game->mapSize;i++) {
            // Node is connected to each other
            if (*(pt + i) == 1) {
                printf("%d. %s\n", ++counter, ((LocationData *) game->locationData)->name);
            }
        }
        printf("%d. Quit\n", ++counter);
        printf("Select the location you want to move to: \n");
        int choice;
        scanf("%d", &choice);
        if (choice == counter) return;
        choice = findBinaryMapping(pt, choice-1, game->mapSize);
        if (choice == -1) {
            printf("Invalid choice, please select number from 0-%d!\n", choice);
            continue;
        }
        
        printf("You have moved to %s", ((LocationData *) game->locationData + choice)->name);
        game->level = choice;
    }
}

void teleport(Game * game, int dest) {
    LinkedList queue;
    init(queue);
    int *current = (int *) malloc(sizeof(int));
    *current = game->level;
    insert(queue, current);
    int stepCounter = 0;
    for (int j = 0;j < game->mapSize;j++) {
        Node *node = removeAt(queue, 0);
        current = (int *) node->value;
        int *pt = game->map + game->mapSize*(*current);
        free(node);
        free(current);

        for (int i = 0;i < game->mapSize;i++) {
            // Node is connected to each other
            if (*(pt + i) == 1) {
                if (i == dest) {
                    game->level = dest;
                    addTimeOfTheDay(game, game->config.teleportPenalty + stepCounter);
                    return;
                } else {
                    int *temp = (int *) malloc(sizeof(int));
                    *temp = i;
                    insert(queue, temp);
                }
            }
        }
    }
}