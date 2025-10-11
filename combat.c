//wrote code while sleepy, might have mistake here and ther (maybe alot even), please go easy on me.. coding this part is not that fun when it all come to logi where i have to look it up somewhere else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_object.h"
#define MAX_MONSTERS 10
#define ALLY_COUNT 3
// Combat status: 0 continue, 1 victory, 2 defeat
extern void openInventory(Game *game); // item.c
extern void useSkill(Champion *c, void *targets, int targetCount, int isMonster); // character.c
int allMonstersDefeated(Monster enemies[], int enemyCount);
int allChampionsDefeated(Game *game);
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount);
void showMonsterDetails(Monster *m, int index);
void showChampionDetails(Champion *c, int index);
void printCombatStatus(Game *game, Monster enemies[], int enemyCount);
int selectAliveMonster(Monster enemies[], int enemyCount);
int selectAliveChampion(Game *game);
void initCombat(Game *game, Monster enemies[], int enemyCount) {
    if (enemyCount > MAX_MONSTERS) enemyCount = MAX_MONSTERS;
    printf("\nCOMBAT START\n");
    printf("%d monsters appear!\n", enemyCount);
    // local copy of enemies to avoid memory usage.. hopefully
    Monster localEnemies[MAX_MONSTERS];
    int i;
    for (i = 0; i < enemyCount; ++i) {
        localEnemies[i] = enemies[i];
    }
    int combatStatus = 0; // 0 mmean continue, 1 is victory, 2 is defeat
    // Main loop: run until all allies or all enemies are dead
    while (combatStatus == 0) {
        int maxSlots = (ALLY_COUNT > enemyCount) ? ALLY_COUNT : enemyCount;
        int slot;
        for (slot = 0; slot < maxSlots && combatStatus == 0; ++slot) {
            // Ally turn (if exists and alive)
            if (slot < ALLY_COUNT) {
                Champion *ally = &game->champion[slot];
                if (ally->health > 0) {
                    int turnDone = 0;
                    int attackedThisTurn = 0;
                    while (!turnDone) {
                        printf("\n========[ Combat Option Menu ]========\n");
                        printf("\n=== Ally Slot %d's Turn (HP: %d/%d) ===\n", slot + 1, ally->health, ally->maxHealth);
                        printf("[1] Attack\n");
                        printf("[2] Use Skill\n");
                        printf("[3] Use Item\n");
                        printf("[4] View Stats\n");
                        printf("[5] Skip Turn\n");
                        printf("Choose action: ");
                        int choice = 0;
                        if (scanf("%d", &choice) != 1) {
                            int ch;
                            while ((ch = getchar()) != '\n' && ch != EOF) {}
                            printf("Invalid input.\n");
                            continue;
                        }
                        if (choice == 1) { // Attack
                            if (attackedThisTurn) {
                                printf("You've already attacked this turn.\n");
                                continue;
                            }
                            int target = selectAliveMonster(localEnemies, enemyCount);
                            if (target < 0) {
                                printf("No valid targets to attack.\n");
                                // If all monsters dead then end encounter here return to explore and looting
                                if (allMonstersDefeated(localEnemies, enemyCount)) {
                                    combatStatus = 1;
                                    break;
                                } else {
                                    continue;
                                }
                            }
                            Monster *m = &localEnemies[target];
                            int dmg = ally->damage;
                            m->health -= dmg;
                            if (m->health < 0) m->health = 0;
                            printf("Ally %d attacks %s for %d damage! (HP now %d/%d)\n",
                                   slot + 1, m->name, dmg, m->health, m->maxHealth);
                            attackedThisTurn = 1;
                            turnDone = 1; // attack ends ally's turn
                            if (allMonstersDefeated(localEnemies, enemyCount)) {
                                combatStatus = 1; // victory
                                break;
                            }
                        } else if (choice == 2) { //Skill usage
                            printf("Using skill (delegated to useSkill)...\n");
                            useSkill(ally, (void *)localEnemies, enemyCount, 1); // isMonster = 1 (targets are monster)
                            turnDone = 1;

                            if (allMonstersDefeated(localEnemies, enemyCount)) {
                                combatStatus = 1;
                                break;
                            }
                        } else if (choice == 3) { // Use Item
                            printf("Opening inventory (delegated to openInventory)...\n");
                            openInventory(game);
                        } else if (choice == 4) { // View Stats
                            viewStatsMenu(game, localEnemies, enemyCount);
                        } else if (choice == 5) { // Skip turn
                            printf("Ally %d skips their turn.\n", slot + 1);
                            turnDone = 1;
                        } else {
                            printf("Invalid choice.\n");
                        }
                    } //while loop ended
                    if (combatStatus != 0) break;
                }
            //} 
            //if (allMonstersDefeated(localEnemies, enemyCount)) {
            //    combatStatus = 1;
            //    break;
            //}
            // Enemy turn (slot-th enemy)
            if (slot < enemyCount) {
                Monster *enemy = &localEnemies[slot];
                if (enemy->health > 0) {
                    int possibleTargets[ALLY_COUNT];
                    int aliveCount = 0;
                    int c;
                    for (c = 0; c < ALLY_COUNT; ++c) {
                        if (game->champion[c].health > 0) {
                            possibleTargets[aliveCount++] = c;
                        }
                    }
                    if (aliveCount == 0) {
                        combatStatus = 2; // defeat
                        break;
                    }
                    int randIndex = rand() % aliveCount;
                    int targetIdx = possibleTargets[randIndex];
                    Champion *target = &game->champion[targetIdx];
                                        int enemyAction = rand() % 100;
                 else {
                    int dmg = enemy->damage;
                    target->health -= dmg;
                    if (target->health < 0) target->health = 0;
                    printf("\n%s attacks Champion %d for %d damage! (HP now %d/%d)\n",
                           enemy->name, targetIdx + 1, dmg, target->health, target->maxHealth);
                    if (allChampionsDefeated(game)) {
                        combatStatus = 2; // defeat
                        break;
                    }
                } 
            } 
            if (combatStatus == 0) {
                printCombatStatus(game, localEnemies, enemyCount);
            }
        } 
    } // end main while
    if (combatStatus == 2) {
        printf("\n=== DEFEAT ===\n");
        printf("All champions are defeated!\n");
    } else if (combatStatus == 1) {
        printf("\n=== VICTORY ===\n");
        printf("All monsters are defeated!\n");
        printf("Calculating experience and preparing loot...\n");
    }
    int i;
    for (i = 0; i < enemyCount; ++i) {
        enemies[i].health = localEnemies[i].health;
    }
}
int allMonstersDefeated(Monster enemies[], int enemyCount) {
    int i;
    for (i = 0; i < enemyCount; ++i) {
        if (enemies[i].health > 0) return 0;
    }
    return 1;
}
int allChampionsDefeated(Game *game) {
    int i;
    for (i = 0; i < ALLY_COUNT; ++i) {
        if (game->champion[i].health > 0) return 0;
    }
    return 1;
}
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount) {
    while (1) {
        printf("\n=== VIEW STATS ===\n");
        printf("[1] View Champion Stats\n");
        printf("[2] View Monster Stats\n");
        printf("[0] Back\n");
        printf("Choice: ");
        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Invalid input.\n");
            continue;
        }
        if (choice == 0) break;
        if (choice == 1) {
            printf("\nSelect Champion:\n");
            int i;
            for (i = 0; i < ALLY_COUNT; ++i) {
                printf("[%d] Champion %d\n", i + 1, i + 1);
            }
            printf("Choice: ");
            int champChoice = 0;
            if (scanf("%d", &champChoice) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} continue; }
            champChoice--;
            if (champChoice >= 0 && champChoice < ALLY_COUNT) {
                showChampionDetails(&game->champion[champChoice], champChoice + 1);
            } else {
                printf("Invalid champion.\n");
            }
        } else if (choice == 2) {
            printf("\nSelect Monster:\n");
            int i;
            for (i = 0; i < enemyCount; ++i) {
                if (enemies[i].health > 0) {
                    printf("[%d] %s\n", i + 1, enemies[i].name);
                }
            }
            printf("Choice: ");
            int monChoice = 0;
            if (scanf("%d", &monChoice) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} continue; }
            monChoice--;
            if (monChoice >= 0 && monChoice < enemyCount && enemies[monChoice].health > 0) {
                showMonsterDetails(&enemies[monChoice], monChoice + 1);
            } else {
                printf("Invalid monster.\n");
            }
        } else {
            printf("Invalid choice.\n");
        }
    }
}
void showMonsterDetails(Monster *m, int index) {
    printf("\n--- Monster %d ---\n", index);
    printf("Name: %s\n", m->name);
    printf("HP: %d/%d\n", m->health, m->maxHealth);
    printf("Damage: %d\n", m->damage);
    printf("------------------\n");
}
void showChampionDetails(Champion *c, int index) {
    printf("\n--- Champion %d ---\n", index);
    printf("HP: %d/%d\n", c->health, c->maxHealth);
    printf("Damage: %d\n", c->damage);
    printf("Class: %d\n", c->class);
    printf("-------------------\n");
}
void printCombatStatus(Game *game, Monster enemies[], int enemyCount) {
    printf("\n=== COMBAT STATUS ===\n");
    printf("\nChampions:\n");
    int i;
    for (i = 0; i < ALLY_COUNT; ++i) {
        Champion *c = &game->champion[i];
        if (c->health > 0) {
            printf(" Champion %d: %d/%d HP\n", i + 1, c->health, c->maxHealth);
        } else {
            printf(" Champion %d: DEFEATED\n", i + 1);
        }
    }
    printf("\nMonsters:\n");
    int i;
    for (i = 0; i < enemyCount; ++i) {
        if (enemies[i].health > 0) {
            printf(" %s: %d/%d HP\n", enemies[i].name, enemies[i].health, enemies[i].maxHealth);
        } else {
            printf(" %s: DEFEATED\n", enemies[i].name);
        }
    }
    printf("=====================\n");
}
// If all enemies are dead, return -1 (no valid targets left)
int selectAliveMonster(Monster enemies[], int enemyCount) {
    int anyAlive = 0;
    int i;
    for (i = 0; i < enemyCount; ++i) if (enemies[i].health > 0) anyAlive = 1;
    if (!anyAlive) return -1;
    while (1) {
        printf("\n--- Select Target ---\n");
        int i;
        for (i = 0; i < enemyCount; ++i) {
            if (enemies[i].health > 0) {
                printf("[%d] %s (HP: %d/%d)\n", i + 1, enemies[i].name, enemies[i].health, enemies[i].maxHealth);
            }
        }
        printf("Select target (0 to cancel): ");
        int targetChoice = 0;
        if (scanf("%d", &targetChoice) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Invalid input.\n");
            continue;
        }
        if (targetChoice == 0) return -1;
        targetChoice--;
        if (targetChoice >= 0 && targetChoice < enemyCount && enemies[targetChoice].health > 0) {
            return targetChoice;
        } else {
            printf("Invalid target!\n");
        }
    }
}
int selectAliveChampion(Game *game) {
    int i;
    for (i = 0; i < ALLY_COUNT; ++i) {
        if (game->champion[i].health > 0) return i;
    }
    return -1;
}
