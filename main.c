#include "fileio.c"
#include "game.c"
#include "explore.c"
#include "combat.c"
#include "game_object.h"

void showMainMenu(Game * game);

int main() {
    Game game;
    game.initialized = 0;
    showMainMenu(&game);
    return 0;
}

void showMainMenu(Game * game) {
   int choice;
   printf ("=================================\n");
   printf("   WELCOME TO OUR ADVENTURE GAME   \n");
   printf("==================================\n");
   
   while(1){
   	if(game -> initializedd){
   		printf("\n-------------MAIN MENU ------------\n");
   		printf("1.Continue\n");
   		printf("2. Save game\n");
   		printf("3. View stats\n");
   		printf("4. New game\n");
   		printf("5.Load game\n");
   		printf("6.Exit\n");
   		printf("-------------------------------------\n");
   		printf("Enter your choide: ");
   		
   		if(scanf("%d", &choice) != 1){
   			printf("Invalid input. Please enter a number.\n");
   			while(getchar() != '\n');
   			continue;
		   }
		   switch(choice){
		   	case 1:
		   		doGameTick(game);
		   		break;
		   	case 2:
		   		saveGame(game, 0);
		   		break;
		   	case 3:
		   		viewStats(game);
		   		break;
		   	case 4:
		   		initGame(game);
		   		break;
		   	case 5:
		   		loadGame();
				break;
			case 6:
				printf("Goodbye ! Thanks for playing!\n");
				return;
			default:
				printf("Please input a number in rang 1-5.\n");
		   }
	    }
		   else {
		   	printf("\n============MAIN MENU-------------\n");
		   	printf("1. New game\n");
		   	printf("2. Load game\n");
		   	printf("3. Exit\n");
		   	printf("------------------------------------\n");
		   	printf("Enter your choice: ");
		   	
		   	if(scanf("%d", &choice) != 1){
		   		printf("Invalid input. Please enter a number.\n");
		   		while(getchar() != '\n');
		   		continue;
			   }
			   switch(choice){
			   	case 1:
			   		initGame(game);
			   		doGameTick(game);
			   		break;
			   	case 2:
			   		loadGame(game);
			   		doGameTick(game);
			   		break;
			   	case 3:
			   		printf("Goodbye! Thanks for playing!\n");
			   		return;
			   	default:
			   		printf("Please input a number in range 1-3.\n");
			   }
		   }
	   }
   }
