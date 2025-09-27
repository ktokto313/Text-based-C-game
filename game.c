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
		 printf("\n=== LUA CHON HANH DONG ===\n");
        printf("1. Kham pha\n");
        printf("2. Di chuyen\n");
        printf("3. Bien ve\n");
        printf("4. Menu\n");
        printf("5. Thoat\n");
        printf("Nhap lua chon: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("-> Luong kham pha\n");
                break;
            case 2:
                printf("-> Luong di chuyen\n");
                break;
            case 3:
                printf("-> +1 buoi + luong di chuyen\n");
                break;
            case 4:
                printf("-> Luong main menu\n");
                break;
            case 5:
                printf("Thoat chuong trinh.\n");
                return 0;
            default:
                printf("Lua chon khong hop le, vui long nhap lai.\n");
        }
    }
	}
	return 0;
}
