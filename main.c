#include <stdio.h>
#include "fileio.h"
#include "game.h"
#include "explore.h"
#include "combat.h"
#include "game_object.h"
#include "character.h"
#include "movement.h"
#include "cJSON.h"

// Khai b�o c�c h�m
void showMainMenu(Game *game);
void adminMenu();
void testerMenu();

int main() {
    int modeChoice;
    Game game;
    game.initialized = 0;

    while (1) {
        printf("\n====================================\n");
        printf("     WELCOME TO OSG ADVENTURE GAME  \n");
        printf("====================================\n");
        printf("Select mode:\n");
        printf("1. Admin mode\n");
        printf("2. Tester mode\n");
        printf("3. Main game\n");
        printf("0. Exit\n");
        printf("------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &modeChoice);

	    switch(modeChoice) {
	    	case 1:
	            adminMenu();
	            continue;
	        case 2:
	            game.isTester = 1;
	            break;
	        case 0:
	            printf("Goodbye! Thanks for playing!\n");
	            return 0; 
	        default:
	            printf("Invalid choice! Please choose 0�3.\n");
	            continue;
		}
            
        showMainMenu(&game);
    }
    return 0;
}

void showMainMenu(Game *game) {
    int choice;

    if (game->initialized) {
        while (1) {
            printf("\n------------- MAIN MENU ------------\n");
            printf("1. Continue\n");
            printf("2. Save game\n");
            printf("3. View stats\n");
            printf("4. New game\n");
            printf("5. Load game\n");
            printf("6. Exit to Mode Selection\n");
            printf("7. Admin Mode\n");
            printf("------------------------------------\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            if (choice == 1) {
                printf("Continuing game...\n");
            } 
            else if (choice == 2) {
                printf("Saving game...\n");
            } 
            else if (choice == 3) {
                printf("Showing player stats...\n");
            } 
            else if (choice == 4) {
                printf("Starting new game...\n");
                game->initialized = 1;
            } 
            else if (choice == 5) {
                printf("Loading saved game...\n");
                game->initialized = 1;
            } 
            else if (choice == 6) {
                printf("Returning to Mode Selection...\n");
                return;
            } 
            else if (choice == 7) {
                adminMenu();
            } 
            else {
                printf("Please input a number in range 1-7.\n");
            }
        }
    } 

    else {
        while (1) {
            printf("\n========== MAIN GAME MENU ==========\n");
            printf("1. New game\n");
            printf("2. Load game\n");
            printf("3. Back to mode selection\n");
            printf("------------------------------------\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            if (choice == 1) {
                printf("Starting new game...\n");
                game->initialized = 1;
            } 
            else if (choice == 2) {
                printf("Loading saved game...\n");
                game->initialized = 1;
            } 
            else if (choice == 3) {
                printf("Returning to mode selection...\n");
                return;
            } 
            else {
                printf("Invalid choice! Please choose 1�3.\n");
            }
        }
    }
}


void adminMenu() {
    int choice;
    while (1) {
        printf("\n========== ADMIN MENU ==========\n");
        printf("1. View all saved games\n");
        printf("2. Delete save file\n");
        printf("3. Back to mode selection\n");
        printf("--------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("[Save files list]\n- save1.dat\n- save2.dat\n");
        } 
        else if (choice == 2) {
            char filename[50];
            printf("Enter file name to delete: ");
            scanf("%s", filename);
            if (remove(filename) == 0)
                printf("File '%s' deleted successfully!\n", filename);
            else
                printf("Failed to delete '%s'.\n", filename);
        } 
        else if (choice == 3) {
            printf("Returning to mode selection...\n");
            return;
        } 
        else {
            printf("Invalid choice! Please choose 1-3.\n");
        }
    }
}
