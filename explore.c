#include <stdio.h>
#include "game_data.h"
void explorer(Game *game, Config *config) {
    int choice;
    printf("=== Exploration Mode ===\n");
    while (1) {
        printf("\nWhat would you like to do?\n");
        printf("1. Move\n");
        printf("2. Shop\n");
        printf("3. Exit exploration\n");
        printf("Choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("You move to a new location...\n");
                break;
            case 2:
                printf("You enter the shop...\n");
                break;
            case 3:
                printf("Exiting exploration...\n");
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}
