#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_object.h"
#define MAX_COMBAT_MONSTERS 10
extern void openInventory(Game *game);
extern void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount);
int allMonstersDefeated(Monster enemies[], int enemyCount);
int allChampionsDefeated(Game *game);
void printCombatStatus(Game *game, Monster enemies[], int enemyCount);
int selectTarget(Monster enemies[], int enemyCount);
void showMonsterStats(Monster *m, int index);
void showChampionStats(Champion *c, int index);
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount);
void initCombat(Game *game) {
    printf("\n=== COMBAT START ===\n");
    LinkedList *monsterList = (LinkedList *)(game->mapMonsterList + game->level * sizeof(LinkedList));
    int totalMonsters = monsterList->size;
    if (totalMonsters == 0) {
        printf("No monsters to fight!\n");
        return;
    }
    int fightCount = game->config.maxCombatEnemies;
    if (fightCount > totalMonsters) fightCount = totalMonsters;
    if (fightCount > MAX_COMBAT_MONSTERS) fightCount = MAX_COMBAT_MONSTERS;
    Monster localEnemies[MAX_COMBAT_MONSTERS];
    int enemyCount = 0;
    int selected[MAX_COMBAT_MONSTERS] = {0};
    while (enemyCount < fightCount) {
        int randIndex = rand() % totalMonsters;
        int alreadyPicked = 0;
        int i;
        for (i = 0; i < enemyCount; i++) {
            if (selected[i] == randIndex) {
                alreadyPicked = 1;
                break;
            }
        }
        if (!alreadyPicked) {
            Node *node = getElementAt(*monsterList, randIndex);
            if (node != NULL) {
                Monster *m = (Monster *)node->value;
                localEnemies[enemyCount] = *m;
                selected[enemyCount] = randIndex;
                enemyCount++;
            }
        }
    }
    printf("%d monsters appear!\n", enemyCount);
    int championIndex = 0;
    int monsterIndex = 0;
    while (1) {
        while (championIndex < 3 && game->champion[championIndex].health <= 0) {
            championIndex++;
        }
        if (championIndex >= 3) {
            championIndex = 0;
            continue;
        }
        if (allChampionsDefeated(game)) {
            printf("\n=== DEFEAT ===\n");
            printf("All champions defeated!\n");
            game->initialized = 0;
            return;
        }
        Champion *ally = &game->champion[championIndex];
        printf("\n=== Champion %d's Turn (HP: %d/%d) ===\n", 
               championIndex + 1, ally->health, ally->maxHealth);
        printf("[1] Attack\n");
        printf("[2] Use Skill\n");
        printf("[3] Use Item\n");
        printf("[4] View Stats\n");
        printf("Choose action: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input.\n");
            continue;
        }
        switch (choice) {
            case 1: {
                int target = selectTarget(localEnemies, enemyCount);
                if (target < 0) {
                    if (allMonstersDefeated(localEnemies, enemyCount)) {
                        printf("\n=== VICTORY ===\n");
                        printf("All monsters defeated!\n");
                        printf("Calculating experience...\n");
                        return;
                    }
                    continue;
                }
                localEnemies[target].health -= ally->damage;
                if (localEnemies[target].health < 0) {
                    localEnemies[target].health = 0;
                }
                printf("Champion %d attacks %s for %d damage! (HP: %d/%d)\n",
                       championIndex + 1, localEnemies[target].name, 
                       ally->damage, localEnemies[target].health, 
                       localEnemies[target].maxHealth);
                
                if (allMonstersDefeated(localEnemies, enemyCount)) {
                    printf("\n=== VICTORY ===\n");
                    printf("All monsters defeated!\n");
                    printf("Calculating experience...\n");
                    return;
                }
                championIndex++;
                if (championIndex >= 3) championIndex = 0;
                break;
            }
            case 2:
                useSkill(ally, game, localEnemies, enemyCount);
                if (allMonstersDefeated(localEnemies, enemyCount)) {
                    printf("\n=== VICTORY ===\n");
                    printf("All monsters defeated!\n");
                    printf("Calculating experience...\n");
                    return;
                }
                championIndex++;
                if (championIndex >= 3) championIndex = 0;
                break;
            case 3:
                openInventory(game);
                break;
            case 4:
                viewStatsMenu(game, localEnemies, enemyCount);
                break;
            
            default:
                printf("Invalid choice.\n");
                break;
        }
        while (monsterIndex < enemyCount && localEnemies[monsterIndex].health <= 0) {
            monsterIndex++;
        }
        if (monsterIndex >= enemyCount) {
            monsterIndex = 0;
            continue;
        }
        if (allMonstersDefeated(localEnemies, enemyCount)) {
            printf("\n=== VICTORY ===\n");
            printf("All monsters defeated!\n");
            printf("Calculating experience...\n");
            return;
        }
        Monster *enemy = &localEnemies[monsterIndex];
        int aliveIndices[3];
        int aliveCount = 0;
        int i;
        for (i = 0; i < 3; i++) {
            if (game->champion[i].health > 0) {
                aliveIndices[aliveCount] = i;
                aliveCount++;
            }
        }
        if (aliveCount == 0) {
            printf("\n=== DEFEAT ===\n");
            printf("All champions defeated!\n");
            game->initialized = 0;
            return;
        }
        int targetIdx = aliveIndices[rand() % aliveCount];
        game->champion[targetIdx].health -= enemy->damage;
        if (game->champion[targetIdx].health < 0) {
            game->champion[targetIdx].health = 0;
        }
        printf("\n%s attacks Champion %d for %d damage! (HP: %d/%d)\n",
               enemy->name, targetIdx + 1, enemy->damage,
               game->champion[targetIdx].health, 
               game->champion[targetIdx].maxHealth);
        
        if (allChampionsDefeated(game)) {
            printf("\n=== DEFEAT ===\n");
            printf("All champions defeated!\n");
            game->initialized = 0;
            return;
        }
        monsterIndex++;
        if (monsterIndex >= enemyCount) monsterIndex = 0;
        
        printCombatStatus(game, localEnemies, enemyCount);
    }
}
int allMonstersDefeated(Monster enemies[], int enemyCount) {
    int i;
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i].health > 0) return 0;
    }
    return 1;
}
int allChampionsDefeated(Game *game) {
    int i;
    for (i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) return 0;
    }
    return 1;
}
int selectTarget(Monster enemies[], int enemyCount) {
    while (1) {
        printf("\n--- Select Target ---\n");
        int i;
        for (i = 0; i < enemyCount; i++) {
            if (enemies[i].health > 0) {
                printf("[%d] %s (HP: %d/%d)\n", 
                       i + 1, enemies[i].name, 
                       enemies[i].health, enemies[i].maxHealth);
            }
        }
        printf("[0] Cancel\n");
        printf("Select: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input.\n");
            continue;
        } 
        if (choice == 0) return -1;
        choice--;
        if (choice >= 0 && choice < enemyCount && enemies[choice].health > 0) {
            return choice;
        }
        printf("Invalid target!\n");
    }
}
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount) {
    while (1) {
        printf("\n=== VIEW STATS ===\n");
        printf("[1] View All Champions\n");
        printf("[2] View Monster\n");
        printf("[0] Back\n");
        printf("Choice: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        if (choice == 0) break;
        if (choice == 1) {
            printf("\n--- All Champions ---\n");
            int i;
            for (i = 0; i < 3; i++) {
                Champion *c = &game->champion[i];
                printf("Champion %d: HP %d/%d | Damage: %d | Class: %d%s\n",
                       i + 1, c->health, c->maxHealth, c->damage, c->class,
                       c->health <= 0 ? " [DEFEATED]" : "");
            }
        } else if (choice == 2) {
            printf("\nSelect Monster:\n");
            int i;
            for (i = 0; i < enemyCount; i++) {
                if (enemies[i].health > 0) {
                    printf("[%d] %s\n", i + 1, enemies[i].name);
                }
            }
            printf("Choice: ");
            int monChoice;
            if (scanf("%d", &monChoice) != 1) {
                while (getchar() != '\n');
                continue;
            }
            monChoice--;
            if (monChoice >= 0 && monChoice < enemyCount && enemies[monChoice].health > 0) {
                showMonsterStats(&enemies[monChoice], monChoice + 1);
            }
        }
    }
}
void showMonsterStats(Monster *m, int index) {
    printf("\n--- Monster %d ---\n", index);
    printf("Name: %s\n", m->name);
    printf("HP: %d/%d\n", m->health, m->maxHealth);
    printf("Damage: %d\n", m->damage);
    printf("------------------\n");
}
void printCombatStatus(Game *game, Monster enemies[], int enemyCount) {
    printf("\n=== COMBAT STATUS ===\n");
    printf("Champions:\n");
    int i;
    for (i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) {
            printf("  [%d] HP: %d/%d\n", i + 1, 
                   game->champion[i].health, game->champion[i].maxHealth);
        } else {
            printf("  [%d] DEFEATED\n", i + 1);
        }
    }
    printf("Monsters:\n");
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i].health > 0) {
            printf("  %s: %d/%d HP\n", enemies[i].name, 
                   enemies[i].health, enemies[i].maxHealth);
        } else {
            printf("  %s: DEFEATED\n", enemies[i].name);
        }
    }
    printf("=====================\n");
}