#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventory.h"
Item *createItemInstance(const Item *templateItem, int qty) {
    (void)qty; /* qty not stored on Item in game_object.h; treat each instance as one unit */
    Item *it = malloc(sizeof(Item));
    if (!it) return NULL;
    *it = *templateItem;
    return it;
}
void addItemToInventory(Game *game, Item *item) {
    if (!game || !item) return;
    insert(&(game->itemList), item);
}
void openInventoryMenu(Game *game) {
    int choice;
    while (1) {
        printf("\n======== INVENTORY ========\n");
        if (game->itemList.size == 0) {
            printf("You have no items.\n");
        } else {
            Node *node = game->itemList.head;
            int i = 1;
            while (node) {
                Item *item = (Item *) node->value;
                if (item->type == HEALTH_POTION) {
                    printf("%d. %s\n", i, item->name);
                } else {
                    printf("%d. %s\n", i, item->name);
                }
                node = node->next;
                i++;
            }
        }
        printf("0. Exit\n");
        printf("Select item index to inspect/use/equip/drop (0 to exit): ");
        scanf("%d", &choice);
        if (choice == 0) break;
        if (choice < 0 || choice > game->itemList.size) {
            printf("Invalid choice\n");
            continue;
        }
        Node *selNode = getElementAt(game->itemList, choice-1);
        if (!selNode) {
            printf("Could not find item\n");
            continue;
        }
        Item *sel = (Item *) selNode->value;
        printf("Selected: %s\n", sel->name);
        printf("1. Use\n2. Equip/Unequip\n3. Drop\n4. Back\n");
        int action;
        printf("Action: ");
        scanf("%d", &action);
        if (action == 1) {
                if (sel->type == HEALTH_POTION) {
                for (int i=0;i<3;i++) {
                    if (game->champion[i].maxHealth > 0) {
                        game->champion[i].health += sel->effectValue;
                        if (game->champion[i].health > game->champion[i].maxHealth)
                            game->champion[i].health = game->champion[i].maxHealth;
                        printf("%s used on %s (champion %d). Healed %d.\n", sel->name, "Champion", i+1, sel->effectValue);
                        /* remove this item instance from inventory */
                        Node *rem = removeAt(&(game->itemList), choice-1);
                        if (rem) { free(rem->value); free(rem); }
                        break;
                    }
                }
            } else {
                printf("Cannot use this item directly. Try equip.\n");
            }
        } else if (action == 2) {
            for (int i=0;i<3;i++) {
                if (game->champion[i].maxHealth > 0) {
                    game->champion[i].damage += sel->effectValue;
                    printf("Equipped %s to champion %d (buff applied)\n", sel->name, i+1);
                    Node *rem2 = removeAt(&(game->itemList), choice-1);
                    if (rem2) { free(rem2->value); free(rem2); }
                    break;
                }
            }
        } else if (action == 3) {
            printf("Dropping %s\n", sel->name);
            Node *rem3 = removeAt(&(game->itemList), choice-1);
            if (rem3) { free(rem3->value); free(rem3); }
        } else {
            continue;
        }
    }
}
void handleMonsterDrops(Game *game, LinkedList *drops, int autoPickup) {
    if (!drops) return;
    Node *node = drops->head;
    while (node) {
        Item *it = (Item *) node->value;
        if (autoPickup) {
            addItemToInventory(game, it);
        } else {
            addItemToInventory(game, it);
        }
        node = node->next;
    }
}
void serializeInventory(Game *game, FILE *fp) {
}
void deserializeInventory(Game *game, FILE *fp) {
}
void printInventory(Game *game) {
    printf("\n--- INVENTORY DUMP ---\n");
    if (!game) return;
    Node *n = game->itemList.head;
    int idx = 1;
    if (!n) { printf("(empty)\n"); return; }
    while (n) {
        Item *it = (Item *) n->value;
        if (it->type == HEALTH_POTION) printf("%d: %s (heal %d)\n", idx, it->name, it->effectValue);
        else printf("%d: %s (val %d eff %d)\n", idx, it->name, it->value, it->effectValue);
        n = n->next; idx++;
    }
    printf("----------------------\n");
}
