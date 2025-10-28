#include <stdio.h>
#include <stdlib.h>
#include "sideEvent.h"
#include "movement.h"
#include "explore.h"
#include "shop.h"
#include "character.h"
#include "game_object.h"
#include "combat.h"
#include "fileio.h"

int tickInterval = 0;

void initGame(Game * game) {
	game->initialized = 1;
	// We already did this in main.c
	// game->level = 0;
	// game->day = 0;
	// game->timeOfTheDay = 0;
	// game->gold = 0;
	// for (int i = 0;i < 3;i++) {
	// 	game->champion[i].class = 0;
	// 	game->champion[i].damage = 0;
	// 	game->champion[i].health = 0;
	// 	game->champion[i].maxHealth = 0;
	// 	game->champion[i].level = 0;
	// 	game->champion[i].xp = 0;
	// }
	init(&(game->itemList));
	// editFormation(game);
	loadMapAndLocationData(game);
	loadConfig(game);
	loadShop(game);
	// saveGame(game, 1);
}

void rest(Game * game) {
	for (int i=0;i < 3;i++) {
		if (game->champion[i].maxHealth > 0) {
			game->champion[i].health = game->champion[i].maxHealth;
		}
	}
	printf("Your team rested and fully healed\n");
	addTimeOfTheDay(game, 1);
}
void doTraining(Game * game) {
	int choice;
	while (1) {
		printf("Training menu: \n");
		printf("1. Mock battle\n");
		printf("2. Change formation\n");
		printf("3. Exit\n");
		printf("Your choice: ");
		scanf("%d", &choice);
		getchar();

		switch (choice)
		{
		case 1:
			createCombat(game);
			break;
		case 2:
			editFormation(game);
			break;
		case 3:
			return;
		default:
			printf("Please input a number in range 1-3\n");
			break;
		}
	}	
}

void doGameTick(Game * game){
	int choice;
	while(1){
		if (game->level == 0) {
			printf("\n====MAIN MENU (Day %d) ===\n", game->day);
			printf("1. Shop\n");
			printf("2. Di chuyen\n");
			printf("3. Huan luyen\n");
			printf("4. Nghi ngoi\n");
			printf("5. Thoat\n");
			printf("Chon: ");
			scanf("%d", &choice);
			
			switch(choice){
				case 1:
					openShop(game);
					break;
			    case 2:
			    	move(game);
			    	break;
			    case 3:
			    	doTraining(game);
			    	break;
			    case 4:
			    	rest(game);
			    	break;
			    case 5:
			    	return;
				default:
					printf("Lua chon khong hop le.\n");
			}
		} else {
			printf("\n=== LUA CHON HANH DONG ===\n");
			printf("1. Kham pha\n");
			printf("2. Di chuyen\n");
			printf("3. Bien ve\n");
			printf("4. Thoat\n");
			printf("Nhap lua chon: ");
			scanf("%d", &choice);

			switch (choice) {
				case 1:
					if (explore(game)) {//Team died
						printf("You died! You wake up at home\n");
						teleport(game, 0);
						rest(game);
					}
					break;
				case 2:
					move(game);
					break;
				case 3:
					if (game->timeOfTheDay < game->config.maxTimeOfTheDay) {
						game->timeOfTheDay++;
						teleport(game, 0);	
					} else {
						printf("Can't teleport home when it's night time!\n");
					}
					break;
				case 4:
					return;
				default:
					printf("Lua chon khong hop le, vui long nhap lai.\n");
			}
		}
		tickInterval = tickInterval % game->config.saveInterval;
		if (tickInterval == 9) {saveGame(game, 1);}
		tickInterval++;
	}
}
