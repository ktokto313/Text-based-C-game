#include <stdio.h>
#include <stdlib.h>

void doTraining(Game * game) {
	
}

void doGameTick(Game * game){
	int choide;
	while(1){
		if (game->level == 0) {
			printf("\n====MAIN MENU (Day %d) ===\n", day);
			printf("1. Shop\n");
			printf("2. Di chuyen\n");
			printf("3. Huan luyen\n");
			printf("4. Nghi ngoi\n");
			printf("5. Thoat\n");
			printf("Chon: ");
			scanf("%d, &choice");
			
			switch(choice){
				case 1:
					openShop();
					break;
			    case 2:
			    	move();
			    	break;
			    case 3:
			    	doTraining();
			    	break;
			    case 4:
			    	rest();
			    	break;
			    case 5:
			    	return;
				default:
					printf("Lua chon khong hop le.\n");
			}
		} else {
			printf("\n====MAIN MENU (Day %d) ===\n", day);
		    printf("1. Kham pha\n");
	        printf("2. Di chuyen\n");
	        printf("3. Bien ve\n");
	        printf("4. Thoat\n");
	        printf("Chon: ");
			scanf("%d", &choice);
			
			switch(choice){
				case 1:
					explore();
					break;
			    case 2:
			    	move();
			    	break;
			    case 3:
			    	go_back();
			    	break;
			    case 4:
			    	return;
				default:
					printf("Lua chon khong hop le.\n");
			}
	        
		}
	}
	return 0;
}
