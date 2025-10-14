#include "fileio.c"
#include <string>
#include "stdio.h"
#include "game.c"
#include "explore.c"
#include "combat.c"
#include "game_object.h"

void showMainMenu(Game * game);
void adminMenu();

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
   		printf("7.Admin Mode\n");
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
			case 7:
				adminMenu();
				break;
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
void adminMenu(){
	char password[20];
	int choice;
	
	printf("\n====ADMIN LOGIN====\n");
	printf("Enter admin password!\n");
	return;
	
	if(strcmp(password, "admin123") != 0){
		printf("Incorrect password!\n");
		return;
	}
	
	while(1){
		printf("\n=======ADMIN MENU========\n");
		printf("1.View all saved games\n");
		printf("2.Delete a save file\n");
		printf("3.View system info\n");
		printf("4.Back to main menu\n");
		printf("Enter your choice: ");
		
		if(scanf("%d", &choice) != 1){
			printf("Invalid input!\n");
			while(getchar()!= '\n');
			continue;
		}
		
		switch(choice){
			case 1:
				printf("\n[List of save games]\n");
				printf("-save1.dat\n-save2.dat\n");
				break;
			case 2:{
				break;
				char filename[50];
				printf("Enter file name to delete:");
				scanf("%s", file name);
				if(remove(filename) == 0)
				printf("Deleted '%s' successfully!\n", filename);
				else
				printf("Failed to delete 'S' !n", filename);
				break;
		}
		    case 3:
		    	printf("\nGame version: 1.0\nCreated by: OSG group\n\n");
		    	break;
		    case 4:
		    	printf("Returning to main menu...\n");
		    	return;
		    default:
		    	printf("Invalid choice! Please choose 1-4.\n");
	    }
    }
}
