#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_object.h"
extern void useItem(Game *game);
extern void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount);
void printCombatStatus(Game *game, Monster enemies[], int enemyCount);
int selectTarget(Monster enemies[], int enemyCount);
void showMonsterStats(Monster *m, int index);
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
    Monster localEnemies[fightCount];
    int enemyCount = 0;

    Node *current = monsterList->head;
    while (current != NULL && enemyCount < fightCount) {
        Monster *m = (Monster *)current->value;
        localEnemies[enemyCount].health = m->health;
        localEnemies[enemyCount].maxHealth = m->maxHealth;
        localEnemies[enemyCount].damage = m->damage;
        strcpy(localEnemies[enemyCount].name, m->name);
        enemyCount++;
        current = current->next;
    }
    printf("%d monsters appear!\n", enemyCount);
    int championIndex = 0;
    int monsterIndex = 0;
    int downedMonster = 0;
    int totalExp = 0;
    while (1) {
        if (championIndex >= 3) championIndex = 0;
        while (game->champion[championIndex].health <= 0) {
            championIndex++;
            if (championIndex >= 3) championIndex = 0;
        }
        Champion ally = game->champion[championIndex];
        printf("\n=== Champion %d's Turn (HP: %d/%d) ===\n", 
               championIndex + 1, ally.health, ally.maxHealth);
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
                if (target < 0) continue;
                localEnemies[target].health -= ally.damage;
                if (localEnemies[target].health <= 0) {
                    localEnemies[target].health = 0;
                    downedMonster++;
                    totalExp += 50;
                }
                printf("Champion %d attacks %s for %d damage! (HP: %d/%d)\n",
                       championIndex + 1, localEnemies[target].name, 
                       ally.damage, localEnemies[target].health, 
                       localEnemies[target].maxHealth);
                if (downedMonster >= enemyCount) {
                    printf("\n=== VICTORY ===\n");
                    printf("All monsters defeated!\n");
                    printf("Experience gained: %d\n", totalExp);
                    return;
                }
                championIndex++;
                break;
            }
            case 2: {
                useSkill(&ally, game, localEnemies, enemyCount);
                downedMonster = 0;
                for (int i = 0; i < enemyCount; i++) {
                    if (localEnemies[i].health <= 0) downedMonster++;
                }
                if (downedMonster >= enemyCount) {
                    printf("\n=== VICTORY ===\n");
                    printf("All monsters defeated!\n");
                    printf("Experience gained: %d\n", totalExp);
                    return;
                }
                championIndex++;
                break;
            }
            case 3:
                useItem(game);
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
            while (monsterIndex < enemyCount && localEnemies[monsterIndex].health <= 0) {
                monsterIndex++;
            }
        }
        if (monsterIndex >= enemyCount) continue;
        int aliveChampions[3];
        int aliveCount = 0;
        for (int i = 0; i < 3; i++) {
            if (game->champion[i].health > 0) {
                aliveChampions[aliveCount++] = i;
            }
        }
        if (aliveCount == 0) {
            printf("\n=== DEFEAT ===\n");
            printf("All champions defeated!\n");
            game->initialized = 0;
            return;
        }
        int targetIdx = aliveChampions[rand() % aliveCount];
        game->champion[targetIdx].health -= localEnemies[monsterIndex].damage;
        if (game->champion[targetIdx].health < 0) {
            game->champion[targetIdx].health = 0;
        }
        printf("\n%s attacks Champion %d for %d damage! (HP: %d/%d)\n",
               localEnemies[monsterIndex].name, targetIdx + 1, 
               localEnemies[monsterIndex].damage,
               game->champion[targetIdx].health, 
               game->champion[targetIdx].maxHealth);
        aliveCount = 0;
        for (int i = 0; i < 3; i++) {
            if (game->champion[i].health > 0) aliveCount++;
        }
        if (aliveCount == 0) {
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
int selectTarget(Monster enemies[], int enemyCount) {
    while (1) {
        printf("\n--- Select Target ---\n");
        int aliveMap[enemyCount];
        int counter = 0;
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].health > 0) {
                aliveMap[i] = 1;
                printf("[%d] %s (HP: %d/%d)\n", 
                       ++counter, enemies[i].name, 
                       enemies[i].health, enemies[i].maxHealth);
            } else {
                aliveMap[i] = 0;
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
        int target = findBinaryMapping(aliveMap, choice - 1, enemyCount);
        if (target == -1) {
            printf("Invalid target!\n");
            continue;
        }
        return target;
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
            for (int i = 0; i < 3; i++) {
                Champion c = game->champion[i];
                printf("Champion %d: HP %d/%d | Damage: %d | Class: %d%s\n",
                       i + 1, c.health, c.maxHealth, c.damage, c.class,
                       c.health <= 0 ? " [DEFEATED]" : "");
            }
        } else if (choice == 2) {
            printf("\nSelect Monster:\n");
            int aliveMap[enemyCount];
            int counter = 0;
            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i].health > 0) {
                    aliveMap[i] = 1;
                    printf("[%d] %s\n", ++counter, enemies[i].name);
                } else {
                    aliveMap[i] = 0;
                }
            }
            printf("Choice: ");
            int monChoice;
            if (scanf("%d", &monChoice) != 1) {
                while (getchar() != '\n');
                continue;
            }
            int target = findBinaryMapping(aliveMap, monChoice - 1, enemyCount);
            if (target != -1) {
                showMonsterStats(&enemies[target], target + 1);
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
    for (int i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) {
            printf("  [%d] HP: %d/%d\n", i + 1, 
                   game->champion[i].health, game->champion[i].maxHealth);
        } else {
            printf("  [%d] DEFEATED\n", i + 1);
        }
    }
    printf("Monsters:\n");
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].health > 0) {
            printf("  %s: %d/%d HP\n", enemies[i].name, 
                   enemies[i].health, enemies[i].maxHealth);
        } else {
            printf("  %s: DEFEATED\n", enemies[i].name);
        }
    }
    printf("=====================\n");
}