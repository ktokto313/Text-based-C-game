#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventory.h"

Item *createItemInstance(const Item *templateItem, int qty) {
    (void)qty;
    Item *it = malloc(sizeof(Item));
    if (!it) return NULL;
    *it = *templateItem;
    return it;
}

void addItemToInventory(Game *game, Item *item) {
    if (!game || !item) return;
    insert(&(game->itemList), item);
    printf("Added %s to inventory\n", item->name);
}
void openInventoryMenu(Game *game) {
    int choice;
    while (1) {
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║ %-44s ║\n", "INVENTORY");
        printf("╠══════════════════════════════════════════════╣\n");
        
        if (game->itemList.size == 0) {
            printf("║ %-44s ║\n", "You have no items.");
        } else {
            Node *node = game->itemList.head;
            int i = 1;
            while (node) {
                Item *item = (Item *)node->value;
                char line[128];
                if (item->type == HEALTH_POTION) {
                    snprintf(line, sizeof(line), "[%d] %s (Heals %d HP)", i, item->name, item->effectValue);
                } else {
                    snprintf(line, sizeof(line), "[%d] %s", i, item->name);
                }
                printf("║ %-44s ║\n", line);
                node = node->next;
                i++;
            }
        }
        
        printf("║ %-44s ║\n", "[0] Exit");
        printf("╚══════════════════════════════════════════════╝\n");
        printf("Select item (0 to exit): ");
        scanf("%d", &choice);
        
        if (choice == 0) break;
        if (choice < 0 || choice > game->itemList.size) {
            printf("Invalid choice\n");
            continue;
        }
        
        Node *selNode = getElementAt(game->itemList, choice - 1);
        if (!selNode) {
            printf("Could not find item\n");
            continue;
        }
        
        Item *sel = (Item *)selNode->value;
        
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║ %-44s ║\n", sel->name);
        printf("╠══════════════════════════════════════════════╣\n");
        char info[128];
        if (sel->type == HEALTH_POTION) {
            snprintf(info, sizeof(info), "Type: Consumable | Heals: %d HP", sel->effectValue);
        } else {
            snprintf(info, sizeof(info), "Type: Special | Value: %d", sel->value);
        }
        printf("║ %-44s ║\n", info);
        printf("╠══════════════════════════════════════════════╣\n");
        printf("║ %-44s ║\n", "[1] Use");
        printf("║ %-44s ║\n", "[2] Drop");
        printf("║ %-44s ║\n", "[0] Back");
        printf("╚══════════════════════════════════════════════╝\n");
        
        int action;
        printf("Action: ");
        scanf("%d", &action);
        
        if (action == 1) {
            if (sel->type == HEALTH_POTION) {
                printf("\nSelect champion to use on:\n");
                for (int i = 0; i < 3; i++) {
                    if (game->champion[i].maxHealth > 0) {
                        printf("[%d] %s - HP: %d/%d\n", i + 1, 
                               champion_string[game->champion[i].class],
                               game->champion[i].health, game->champion[i].maxHealth);
                    }
                }
                printf("[0] Cancel\nChoice: ");
                int targetChoice;
                scanf("%d", &targetChoice);
                
                if (targetChoice > 0 && targetChoice <= 3 && game->champion[targetChoice - 1].maxHealth > 0) {
                    int idx = targetChoice - 1;
                    game->champion[idx].health += sel->effectValue;
                    if (game->champion[idx].health > game->champion[idx].maxHealth)
                        game->champion[idx].health = game->champion[idx].maxHealth;
                    
                    printf("%s used on %s. Healed %d HP.\n", sel->name, 
                           champion_string[game->champion[idx].class], sel->effectValue);
                    Node *rem = removeAt(&(game->itemList), choice - 1);
                    if (rem) { free(rem->value); free(rem); }
                }
            } else {
                printf("Cannot use this item directly.\n");
            }
            
        } else if (action == 2) {
            printf("Dropping %s...\n", sel->name);
            Node *rem = removeAt(&(game->itemList), choice - 1);
            if (rem) { free(rem->value); free(rem); }
            printf("Item dropped.\n");
        }
    }
}
void handleMonsterDrops(Game *game, LinkedList *drops, int autoPickup) {
    (void)autoPickup;
    if (!drops) return;
    Node *node = drops->head;
    while (node) {
        Item *it = (Item *) node->value;
        addItemToInventory(game, it);
        node = node->next;
    }
}
void printInventory(Game *game) {
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║ %-44s ║\n", "INVENTORY");
    printf("╠══════════════════════════════════════════════╣\n");
    
    if (!game || game->itemList.size == 0) {
        printf("║ %-44s ║\n", "(empty)");
    } else {
        Node *n = game->itemList.head;
        int idx = 1;
        while (n) {
            Item *it = (Item *)n->value;
            char line[128];
            if (it->type == HEALTH_POTION) {
                snprintf(line, sizeof(line), "%d: %s (heal %d HP)", 
                        idx, it->name, it->effectValue);
            } else {
                snprintf(line, sizeof(line), "%d: %s (val %d eff %d)", 
                        idx, it->name, it->value, it->effectValue);
            }
            printf("║ %-44s ║\n", line);
            n = n->next;
            idx++;
        }
    }
    
    printf("╚══════════════════════════════════════════════╝\n");
}
