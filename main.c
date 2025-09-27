#include "fileio.c"
#include "game.c"
#include "explore.c"
#include "combat.c"
#include "game_object.h"

void showMainMenu(Game * game);

int main() {
    Game game;
    showMainMenu(&game);
}

void showMainMenu(Game * game) {
    printf("Welcome to our game. This is the main menu.");
    int choice;
    while (1) {
        // Game is already running, add new option
        if (game->initialized) {
            printf("1. Continue");
            printf("2. Save game");
            printf("3. View stats");
            printf("4. New game");
            printf("5. Load game");
            printf("6. Exit");
            choice = scanf("%d", &choice);

            switch (choice)
            {
                case 1:
                    doGameTick(game);
                    break;
                case 2:
                    //This is manual save so autosave = 0
                    saveGame(game, 0);
                    break;
                case 3:
                    viewStats(game);
                    break;
                case 4:
                    initGame(game);
                    break;
                case 5:
                    loadGame(game);
                    break;
                case 6:
                    return;
            }
        } else {
            printf("1. New game");
            printf("2. Load game");
            printf("3. Exit");
            choice = scanf("%d", &choice);

            switch (choice)
            {
                case 1:
                    initGame(game);
                    doGameTick(game);
                    break;
                case 2:
                    loadGame(game);
                    doGameTick(game);
                    break;
                case 3:
                    return;
            }
        }

    }
}