#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_object.h"
#include "character.h"
#include "sideEvent.h"
#include "inventory.h"

void handleSideEvent(Game* game) {
    int eventType = rand() % 2;
    if (eventType == 0) {
        handleTrap(game);
    } else {
        handleChest(game);
    }
    return 1;
}

// Return true if the team is alive,
// return false if the team is ded
int isAlive(Game * game) {
    for (int i = 0;i < 3;i++) {
        if (game->champion[i].health != 0 && 
            game->champion[i].maxHealth != 0) return 0;
    }
    return 1;
}

int handleTrap(Game* game) {
    int trapChoice;
    while (1) {
        printf("You triggered a trap!\n");
        printf("1. Disarm\n");
        printf("2. Take damage\n");
        printf("Choice: ");
        scanf("%d", &trapChoice);
        switch (trapChoice) {
            case 1: {
                int disarmChance = game->config.trapDisarmChance;
                if (rand() % 100 < disarmChance) {
                    printf("Successfully disarmed the trap!\n");
                    printf("You gain %d EXP for your skill.\n", game->config.trapDisarmExp);
                    for (int i = 0; i < 3; i++) {
                        if (game->champion[i].health > 0) {
                            addXp(game, game->config.trapDisarmExp);
                            return 1;
                        }
                    }
                } else {
                    printf("Failed! The trap activates!\n");
                    printf("All champions take %d damage!\n", game->config.trapDamage);
                    for (int i = 0; i < 3; i++) {
                        if (game->champion[i].health > 0) {
                            game->champion[i].health -= game->config.trapDamage;
                            if (game->champion[i].health < 0) {
                                if (game->isTester) {
                                    printf("You were critically hit!\n");
                                    game->champion[i].health = 1;
                                } else game->champion[i].health = 0;
                            }
                        }
                    }
                    return isAlive(game);
                }
            }
            case 2:
                printf("Taking trap damage!\n");
                printf("All champions take %d damage!\n", game->config.trapDamage);
                for (int i = 0; i < 3; i++) {
                    if (game->champion[i].health > 0) {
                        game->champion[i].health -= game->config.trapDamage;
                        if (game->champion[i].health < 0) {
                            if (game->isTester) {
                                printf("You were critically hit!\n");
                                game->champion[i].health = 1;
                            } else game->champion[i].health = 0;
                        }
                    }
                }
                return isAlive(game);
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}
void handleChest(Game* game) {
    int chestChoice;
    while (1) {
        printf("Found a locked chest!\n");
        printf("1. Lockpick\n");
        printf("2. Force open\n");
        printf("3. Leave\n");
        printf("Choice: ");
        scanf("%d", &chestChoice);
        switch (chestChoice) {
            case 1: {
                int lockpickChance = game->config.lockpickChance;
                if (rand() % 100 < lockpickChance) {
                    int goldMin = game->config.chestGoldMin;
                    int goldMax = game->config.chestGoldMax;
                    int goldFound = goldMin + rand() % (goldMax - goldMin + 1);
                    printf("Lockpicked successfully!\n");
                    printf("You find %d gold inside!\n", goldFound);
                    game->gold += goldFound;
                    if (rand() % 100 < game->config.chestItemChance) {
                        // Give random item from chest loot pool
                        const char *chestLoot[] = {"Health Potion", "Large Health Potion", "Health Potion"};
                        const int chestHeals[] = {15, 25, 15};
                        int lootIdx = rand() % 3;
                        
                        Item *chestItem = (Item *)malloc(sizeof(Item));
                        if (chestItem) {
                            chestItem->type = HEALTH_POTION;
                            strcpy(chestItem->name, chestLoot[lootIdx]);
                            chestItem->value = chestHeals[lootIdx] * 5;
                            chestItem->effectValue = chestHeals[lootIdx];
                            addItemToInventory(game, chestItem);
                        }
                    }
                    return;
                } else {
                    int retryChoice;
                    while (1) {
                        printf("Lockpicking failed.\n");
                        printf("The lock jams. You can try forcing it or leave.\n");
                        printf("1. Force open\n");
                        printf("2. Leave\n");
                        printf("Choice: ");
                        scanf("%d", &retryChoice);
                        if (retryChoice == 1) {
                            int goldMin = game->config.chestGoldMin;
                            int goldMax = game->config.chestGoldMax;
                            int goldFound = (goldMin + rand() % (goldMax - goldMin + 1)) / 2;
                            printf("You force the chest open!\n");
                            printf("Some contents are damaged, but you recover %d gold.\n", goldFound);
                            game->gold += goldFound;
                            return;               
                        } else if (retryChoice == 2) {
                            printf("You leave the chest alone.\n");
                            return;
                            
                        } else {
                            printf("Invalid choice. Please try again.\n");
                        }
                    }
                }
                break;
            }
            case 2: {
                int goldMin = game->config.chestGoldMin;
                int goldMax = game->config.chestGoldMax;
                int goldFound = (goldMin + rand() % (goldMax - goldMin + 1)) / 2;
                printf("You force the chest open!\n");
                printf("Some contents are damaged, but you recover %d gold.\n", goldFound);
                game->gold += goldFound;
                return;
            }
            case 3:
                printf("You leave the chest alone.\n");
                return;                
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}