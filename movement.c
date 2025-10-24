
#include <stdlib.h>
#include <stdio.h>
#include "game_object.h"
#include "movement.h"

// Show movement interface with locations that 
// the player can move to
void move(Game * game) {
    while (1) {
        printf("Select locations you want to move to!\n");
        int *pt = game->map + game->mapSize*game->level;
        int counter = 0;
        for (int i = 0;i < game->mapSize;i++) {
            // Node is connected to each other
            if (*(pt + i) == 1) {
                printf("%d. %s\n", ++counter, game->locationData[i].name);
            }
        }
        printf("%d. Quit\n", ++counter);
        printf("Select the location you want to move to: \n");
        int choice;
        scanf("%d", &choice);
        if (choice == counter) return;
        choice = findBinaryMapping(pt, choice, game->mapSize);
        if (choice == -1) {
            printf("Invalid choice, please select number from 1-%d!\n", counter);
            continue;
        }
        
        printf("You have moved to %s\n", game->locationData[choice].name);
        addTimeOfTheDay(game, 1);
        game->level = choice;
        return;
    }
}

void teleport(Game * game, int dest) {
    LinkedList queue;
    int visited[game->mapSize];
    int distance[game->mapSize];
    for (int i = 0;i < game->mapSize;i++) {
        visited[i] = 0;
        distance[i] = game->mapSize+1;
    }
    init(&queue);
    int *current = (int *) malloc(sizeof(int));
    *current = game->level;
    insert(&queue, current);
    distance[*current] = 0;
    for (int j = 0;j < game->mapSize;j++) {
        Node *node = removeAt(&queue, 0);
        int currentLevel = *(int *) node->value;
        visited[currentLevel] = 1;
        int *pt = game->map + game->mapSize*currentLevel;
        free(node->value);
        free(node);

        for (int i = 0;i < game->mapSize;i++) {
            // Node is connected to each other
            if (*(pt + i) == 1) {
                if (i == dest) {
                    game->level = dest;
                    distance[dest] = distance[currentLevel] + 1;
                    printf("Teleported to location which is: %d steps away.\n", distance[dest]);
                    if (game->isTester) {
                        addTimeOfTheDay(game, distance[dest]);
                    } else {
                        addTimeOfTheDay(game, game->config.teleportPenalty + distance[dest]);
                    }
                    freeList(&queue);
                    return;
                } else {
                    if (visited[i]) continue;
                    int *temp = (int *) malloc(sizeof(int));
                    *temp = i;
                    if (distance[i] > distance[currentLevel] + 1) {
                        distance[i] = distance[currentLevel] + 1;
                        insert(&queue, temp);
                    }
                }
            }
        }
    }
}