#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_object.h"

// Show item currently on sale in shop
void showShop(Game *game) {
    printf("\n======== SHOP ========\n");
    int i = 0;
    Node *node = game->shop.itemList.head;
    for (;i < game->shop.itemList.size;i++) {
        Item * item = (Item *) node->value;
        printf("%d. %s - %d Gold\n", i + 1, item->name, item->value);
        node++;
    }
    printf("%d. Thoat\n", i + 1);
}

// Show item and selling price in player's inventory
void showInventory(Game *game) {
    printf("\n======== INVENTORY ========\n");
    int i = 0;
    if (game->itemList.size == 0) {
        printf("Ban chua co do nao.\n");
    } else {
        Node *node =  game->itemList.head;

        while (node != NULL) {
            Item *item = (Item *) node->value;
            printf("%d. %s - Ban duoc %.2lf Gold\n", i + 1, item->name,  (item->value * game->config.sellValue));
            node = node->next;
            i++;
        }
    }
    printf("%d. Thoat\n", i + 1);
}

// Buy item from shop
void buyItem(Game *game) {
    int choice;
    while (1) {
        showShop(game);
        printf("Chon do muon mua: ");
        scanf("%d", &choice);

        if (choice == game->shop.itemList.size + 1)
            break;

        if (choice < 1 || choice > game->shop.itemList.size) {
            printf("Lua chon khong hop le!\n");
            continue;
        }

        Item *it = (Item *) getElementAt(game->shop.itemList, choice-1)->value;
        if (game->gold >= it->value || game->isTester == 1) {
            game->gold -= it->value;
            Item *newItem = malloc(sizeof(Item));
            // Value copy item to inventory
            *newItem = *it;
            insert(&(game->itemList), newItem);
            printf("Ban da mua %s! (Con %d Gold)\n", it->name, game->gold);
        } else {
            printf("Ban khong du tien!\n");
        }
    }
}

// Sell item in inventory
void sellItem(Game *game) {
    int choice;
    while (1) {
        showInventory(game);
        printf("Chon do muon ban: ");
        scanf("%d", &choice);

        if (choice == game->itemList.size + 1)
            break;

        if (choice < 1 || choice > game->itemList.size) {
            printf("Lua chon khong hop le!\n");
            continue;
        }

        Item *it = (Item *) getElementAt(game->itemList, choice - 1)->value;
        int goldEarned = it->value * game->config.sellValue;
        game->gold += goldEarned;
        
        removeAt(&(game->itemList), choice - 1);

        printf("Ban da ban %s va nhan duoc %d Gold! (Tong: %d Gold)\n", it->name, goldEarned, game->gold);
    }
}

// Show shop interface
void openShop(Game * game) {
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