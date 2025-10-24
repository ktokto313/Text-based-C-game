#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game_object.h"
#include "character.h"
void printCombatStatus(Game *game, Monster enemies[], int enemyCount);
int selectTarget(Monster enemies[], int enemyCount);
int selectAlly(Game *game);
void showMonsterStats(Monster *m, int index);
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount);
void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount);
void initCombat(Game *game);
void createCombat(Game *game);
void simulateCombat(Game *game);
void useItem(Game *game) {
    openInventoryMenu(game);
}
static void sanitizeChampion(Champion *c) {
    if (c == NULL) return;
    if (c->maxHealth <= 0 || c->maxHealth > 1000000) c->maxHealth = 100;
    if (c->health < 0) c->health = 0;
    if (c->health > c->maxHealth) c->health = c->maxHealth;
    if (c->damage < -1000000 || c->damage > 1000000) c->damage = 10;
    if (c->level < 0 || c->level > 100) c->level = 0;
    if (c->xp < 0 || c->xp > 100000000) c->xp = 0;
}
static void sanitizeAllChampions(Game *game) {
    if (!game) return;
    for (int i = 0; i < 3; i++) sanitizeChampion(&game->champion[i]);
}
typedef enum { LST_SINGLE_ENEMY, LST_AOE_ENEMY, LST_SINGLE_ALLY, LST_AOE_ALLY } LocalTarget;
typedef enum { LTE_DAMAGE, LTE_DEBUFF, LTE_HEAL, LTE_BUFF } LocalType;
typedef struct {
    char name[40];
    enum LocalTarget target;
    enum LocalType type;
    int value;
    int cooldown;
    int hits;
} LocalSkill;
#define MAX_SKILLS 6
static LocalSkill wizardSkills[] = {
    {"Electric Discharge", SINGLE_ENEMY, DAMAGE, 20, 2, 1},
    {"Fireball", AOE_ENEMY, DAMAGE, 12, 3, -1},
    {"Restoration", SINGLE_ALLY, HEAL, 25, 3, 1},
    {"Healing Ritual", AOE_ALLY, HEAL, 15, 4, -1}
};
static int wizardSkillCount = sizeof(wizardSkills) / sizeof(wizardSkills[0]);
static LocalSkill knightSkills[] = {
    {"Crippling Blow", AOE_ENEMY, DAMAGE, 12, 2, -1},
    {"Enrage", SINGLE_ALLY, BUFF, 10, 4, 1},
    {"Whirlwind", AOE_ENEMY, DAMAGE, 10, 2, -1}
};
static int knightSkillCount = sizeof(knightSkills) / sizeof(knightSkills[0]);
static LocalSkill paladinSkills[] = {
    {"Firebrand", LST_AOE_ALLY, LTE_BUFF, 3, 3, -1},
    {"Healing Tears", LST_AOE_ALLY, LTE_BUFF, 3, 3, -1},
    {"Bouncing Shield", LST_AOE_ENEMY, LTE_DAMAGE, 14, 3, 2} /* hits 2 enemies */
};
static LocalSkill rogueSkills[] = {
    {"Backlash", SINGLE_ENEMY, DAMAGE, 18, 1, 1},
    {"Throwing Knife", AOE_ENEMY, DAMAGE, 8, 2, -1},
    {"Corrupted Blade", SINGLE_ENEMY, DEBUFF, 4, 3, 1}
};
static int rogueSkillCount = sizeof(rogueSkills) / sizeof(rogueSkills[0]);
static LocalSkill elfSkills[] = {
    {"First Aid", SINGLE_ALLY, HEAL, 20, 2, 1},
    {"Ricochet", AOE_ENEMY, DAMAGE, 9, 2, -1},
    {"Marksman's Fang", SINGLE_ENEMY, DAMAGE, 22, 3, 1}
};

static int wizardSkillCount = sizeof(wizardSkills)/sizeof(wizardSkills[0]);
static int knightSkillCount = sizeof(knightSkills)/sizeof(knightSkills[0]);
static int paladinSkillCount = sizeof(paladinSkills)/sizeof(paladinSkills[0]);
static int rogueSkillCount = sizeof(rogueSkills)/sizeof(rogueSkills[0]);
static int elfSkillCount = sizeof(elfSkills)/sizeof(elfSkills[0]);
static const char *local_effect_target_string[] = {
    "SINGLE_ENEMY",
    "AOE_ENEMY",
    "SINGLE_ALLY",
    "AOE_ALLY"
};
static const char *local_effect_type_string[] = {
    "DAMAGE",
    "DEBUFF",
    "HEAL",
    "BUFF"
};
static int localCooldowns[3][MAX_SKILLS] = {{0}};
static int championIndexOf(Game *game, Champion *c) {
    if (!game || !c) return -1;
    for (int i = 0; i < 3; i++) if (&game->champion[i] == c) return i;
    return -1;
}
void decrementLocalCooldowns(void) {
    for (int ci = 0; ci < 3; ci++) {
        for (int si = 0; si < 4; si++) {
            if (localCooldowns[ci][si] > 0) localCooldowns[ci][si]--;
        }
    }
}
void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount) {
    printf("\n=== Skills ===\n");
    int ci = championIndexOf(game, c);
    if (ci < 0) ci = 0;
    LocalSkill *skillList = NULL;
    int skillCount = 0;
    switch (c->class) {
        case WIZARD: skillList = wizardSkills; skillCount = wizardSkillCount; break;
        case KNIGHT: skillList = knightSkills; skillCount = knightSkillCount; break;
        case PALADIN: skillList = paladinSkills; skillCount = paladinSkillCount; break;
        case ROGUE: skillList = rogueSkills; skillCount = rogueSkillCount; break;
        case ELF: skillList = elfSkills; skillCount = elfSkillCount; break;
        default: skillList = wizardSkills; skillCount = wizardSkillCount; break;
    }
    for (int s = 0; s < skillCount; s++) {
        LocalSkill *ls = &skillList[s];
        printf("[%d] %s - %s (%s) Value:%d CD:%d%s\n",
               s+1, ls->name, target_string[ls->target],
               target_type[ls->type], ls->value, localCooldowns[ci][s],
               localCooldowns[ci][s] > 0 ? " *ON COOLDOWN*" : "");
    }
    printf("[0] Cancel\nSelect skill: ");
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    if (choice == 0) return;
    if (choice < 1 || choice > skillCount) {
        printf("Invalid skill\n");
        return;
    }
    int sidx = choice - 1;
    if (localCooldowns[ci][sidx] > 0) {
        printf("Skill on cooldown (%d)\n", localCooldowns[ci][sidx]);
        return;
    }
    LocalSkill *ls = &skillList[sidx];
    localCooldowns[ci][sidx] = ls->cooldown;
    switch (ls->target) {
        case SINGLE_ENEMY: {
            int target = selectTarget(enemies, enemyCount);
            if (target < 0) return;
            if (ls->type == LTE_DAMAGE) {
                enemies[target].health -= ls->value;
                if (enemies[target].health < 0) enemies[target].health = 0;
                printf("%s used %s on %s (%d dmg)\n", champion_string[c->class], ls->name, enemies[target].name, ls->value);
            } else if (ls->type == DEBUFF) {
                enemies[target].damage -= ls->value;
                if (enemies[target].damage < 0) enemies[target].damage = 0;
                printf("%s used %s on %s (-%d dmg)\n", champion_string[c->class], ls->name, enemies[target].name, ls->value);
            }
            break;
        }
        case AOE_ENEMY: {
            if (ls->type == DAMAGE) {
                if (ls->hits == -1) {
                    for (int i = 0; i < enemyCount; i++) {
                        if (enemies[i].health > 0) {
                            enemies[i].health -= ls->value;
                            if (enemies[i].health < 0) enemies[i].health = 0;
                        }
                    }
                    printf("%s used %s on all enemies (%d each)\n", champion_string[c->class], ls->name, ls->value);
                } else {
                    int applied = 0;
                    for (int i = 0; i < enemyCount && applied < ls->hits; i++) {
                        if (enemies[i].health > 0) {
                            enemies[i].health -= ls->value;
                            if (enemies[i].health < 0) enemies[i].health = 0;
                            applied++;
                        }
                    }
                    printf("%s used %s and hit %d enemy(ies) (%d each)\n", champion_string[c->class], ls->name, applied, ls->value);
                }
            } else if (ls->type == LTE_DEBUFF) {
                for (int i=0;i<enemyCount;i++) if (enemies[i].health>0) { enemies[i].damage -= ls->value; if (enemies[i].damage<0) enemies[i].damage=0; }
                printf("%s used %s on all enemies (-%d dmg)\n", champion_string[c->class], ls->name, ls->value);
            }
            break;
        }
        case SINGLE_ALLY: {
            int target = selectAlly(game);
            if (target < 0) return;
            if (ls->type == LTE_HEAL) {
                game->champion[target].health += ls->value;
                if (game->champion[target].health > game->champion[target].maxHealth)
                    game->champion[target].health = game->champion[target].maxHealth;
                printf("%s used %s on Champion %d (+%d HP)\n", champion_string[c->class], ls->name, target+1, ls->value);
            } else if (ls->type == BUFF) {
                game->champion[target].damage += ls->value;
                printf("%s used %s on Champion %d (+%d dmg)\n", champion_string[c->class], ls->name, target+1, ls->value);
            }
            break;
        }
        case LST_AOE_ALLY: {
            if (ls->type == LTE_HEAL) {
                for (int i=0;i<3;i++) if (game->champion[i].health>0) { game->champion[i].health += ls->value; if (game->champion[i].health>game->champion[i].maxHealth) game->champion[i].health=game->champion[i].maxHealth; }
                printf("%s used %s on all allies (+%d HP)\n", champion_string[c->class], ls->name, ls->value);
            } else if (ls->type == LTE_BUFF) {
                for (int i=0;i<3;i++) if (game->champion[i].health>0) { game->champion[i].damage += ls->value; }
                printf("%s used %s on all allies (+%d dmg)\n", champion_string[c->class], ls->name, ls->value);
            }
            break;
        }
    }
}
int selectAlly(Game *game) {
    printf("\n--- Select Ally ---\n");
    int aliveMap[3];
    int counter = 0;
    int i;
    for (i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) {
            aliveMap[i] = 1;
            printf("[%d] Champion %d - %s (HP: %d/%d)\n",
                   ++counter, i + 1, champion_string[game->champion[i].class],
                   game->champion[i].health, game->champion[i].maxHealth);
        } else {
            aliveMap[i] = 0;
        }
    }
    printf("[0] Cancel\nSelect: ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        return -1;
    }
    if (choice == 0) return -1;
    int target = findBinaryMapping(aliveMap, choice, 3);
    if (target == -1) {
        printf("Invalid target!\n");
        return -1;
    }
    return target;
}
void initCombat(Game *game) {
    simulateCombat(game);
}
int selectTarget(Monster enemies[], int enemyCount) {
    while (1) {
        printf("\n--- Select Target ---\n");
        int aliveMap[enemyCount];
        int counter = 0;
        int i;
        for (i = 0; i < enemyCount; i++) {
            if (enemies[i].health > 0){
                aliveMap[i] = 1;
                printf("[%d] %s (HP: %d/%d)\n", 
                       ++counter, enemies[i].name, 
                       enemies[i].health, enemies[i].maxHealth);
            } else {
                aliveMap[i] = 0;
            }
        }
        printf("[0] Cancel\nSelect: ");
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input.\n");
            continue;
        }
        
        if (choice == 0) return -1;
        
        if (choice < 0 || choice > counter) {
            printf("Invalid target!\n");
            continue;
        }
        
        int target = findBinaryMapping(aliveMap, choice - 1, enemyCount);
        return target;
    }
}
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount) {
    while (1) {
        printf("\n=== VIEW STATS ===\n[1] View All Champions\n[2] View Monster\n[0] Back\nChoice: ");
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
                Champion c = game->champion[i];
                printf("Champion %d - %s: HP %d/%d | Damage: %d%s\n",
                       i + 1, champion_string[c.class],
                       c.health, c.maxHealth, c.damage,
                       c.health <= 0 ? " [DEFEATED]" : "");
            }
        } else if (choice == 2) {
            printf("\nSelect Monster:\n");
            int aliveMap[enemyCount];
            int counter = 0; 
            int i;
            for (i = 0; i < enemyCount; i++) {
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
    printf("\n=== COMBAT STATUS ===\nChampions:\n");
    int i;
    for (i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) {
            printf("  [%d] %s: HP %d/%d\n", i + 1, 
                   champion_string[game->champion[i].class],
                   game->champion[i].health, game->champion[i].maxHealth);
        } else {
            printf("  [%d] DEFEATED\n", i + 1);
        }
    }
    printf("Monsters:\n");
    int j;
    for (j = 0; j < enemyCount; j++) {
        if (enemies[j].health > 0) {
            printf("  %s: %d/%d HP\n", enemies[j].name, 
                   enemies[j].health, enemies[j].maxHealth);
        } else {
            printf("  %s: DEFEATED\n", enemies[j].name);
        }
    }
    printf("=====================\n");
}
void createCombat(Game *game) {
    initCombat(game);
}