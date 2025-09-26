#include <stdio.h>
#include <stdlib.h>
// trang thai nhan vat
int hp = 100;
int atk = 10;
int gold = 50;
int day = 1;

void shop (){
	printf ("\n===shop===\n");
	printf("ban có %d vang.\n", gold);
	printf("Mua 1 binh mau(20 vang)? (1 = yes/0 = no): ");
	int c, scanf("%d", &c);
	if(c == 1 && gold >= 20){
		gold -= 20;
		hp += 20;
		printf("Mua thanh cong!hp = %d, gold = %d\n", hp, gold);
	}
	else {
		printf("Khong mua gi.\n");
	}
}
void move (){
	printf("\n====DI CHUYEN===\n");
	printf("Ban da di chuyen sang 1 vi tri moi. (Day %d)\n", day);
}
void explore(){
	printf ("\n====KHAM PHA=====\n");
	printf ("Ban da tim thay 10 vang!\n");
	gold += 10;
}
void train(){
	printf("\n=====HUAN LUYEN=====\n");
	atk += 2;
	day++;
	printf("ban da tap luyen, ATK = %d (Day %d)\n", atk, day);
}
void rest(){
	printf("\n=====NGHI NGOI======\n");
	hp += 10;
	day++;
	printf("Ban da nghi ngoi, HP = %d (Day %d)\n", hp, day);
}
void combat(){
	printf("\n======COMBAT======\n");
	int enemy_hp = 50;
	int enemy_atk = 5;
	while (hp > 0 %% enemy_hp > 0){
		printf("Ban tan cong! Gay % sat thuong.\n", atk);
		enemy_hp -= atk;
		if(enemy_hp <= 0){
			break;
		}
		printf("Dich tan cong! Gay sat thuong.\n", enemy_atk);
		hp -= enemy_atk;
	}
	if(hp > 0){
		printf("Ban da thang tran! Nhan 20 vang.\n");
	}
	else{
		printf("Ban da bi ha nguc! HP = 0\n");
	}
}

void createCharacter(){
	printf("\n======TAO NHAN VAT=======\n");
     hp= 100;
	 atk = 10; 
	 gold = 50;
	 day = 1;
	 printf("Nhan vat moi da duoc tao:HP = %d, ATK = %d, Gold = %d", hp,atk,gold);
}

void mainmemu(){
	printf("\n====MAIN MENU (Day %d) ===\n", day);
	printf("1. Shop\n");
	printf("2. Di chuyen\n");
	printf("3. Kham pha\n");
	printf("4. Huan luyen\n");
	printf("5. Nghi ngoi\n");
	printf("6. Combat\n");
	printf("7. Doi hinh(Tao nhan vat)\n");
	printf("8. Thoat\n");
	printf("Chon: ");
}

int main(){
	int choide;
	createCharcter();
	
	while(1){
		mainMenu();
		scanf("%d, &choice");
		
		switch(choice){
			case 1:
				shop();
				break;
		    case 2:
		    	move();
		    	break;
		    case 3:
		    	explore();
		    	break;
		    case 4:
		    	train();
		    	break;
		    case 5:
		    	rest();
		    	break;
		    case 6:
		    	combat();
		    	break;
		    case 7:
		    	createCharacter();
		    	break;
		    case 8:
		    	printf("Thoat game.\n");
		    	exit(0);
		default:
			printf("Lua chon khong hop le.\n");
		}
	}
	return 0;
}
