#include <stdio.h>
#include <string.h>

// cau trúc item
typedef struct {
    char name[30];
    int price;
} Item;

// Cau trúc game chia toàn bo du lieu (dóng gói)
typedef struct {
    Item shopItems[10];
    int shopSize;

    Item inventory[10];
    int invSize;

    int gold;
} Game;

// Hien thi danh sách trong shop
void showShop(Game *game) {
    printf("\n======== SHOP ========\n");
    for (int i = 0; i < game->shopSize; i++) {
        printf("%d. %s - %d Gold\n", i + 1, game->shopItems[i].name, game->shopItems[i].price);
    }
    printf("%d. Thoat\n", game->shopSize + 1);
}

// Hien thi túi do cua nguoi choi
void showInventory(Game *game) {
    printf("\n======== INVENTORY ========\n");
    if (game->invSize == 0) {
        printf("Ban chua co do nao.\n");
    } else {
        for (int i = 0; i < game->invSize; i++) {
            printf("%d. %s - Ban duoc %d Gold\n", i + 1, game->inventory[i].name, game->inventory[i].price / 2);
        }
    }
    printf("%d. Thoat\n", game->invSize + 1);
}

// Mua do tu shop
void buyItem(Game *game) {
    int choice;
    while (1) {
        showShop(game);
        printf("Chon do muon mua: ");
        scanf("%d", &choice);

        if (choice == game->shopSize + 1)
            break;

        if (choice < 1 || choice > game->shopSize) {
            printf("Lua chon khong hop le!\n");
            continue;
        }

        Item it = game->shopItems[choice - 1];
        if (game->gold >= it.price) {
            game->gold -= it.price;
            game->inventory[game->invSize++] = it;
            printf("Ban da mua %s! (Con %d Gold)\n", it.name, game->gold);
        } else {
            printf("Ban khong du tien!\n");
        }
    }
}

// Bán do trong inventory
void sellItem(Game *game) {
    int choice;
    while (1) {
        showInventory(game);
        printf("Chon do muon ban: ");
        scanf("%d", &choice);

        if (choice == game->invSize + 1)
            break;

        if (choice < 1 || choice > game->invSize) {
            printf("Lua chon khong hop le!\n");
            continue;
        }

        Item it = game->inventory[choice - 1];
        int goldEarned = it.price / 2;
        game->gold += goldEarned;

        for (int i = choice - 1; i < game->invSize - 1; i++) {
            game->inventory[i] = game->inventory[i + 1];
        }
        game->invSize--;

        printf("Ban da ban %s va nhan duoc %d Gold! (Tong: %d Gold)\n", it.name, goldEarned, game->gold);
    }
}

// Mua shop
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
                buyItem(game);
                break;
            case 2:
                sellItem(game);
                break;
            case 3:
                printf("Thoat shop. Bye!\n");
                return;
            default:
                printf("Lua chon khong hop le!\n");
        }
    }
}

// Chuong trình chính
int main() {
    Game game = {
        .shopItems = {
            {"Kiem go", 100},
            {"Kiem sat", 200},
            {"Giap da", 150},
            {"Binh mau", 50}
        },
        .shopSize = 4,
        .invSize = 0,
        .gold = 500
    };

    openShop(&game);
    return 0;
}
