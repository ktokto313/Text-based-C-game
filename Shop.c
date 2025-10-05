#include <stdio.h>
#include <string.h>
 type struct{
 	char name[30];
 	int price;
 }
    item;
    
    item shopitems[] = {
    	{"kiem go", 100};
    	{"kiem sat", 200};
    	{"giap da", 150};
    	{"binh mau", 50};
}
int shopSize = 4;
    
    item inventory[10];
    int invSize = 0;
    int gold = 500;
    
    // hien thi danh sach do shop
    void showShop(){
    	printf("\n========shop=========\n");
    	int i;
    	for (i = 0; i < shopSize; i++){
    		printf("%d. %s -%d Gold\n", i + 1, shopitem[i].name, shopitems[i].price);
    		}
    		printf("%d. thoat\n", shipSize + 1);
	}
	// hien thi tui do
	viod showInventory(){
		printf("\n=======Inventory========\n");
		int i;
		if(invSize == 0){
			printf("Ban chua co do nao.\n");
		}
		else {
			for (i = 0; i < invSize; i++){
				 printf("%d. %s - Ban duoc %d Gold\n", i + 1, inventory[i].name, inventory[i].price / 2);
			}
		}
		printf("%d. thoat\n", invSize + 1);
	}
	// mua do
	void buyItem(){
		int choide;
		while(1){
			showShop();
			printf("chon do muon mua:");
			scanf ("%d", choice);
			
			if (choice == shopSize + 1){
				break;
			}
			if(choice < 1 || choice > shopSize){
				printf("Lua chon khong hop le!\n");
				continue;
			}
			Item it = shopItems[choice - 1];
			if(gold >= it.price){
				gold -= it.price;
				inventory[InvSize++] == it;
				printf("Ban da mua %s! (Con %d Gold)\n", it.name, gold);
			}
			else {
				printf("Ban khong du tien!\n");
			}
		}
	}
	// ban do
	void sellItem(){
		int choide;
		while(1){
			showInventory();
			printf("Chon do muon ban");
			scanf ("%d", &choice);
			
			if(choide == invSize + 1){
				breakl
			}
			if (choide < 1 || choice > invSize){
				printf("Lua chon khong hop le!\n");
				continue;
			}
			continue;
}
            Item it = inventory[choice - 1];
            int goldEarned = it.price * game->config.sellValue;  
            gold += goldEarned
			for (int i = choice - 1; i < invSize - 1; i++){
				inventory[i] = inventory[i + 1];
			}
			invSize--;
		}
	}
	void openShop(Game *game) {
    int choice;

    while (1) {
        printf("\n======= SHOP MENU =======\n");
        printf("1. Mua do\n");
        printf("2. Ban do\n");
        printf("3. Thoat\n");
        printf("Lua chon: ");
        scanf("%d", &choice);
            switch (choice) {
    case 1:
        buyItem(shopItems, shopSize, inventory, &invSize, &gold);
        break;

    case 2:
        sellItem(inventory, &invSize, &gold);
        break;

    case 3:
        printf("Thoat shop. Bye!\n");
        return 0;

    default:
        printf("Lua chon khong hop le!\n");
}
		}
		return 0;
	}
