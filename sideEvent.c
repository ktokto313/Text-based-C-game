#include <stdio.h>
#include <stdlib.h>
#include "game_object.h"
void handleSideEvent(Game* game) {
    int eventType = rand() % 2;
    
    if (eventType == 0) {
        handleTrap(game);
    } else {
        handleChest(game);
    }
}
void handleTrap(Game* game) {
    printf("You triggered a trap!\n");
    printf("1. Disarm\n2. Take damage\n");
    printf("Choice: ");
    int trapChoice;
    scanf("%d", &trapChoice);
    switch (trapChoice) {
        case 1: {
            int disarmChance = game->config.trapDisarmChance;
            if (rand() % 100 < disarmChance) {
                printf("Successfully disarmed the trap!\n");
                printf("You gain %d EXP for your skill.\n", game->config.trapDisarmExp);
                game->champion[0].xp += game->config.trapDisarmExp;
            } else {
                printf("Failed! The trap activates!\n");
                printf("You take %d damage!\n", game->config.trapDamage);
                game->champion[0].health -= game->config.trapDamage;
                if (game->champion[0].health < 0) game->champion[0].health = 0;
            }
            break;
        }
        case 2:
            printf("Taking trap damage!\n");
            printf("You take %d damage!\n", game->config.trapDamage);
            game->champion[0].health -= game->config.trapDamage;
            if (game->champion[0].health < 0) game->champion[0].health = 0;
            break;
        default:
            printf("Invalid choice. Taking damage by default.\n");
            game->champion[0].health -= game->config.trapDamage;
            if (game->champion[0].health < 0) game->champion[0].health = 0;
            break;
    }
}
void handleChest(Game* game) {
    printf("Found a locked chest!\n");
    int chestChoice;
    while (1) {
        printf("1. Lockpick\n2. Force open\n3. Leave\n");
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
                    game->champion[0].gold += goldFound;
                    if (rand() % 100 < game->config.chestItemChance) {
                        printf("You also find a special item!\n");
                        // noting.. or.. noted: Add item to inventory when inventory system ready
                    }
                    return;
                } else {
                    printf("Lockpicking failed.\n");
                    printf("The lock jams. You can try forcing it or leave.\n");
                    printf("1. Force open\n2. Leave\n");
                    printf("Choice: ");
                    int retryChoice;
                    scanf("%d", &retryChoice);
                    if (retryChoice == 1) {
                        int goldMin = game->config.chestGoldMin;
                        int goldMax = game->config.chestGoldMax;
                        int goldFound = (goldMin + rand() % (goldMax - goldMin + 1)) / 2;
                        
                        printf("You force the chest open!\n");
                        printf("Some contents are damaged, but you recover %d gold.\n", goldFound);
                        game->champion[0].gold += goldFound;
                        return;
                    } else {
                        printf("You leave the chest alone.\n");
                        return;
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
                game->champion[0].gold += goldFound;
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