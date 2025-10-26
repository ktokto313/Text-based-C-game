#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game_object.h"
#include "character.h"
#include "inventory.h"
#include <cJSON.h>
#define CRIT_CHANCE 0.15 //add crit chance constant
#define CRIT_DAMAGE 1.5 // Critical damage multiplier
#define DROP_CHANCE 0.3  // 30% chance per monster to drop loot
static int champBuffValue[3] = {0};
static int champBuffTurns[3] = {0};
static int enemyDebuffValue[3] = {0};
static int enemyDebuffTurns[3] = {0};
static double champCritBonus[3] = {0.0};

// Simple loot table - can be expanded later
static void generateLoot(LinkedList *drops, int monsterCount) {
    // Loot pool
    const char *lootNames[] = {"Health Potion", "Small Health Potion", "Large Health Potion"};
    const int lootHeals[] = {15, 10, 25};
    const int lootCount = 3;
    
    for (int i = 0; i < monsterCount; i++) {
        double roll = (double)rand() / (double)RAND_MAX;
        if (roll < DROP_CHANCE) {
            // Random item from loot pool
            int lootIdx = rand() % lootCount;
            Item *drop = (Item *)malloc(sizeof(Item));
            if (drop) {
                drop->type = HEALTH_POTION;
                strcpy(drop->name, lootNames[lootIdx]);
                drop->value = lootHeals[lootIdx] * 5; // Gold value
                drop->effectValue = lootHeals[lootIdx];
                insert(drops, drop);
            }
        }
    }
}

static int loadSaveIntoGame(Game *game, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        printf("Failed to open save file: %s\n", path);
        return 0;
    }
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return 0; }
    rewind(f);
    char *buf = (char *)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return 0; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[rd] = '\0';
    cJSON *root = cJSON_Parse(buf);
    free(buf);
    if (!root) {
        printf("Failed to parse save file JSON: %s\n", path);
        return 0;
    }
    cJSON *level = cJSON_GetObjectItem(root, "level");
    cJSON *day = cJSON_GetObjectItem(root, "day");
    cJSON *tod = cJSON_GetObjectItem(root, "timeOfTheDay");
    cJSON *gold = cJSON_GetObjectItem(root, "gold");
    if (!cJSON_IsNumber(level) || !cJSON_IsNumber(day) || !cJSON_IsNumber(tod) || !cJSON_IsNumber(gold)) {
        cJSON_Delete(root);
        printf("Invalid save file schema: %s\n", path);
        return 0;
    }
    game->level = level->valueint;
    game->day = day->valueint;
    game->timeOfTheDay = tod->valueint;
    game->gold = gold->valueint;
    cJSON *itemList = cJSON_GetObjectItem(root, "itemList");//restore items saved
    if (itemList && cJSON_IsArray(itemList)) {
        init(&(game->itemList));
        int n = cJSON_GetArraySize(itemList);
        for (int i = 0; i < n; i++) {
            cJSON *it = cJSON_GetArrayItem(itemList, i);
            if (!cJSON_IsObject(it)) continue;
            Item *item = (Item *)malloc(sizeof(Item));
            if (!item) continue;
            cJSON *t = cJSON_GetObjectItem(it, "type");
            cJSON *nm = cJSON_GetObjectItem(it, "name");
            cJSON *val = cJSON_GetObjectItem(it, "value");
            cJSON *eff = cJSON_GetObjectItem(it, "effectValue");
            item->type = t && cJSON_IsNumber(t) ? t->valueint : 0;
            strcpy(item->name, nm && cJSON_IsString(nm) ? nm->valuestring : "");
            item->value = val && cJSON_IsNumber(val) ? val->valueint : 0;
            item->effectValue = eff && cJSON_IsNumber(eff) ? eff->valueint : 0;
            insert(&(game->itemList), item);
        }
    }
    cJSON *champions = cJSON_GetObjectItem(root, "champions");//for champs
    if (champions && cJSON_IsArray(champions)) {
        int n = cJSON_GetArraySize(champions);
        for (int i = 0; i < 3 && i < n; i++) {
            cJSON *ch = cJSON_GetArrayItem(champions, i);
            if (!cJSON_IsObject(ch)) continue;
            cJSON *h = cJSON_GetObjectItem(ch, "health");
            cJSON *mh = cJSON_GetObjectItem(ch, "maxHealth");
            cJSON *d = cJSON_GetObjectItem(ch, "damage");
            cJSON *cl = cJSON_GetObjectItem(ch, "class");
            cJSON *lv = cJSON_GetObjectItem(ch, "level");
            cJSON *xp = cJSON_GetObjectItem(ch, "xp");
            game->champion[i].health = h && cJSON_IsNumber(h) ? h->valueint : 0;
            game->champion[i].maxHealth = mh && cJSON_IsNumber(mh) ? mh->valueint : 0;
            game->champion[i].damage = d && cJSON_IsNumber(d) ? d->valueint : 0;
            game->champion[i].class = cl && cJSON_IsNumber(cl) ? cl->valueint : 0;
            game->champion[i].level = lv && cJSON_IsNumber(lv) ? lv->valueint : 0;
            game->champion[i].xp = xp && cJSON_IsNumber(xp) ? xp->valueint : 0;
        }
    }
    cJSON_Delete(root);
    return 1;
}
void printCombatStatus(Game *game, Monster enemies[], int enemyCount);
int selectTarget(Monster enemies[], int enemyCount);
int selectAlly(Game *game);
void showMonsterStats(Monster *m, int index);
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount);
void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount);
void initCombat(Game *game);
void useItem(Game *game) {
    int total = game->itemList.size;
    if (total <= 0) {
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║ %-44s ║\n", "Use Item");
        printf("╠══════════════════════════════════════════════╣\n");
        printf("║ %-44s ║\n", "No items in inventory.");
        printf("╚══════════════════════════════════════════════╝\n");
        return;
    }
    int count = 0;
    Node *n = game->itemList.head;
    int idx = 0;
    while (n) {
        Item *it = (Item *)n->value;
        if (it && it->type == HEALTH_POTION) count++;
        n = n->next; idx++;
    }
    if (count == 0) {
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║ %-44s ║\n", "Use Item");
        printf("╠══════════════════════════════════════════════╣\n");
        printf("║ %-44s ║\n", "No usable items (potions) available.");
        printf("╚══════════════════════════════════════════════╝\n");
        return;
    }
    int map[count]; 
    n = game->itemList.head;
    idx = 0; int disp = 0;
    while (n) {
        Item *it = (Item *)n->value;
        if (it && it->type == HEALTH_POTION) {
            map[disp++] = idx;
        }
        n = n->next; idx++;
    }
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║ %-44s ║\n", "Use Item");
    printf("╠══════════════════════════════════════════════╣\n");
    for (int i = 0; i < count; i++) {
        Node *node = getElementAt(game->itemList, map[i]);
        if (!node) continue;
        Item *it = (Item *)node->value;
        char line[128];
        snprintf(line, sizeof(line), "[%d] %s (Heals %d HP)", i + 1, it->name, it->effectValue);
        printf("║ %-44s ║\n", line);
    }
    printf("║ %-44s ║\n", "[0] Cancel");
    printf("╚══════════════════════════════════════════════╝\n");
    printf("Select item: ");
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    if (choice == 0) return;
    if (choice < 1 || choice > count) { printf("Invalid choice.\n"); return; }
    int listIndex = map[choice - 1];
    Node *selNode = getElementAt(game->itemList, listIndex);
    if (!selNode) { printf("Item not found.\n"); return; }
    Item *sel = (Item *)selNode->value;
    printf("\nSelect champion to use on:\n");
    int aliveMap[3]; int counter = 0;
    for (int i = 0; i < 3; i++) {
        if (game->champion[i].maxHealth > 0) {
            aliveMap[i] = 1;
            printf("[%d] Champion %d - %s (HP: %d/%d)\n", ++counter, i + 1,
                   champion_string[game->champion[i].class],
                   game->champion[i].health, game->champion[i].maxHealth);
        } else aliveMap[i] = 0;
    }
    printf("[0] Cancel\nChoice: ");
    int targetChoice;
    if (scanf("%d", &targetChoice) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    if (targetChoice == 0) return;
    int target = findBinaryMapping(aliveMap, targetChoice, 3);
    if (target == -1) { printf("Invalid target.\n"); return; }
    game->champion[target].health += sel->effectValue;
    if (game->champion[target].health > game->champion[target].maxHealth)
        game->champion[target].health = game->champion[target].maxHealth;
    printf("Used %s on Champion %d (+%d HP).\n", sel->name, target + 1, sel->effectValue);
    Node *rem = removeAt(&(game->itemList), listIndex);//dungf items xong thi remove
    if (rem) { free(rem->value); free(rem); }
}
typedef enum { LST_SINGLE_ENEMY, LST_AOE_ENEMY, LST_SINGLE_ALLY, LST_AOE_ALLY } LocalTarget;
typedef enum { LTE_DAMAGE, LTE_DEBUFF, LTE_HEAL, LTE_BUFF } LocalType;
typedef struct {
    char name[40];
    LocalTarget target;
    LocalType type;
    int value;
    int cooldown;
    int hits;
} LocalSkill;
static const char *local_effect_target_string[] = {
    "SINGLE_ENEMY", "AOE_ENEMY", "SINGLE_ALLY", "AOE_ALLY"
};
static const char *local_effect_type_string[] = {
    "DAMAGE", "DEBUFF", "HEAL", "BUFF"
};
static int localCooldowns[3][5] = {{0}};
static LocalSkill wizardSkills[] = {
    {"Electric Discharge", LST_SINGLE_ENEMY, LTE_DAMAGE, 20, 2, 1},
    {"Fireball", LST_AOE_ENEMY, LTE_DAMAGE, 12, 3, -1},
    {"Restoration", LST_SINGLE_ALLY, LTE_HEAL, 25, 3, 1},
    {"Healing Ritual", LST_AOE_ALLY, LTE_HEAL, 15, 4, -1}
};
static int wizardSkillCount = sizeof(wizardSkills) / sizeof(wizardSkills[0]);
static LocalSkill knightSkills[] = {
    {"Crippling Blow", LST_AOE_ENEMY, LTE_DAMAGE, 12, 2, -1},
    {"Enrage", LST_SINGLE_ALLY, LTE_BUFF, 10, 4, 1},
    {"Whirlwind", LST_AOE_ENEMY, LTE_DAMAGE, 10, 2, -1}
};
static int knightSkillCount = sizeof(knightSkills) / sizeof(knightSkills[0]);
static LocalSkill paladinSkills[] = {
    {"Firebrand", LST_AOE_ALLY, LTE_BUFF, 5, 3, -1},
    {"Healing Tears", LST_AOE_ALLY, LTE_HEAL, 10, 3, -1},
    {"Bouncing Shield", LST_AOE_ENEMY, LTE_DAMAGE, 14, 3, 2}
};
static int paladinSkillCount = sizeof(paladinSkills) / sizeof(paladinSkills[0]);

static LocalSkill rogueSkills[] = {
    {"Backlash", LST_SINGLE_ENEMY, LTE_DAMAGE, 18, 1, 1},
    {"Throwing Knife", LST_AOE_ENEMY, LTE_DAMAGE, 8, 2, -1},
    {"Corrupted Blade", LST_SINGLE_ENEMY, LTE_DEBUFF, 4, 3, 1}
};
static int rogueSkillCount = sizeof(rogueSkills) / sizeof(rogueSkills[0]);
static LocalSkill elfSkills[] = {
    {"First Aid", LST_SINGLE_ALLY, LTE_HEAL, 20, 2, 1},
    {"Ricochet", LST_AOE_ENEMY, LTE_DAMAGE, 9, 2, -1},
    {"Marksman's Fang", LST_SINGLE_ENEMY, LTE_DAMAGE, 22, 3, 1}
};
static int elfSkillCount = sizeof(elfSkills) / sizeof(elfSkills[0]);
static int championIndexOf(Game *game, Champion *c) {
    if (!game || !c) return -1;
    for (int i = 0; i < 3; i++) if (&game->champion[i] == c) return i;
    return -1;
}
static void decrementChampionCooldown(int championIndex) {
    for (int si = 0; si < 5; si++) {
        if (localCooldowns[championIndex][si] > 0) localCooldowns[championIndex][si]--;
    }
}
void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount) {
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║ %-44s ║\n", "*** Skills ***");
    printf("╚══════════════════════════════════════════════╝\n");
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
               s+1, ls->name, local_effect_target_string[ls->target],
               local_effect_type_string[ls->type], ls->value, localCooldowns[ci][s],
               localCooldowns[ci][s] > 0 ? " *ON COOLDOWN*" : "");
        if (s < skillCount - 1) {
            printf("═════════════════════════════════════════════\n");
        }
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
        case LST_SINGLE_ENEMY: {
            int target = selectTarget(enemies, enemyCount);
            if (target < 0) return;

            if (ls->type == LTE_DAMAGE) {
                double effCrit = CRIT_CHANCE + champCritBonus[ci];
                if (effCrit > 0.95) effCrit = 0.95;
                int dmg = ls->value;
                int crit = ((double)rand() / (double)RAND_MAX) < effCrit;
                if (crit) dmg = (int)(dmg * CRIT_DAMAGE + 0.5);
                enemies[target].health -= dmg;
                if (enemies[target].health < 0) enemies[target].health = 0;
                printf("%s used %s on %s (%d dmg%s)\n", champion_string[c->class], ls->name, enemies[target].name, dmg, crit ? " [CRITICAL]" : "");
            } else if (ls->type == LTE_DEBUFF) {
                enemies[target].damage -= ls->value;
                if (enemies[target].damage < 0) enemies[target].damage = 0;
                printf("%s used %s on %s (-%d dmg)\n", champion_string[c->class], ls->name, enemies[target].name, ls->value);
            }
            break;
        }
        case LST_AOE_ENEMY: {
            if (ls->type == LTE_DAMAGE) {
                double effCrit = CRIT_CHANCE + champCritBonus[ci];
                if (effCrit > 0.95) effCrit = 0.95;
                int actionCrit = ((double)rand() / (double)RAND_MAX) < effCrit;
                if (ls->hits == -1) {
                    int targets = 0;
                    for (int i = 0; i < enemyCount; i++) {
                        if (enemies[i].health > 0) targets++;
                    }
                    for (int i = 0; i < enemyCount; i++) {
                        if (enemies[i].health > 0) {
                            int dmg = ls->value;
                            if (actionCrit) dmg = (int)(dmg * CRIT_DAMAGE + 0.5);
                            enemies[i].health -= dmg;
                            if (enemies[i].health < 0) enemies[i].health = 0;
                        }
                    }
                    printf("%s used %s on all %d enemies (%d each%s)\n", champion_string[c->class], ls->name, targets, ls->value, actionCrit ? " [CRITICAL]" : "");
                } else {
                    int applied = 0;
                    for (int i = 0; i < enemyCount && applied < ls->hits; i++) {
                        if (enemies[i].health > 0) {
                            int dmg = ls->value;
                            if (actionCrit) dmg = (int)(dmg * CRIT_DAMAGE + 0.5);
                            enemies[i].health -= dmg;
                            if (enemies[i].health < 0) enemies[i].health = 0;
                            applied++;
                        }
                    }
                    printf("%s used %s and hit %d enemy(ies) (%d each%s)\n", champion_string[c->class], ls->name, applied, ls->value, actionCrit ? " [CRITICAL]" : "");
                }
            } else if (ls->type == LTE_DEBUFF) {
                if (ls->hits == -1) {
                    int targets = 0;
                    for (int i = 0; i < enemyCount; i++) {
                        if (enemies[i].health > 0) targets++;
                    }
                    for (int i = 0; i < enemyCount; i++) {
                        if (enemies[i].health > 0) {
                            enemyDebuffValue[i] += ls->value;
                            enemyDebuffTurns[i] = 2; // lasts 2 turns
                        }
                    }
                    printf("%s used %s on all %d enemies (-%d dmg for 2 turns)\n", champion_string[c->class], ls->name, targets, ls->value);
                } else {
                    int applied = 0;
                    for (int i = 0; i < enemyCount && applied < ls->hits; i++) {
                        if (enemies[i].health > 0) {
                            enemyDebuffValue[i] += ls->value;
                            enemyDebuffTurns[i] = 2;
                            applied++;
                        }
                    }
                    printf("%s used %s and debuffed %d enemy(ies) (-%d dmg for 2 turns)\n", champion_string[c->class], ls->name, applied, ls->value);
                }
            }
            break;
        }
        case LST_SINGLE_ALLY: {
            int target = selectAlly(game);
            if (target < 0) return;

            if (ls->type == LTE_HEAL) {
                game->champion[target].health += ls->value;
                if (game->champion[target].health > game->champion[target].maxHealth)
                    game->champion[target].health = game->champion[target].maxHealth;
                printf("%s used %s on Champion %d (+%d HP)\n", champion_string[c->class], ls->name, target+1, ls->value);
            } else if (ls->type == LTE_BUFF) {
                champBuffValue[target] += ls->value;
                champBuffTurns[target] = 2; // lasts 2 turns
                champCritBonus[target] = 0.25; // +25% crit chance
                printf("%s used %s on Champion %d (+%d ATK, +25%% CRIT for 2 turns)\n", champion_string[c->class], ls->name, target+1, ls->value);
            }
            break;
        }
        case LST_AOE_ALLY: {
            if (ls->type == LTE_HEAL) {
                if (ls->hits == -1) {
                    int targets = 0;
                    for (int i = 0; i < 3; i++) if (game->champion[i].health > 0) targets++;
                    for (int i = 0; i < 3; i++) {
                        if (game->champion[i].health > 0) {
                            game->champion[i].health += ls->value;
                            if (game->champion[i].health > game->champion[i].maxHealth)
                                game->champion[i].health = game->champion[i].maxHealth;
                        }
                    }
                    printf("%s used %s on all %d allies (+%d HP)\n", champion_string[c->class], ls->name, targets, ls->value);
                } else {
                    int applied = 0;
                    for (int i = 0; i < 3 && applied < ls->hits; i++) {
                        if (game->champion[i].health > 0) {
                            game->champion[i].health += ls->value;
                            if (game->champion[i].health > game->champion[i].maxHealth)
                                game->champion[i].health = game->champion[i].maxHealth;
                            applied++;
                        }
                    }
                    printf("%s used %s on %d ally(ies) (+%d HP)\n", champion_string[c->class], ls->name, applied, ls->value);
                }
            } else if (ls->type == LTE_BUFF) {
                if (ls->hits == -1) {
                    int targets = 0;
                    for (int i = 0; i < 3; i++) if (game->champion[i].health > 0) targets++;
                    for (int i = 0; i < 3; i++) {
                        if (game->champion[i].health > 0) {
                            champBuffValue[i] += ls->value;
                            champBuffTurns[i] = 2;
                            champCritBonus[i] = 0.15;
                        }
                    }
                    printf("%s used %s on all %d allies (+%d ATK, +15%% CRIT for 2 turns)\n", champion_string[c->class], ls->name, targets, ls->value);
                } else {
                    int applied = 0;
                    for (int i = 0; i < 3 && applied < ls->hits; i++) {
                        if (game->champion[i].health > 0) {
                            champBuffValue[i] += ls->value;
                            champBuffTurns[i] = 2;
                            champCritBonus[i] = 0.25;
                            applied++;
                        }
                    }
                    printf("%s used %s on %d ally(ies) (+%d ATK, +25%% CRIT for 2 turns)\n", champion_string[c->class], ls->name, applied, ls->value);
                }
            }
            break;
        }
    }
}
int selectAlly(Game *game) {
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║ %-44s ║\n", "*** Select Ally ***");
    printf("╠══════════════════════════════════════════════╣\n");
    int aliveMap[3];
    int counter = 0;
    int i;
    for (i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) {
            aliveMap[i] = 1;
            char line[128];
            snprintf(line, sizeof(line), "[%d] Champion %d - %s (HP: %d/%d)",
                   ++counter, i + 1, champion_string[game->champion[i].class],
                   game->champion[i].health, game->champion[i].maxHealth);
            printf("║ %-44s ║\n", line);
        } else {
            aliveMap[i] = 0;
        }
    }
    printf("║ %-44s ║\n", "[0] Cancel");
    printf("╚══════════════════════════════════════════════╝\n");
    printf("Select: ");
    
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
static void handleDefeat(Game *game) {
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║ %-44s ║\n", "*** DEFEAT ***");
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║ %-44s ║\n", "All champions defeated!");
    printf("╚══════════════════════════════════════════════╝\n");
    FILE *fp;
    char *saveFiles[20];
    int saveCount = 0;
    fp = popen("ls saves/*.json", "r");
    if (fp) {
        char buf[256];
        while (fgets(buf, sizeof(buf), fp) && saveCount < 20) {
            buf[strcspn(buf, "\n")] = 0;
            saveFiles[saveCount] = malloc(strlen(buf)+1);
            strcpy(saveFiles[saveCount], buf);
            saveCount++;
        }
        pclose(fp);
    }
    int loadChoice = -1;
    do {
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║ %-44s ║\n", "*** Load Save File ***");
        printf("╠══════════════════════════════════════════════╣\n");
        for (int s = 0; s < saveCount; s++) {
            char line[128];
            snprintf(line, sizeof(line), "[%d] Load %s", s+1, saveFiles[s]);
            printf("║ %-44s ║\n", line);
        }
        printf("║ %-44s ║\n", "[0] Exit to menu");
        printf("╚══════════════════════════════════════════════╝\n");
        printf("Enter your choice: ");
        if (scanf("%d", &loadChoice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input, please try again.\n");
            loadChoice = -1;
            continue;
        }
        if (loadChoice == 0) {
            printf("Returning to main menu...\n");
            extern void showMainMenu(Game *game);
            showMainMenu(game);
            break;
        } else if (loadChoice > 0 && loadChoice <= saveCount) {
            printf("Loading %s...\n", saveFiles[loadChoice-1]);
            extern void initGame(Game *game);
            initGame(game);
            if (!loadSaveIntoGame(game, saveFiles[loadChoice-1])) {
                printf("Failed to load selected save. Returning to main menu...\n");
            }
            extern void showMainMenu(Game *game);
            showMainMenu(game);
            break;
        } else {
            printf("Invalid input, please try again.\n");
            loadChoice = -1;
        }
    } while (loadChoice == -1);
    for (int s = 0; s < saveCount; s++) free(saveFiles[s]);
}

void initCombat(Game *game) {
    printf("\n=== COMBAT START ===\n");
    
    LinkedList *monsterList = &((LocationData*)game->locationData)[game->level].monsterList;
    int totalMonsters = monsterList->size;
    
    if (totalMonsters == 0) {
        printf("No monsters to fight!\n");
        return;
    }
<<<<<<< Updated upstream
    
    int fightCount = 3;
=======
    int fightCount;
    double roll = (double)rand() / (double)RAND_MAX;
    if (roll < 0.15) {
        fightCount = 1;
    } else if (roll < 0.85) {
        fightCount = 2;
    } else {
        fightCount = 3;//ve co ban thi kha nang swarm wave with 3 va 1 enemies it hon wave 2
    }
>>>>>>> Stashed changes
    if (fightCount > totalMonsters) fightCount = totalMonsters;
    
    Monster localEnemies[fightCount];
    int enemyCount = 0;
<<<<<<< Updated upstream
    int selected[totalMonsters];
    int i;
    for (i = 0; i < totalMonsters; i++) selected[i] = 0;
    
    srand(time(NULL));
    
    while (enemyCount < fightCount) {
        int randIndex = rand() % totalMonsters;
        
        if (selected[randIndex] == 0) {
            selected[randIndex] = 1;
            
            Node *current = monsterList->head;
            int k;
            for (k = 0; k < randIndex; k++) {
                current = current->next;
            }
            
            Monster *m = (Monster *)current->value;
            localEnemies[enemyCount].health = m->health;
            localEnemies[enemyCount].maxHealth = m->maxHealth;
            localEnemies[enemyCount].damage = m->damage;
            strcpy(localEnemies[enemyCount].name, m->name);
            enemyCount++;
        }
=======
    srand(time(NULL));
    for (int i = 0; i < 3; i++) { 
        champBuffValue[i] = 0; champBuffTurns[i] = 0; 
        champCritBonus[i] = 0.0;
    }
    for (int i = 0; i < 3; i++) { enemyDebuffValue[i] = 0; enemyDebuffTurns[i] = 0; }
    while (enemyCount < fightCount && totalMonsters > 0) {
        int randIndex = rand() % totalMonsters;
        Node *current = monsterList->head;
        for (int k = 0; k < randIndex; k++) current = current->next;
        Monster *m = (Monster *)current->value;
        localEnemies[enemyCount].health = m->health;
        localEnemies[enemyCount].maxHealth = m->maxHealth;
        localEnemies[enemyCount].damage = m->damage;
        strcpy(localEnemies[enemyCount].name, m->name);
        enemyCount++;
>>>>>>> Stashed changes
    }
    
    printf("%d monsters appear!\n", enemyCount);
    
    int championIndex = 0;
    int monsterIndex = 0;
    int downedMonster = 0;
    int totalExp = 0;
<<<<<<< Updated upstream
    
    while (1) {
        if (championIndex >= 3) championIndex = 0;
        
=======

    while (1) {
        if (championIndex >= 3) championIndex = 0;
        int anyAlive = 0;
        for (int ci = 0; ci < 3; ci++) if (game->champion[ci].health > 0) anyAlive = 1;
        if (!anyAlive) { handleDefeat(game); return; }
>>>>>>> Stashed changes
        while (game->champion[championIndex].health <= 0) {
            championIndex++;
            if (championIndex >= 3) championIndex = 0;
        }
<<<<<<< Updated upstream
        
        printf("\n=== Champion %d (%s)'s Turn (HP: %d/%d) ===\n", 
               championIndex + 1, champion_string[game->champion[championIndex].class],
               game->champion[championIndex].health, game->champion[championIndex].maxHealth);
        printf("[1] Attack\n[2] Use Skill\n[3] Use Item\n[4] View Stats\nChoose action: ");
        
        int choice;
        if (scanf("%d", &choice) != 1) {
=======

     char title[64];
     snprintf(title, sizeof(title), "*** Champion's Turn (%s) ***",
           champion_string[game->champion[championIndex].class]);
     printf("\n╔══════════════════════════════════════════════╗\n");
     printf("║ %-44s ║\n", title);
     printf("╠══════════════════════════════════════════════╣\n");
     char hpline[64];
     snprintf(hpline, sizeof(hpline), "HP: %d/%d   Choose action:",
           game->champion[championIndex].health,
           game->champion[championIndex].maxHealth);
     printf("║ %-44s ║\n", hpline);
     printf("╠══════════════════════════════════════════════╣\n");
     printf("║ %-44s ║\n", "[1] Attack");
     printf("║ %-44s ║\n", "[2] Use Skill");
    printf("║ %-44s ║\n", "[3] Use Item");
     printf("║ %-44s ║\n", "[4] View Stats");
     printf("╚══════════════════════════════════════════════╝\n");
     printf("Your choice: ");
        int action;
        if (scanf("%d", &action) != 1) {
>>>>>>> Stashed changes
            while (getchar() != '\n');
            printf("Invalid input.\n");
            continue;
        }
<<<<<<< Updated upstream
        
        switch (choice) {
            case 1: {
                int target = selectTarget(localEnemies, enemyCount);
                if (target < 0) continue;
                
                localEnemies[target].health -= game->champion[championIndex].damage;
=======
        switch (action) {
            case 1: {
          int target = selectTarget(localEnemies, enemyCount);
          if (target < 0) break;
          int base = game->champion[championIndex].damage + champBuffValue[championIndex];
          if (base < 0) base = 0;
          int dealt = base;
          double effCrit = CRIT_CHANCE + champCritBonus[championIndex];
          if (effCrit > 0.95) effCrit = 0.95;
          double r = (double)rand() / (double)RAND_MAX;
          int crit = r < effCrit;
          if (crit) dealt = (int)(dealt * CRIT_DAMAGE + 0.5);
          localEnemies[target].health -= dealt;
>>>>>>> Stashed changes
                if (localEnemies[target].health <= 0) {
                    localEnemies[target].health = 0;
                    downedMonster++;
                    totalExp += 50;
                }
<<<<<<< Updated upstream
                
                printf("Champion %d attacks %s for %d damage! (HP: %d/%d)\n",
                       championIndex + 1, localEnemies[target].name, 
                       game->champion[championIndex].damage, localEnemies[target].health, 
                       localEnemies[target].maxHealth);
                
=======
          printf("Champion %d attacks %s for %d damage%s! (HP: %d/%d)\n",
              championIndex + 1, localEnemies[target].name,
              dealt, (crit ? " [CRITICAL]" : ""),
              localEnemies[target].health,
              localEnemies[target].maxHealth);
>>>>>>> Stashed changes
                if (downedMonster >= enemyCount) {
                    printf("\n=== VICTORY ===\n");
                    printf("All %d monsters defeated!\n", enemyCount);
                    printf("Experience gained: %d\n", totalExp);
                    addXp(game, totalExp);
                    
                    // Generate and handle loot drops
                    LinkedList lootDrops;
                    init(&lootDrops);
                    generateLoot(&lootDrops, enemyCount);
                    if (lootDrops.size > 0) {
                        printf("\n--- Loot Obtained ---\n");
                        handleMonsterDrops(game, &lootDrops, 1);
                        freeList(&lootDrops);
                    } else {
                        printf("\nNo items dropped.\n");
                    }
                    return;
                }
                
                championIndex++;
                decrementChampionCooldown(championIndex - 1 < 0 ? 2 : championIndex - 1);
                break;
            }
            case 2: {

                useSkill(&game->champion[championIndex], game, localEnemies, enemyCount);
                
                downedMonster = 0;
<<<<<<< Updated upstream
                int k;
                for (k = 0; k < enemyCount; k++) {
                    if (localEnemies[k].health <= 0) downedMonster++;
                }
                
=======
                for (int k = 0; k < enemyCount; k++) if (localEnemies[k].health <= 0) downedMonster++;
>>>>>>> Stashed changes
                if (downedMonster >= enemyCount) {
                    printf("\n=== VICTORY ===\n");
                    printf("All %d monsters defeated!\n", enemyCount);
                    printf("Experience gained: %d\n", totalExp);
                    addXp(game, totalExp);
                    
                    // Generate and handle loot drops
                    LinkedList lootDrops;
                    init(&lootDrops);
                    generateLoot(&lootDrops, enemyCount);
                    if (lootDrops.size > 0) {
                        printf("\n--- Loot Obtained ---\n");
                        handleMonsterDrops(game, &lootDrops, 1);
                        freeList(&lootDrops);
                    } else {
                        printf("\nNo items dropped.\n");
                    }
                    return;
                }
                
                championIndex++;
                decrementChampionCooldown(championIndex - 1 < 0 ? 2 : championIndex - 1);
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
<<<<<<< Updated upstream
        
        while (monsterIndex < enemyCount && localEnemies[monsterIndex].health <= 0) {
            monsterIndex++;
        }
        
        // Reset monster index if it's out of bounds
        if (monsterIndex >= enemyCount || localEnemies[monsterIndex].health <= 0) {
            monsterIndex = 0;
            while (monsterIndex < enemyCount && localEnemies[monsterIndex].health <= 0) {
                monsterIndex++;
            }
        }
        
        if (monsterIndex >= enemyCount) continue;
        int aliveChampions[3];
        int aliveCount = 0;
        for (i = 0; i < 3; i++) {
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
        if (game->champion[targetIdx].health <= 0) {
            game->champion[targetIdx].health = 0;
            woundedChampions++;
        }
        
        printf("\n%s attacks Champion %d for %d damage! (HP: %d/%d)\n",
               localEnemies[monsterIndex].name, targetIdx + 1, 
               localEnemies[monsterIndex].damage,
               game->champion[targetIdx].health, 
               game->champion[targetIdx].maxHealth);
        
        if (woundedChampions >= 3) {
            printf("\n=== DEFEAT ===\n");
            printf("All champions defeated!\n");
            game->initialized = 0;
            return;
        }
        
=======
        int aliveChampionsCheck = 0;
        for (int ci = 0; ci < 3; ci++) if (game->champion[ci].health > 0) aliveChampionsCheck++;
        if (aliveChampionsCheck == 0) { handleDefeat(game); return; }
        while (monsterIndex < enemyCount && localEnemies[monsterIndex].health <= 0) monsterIndex++;
        if (monsterIndex >= enemyCount) monsterIndex = 0;
        int aliveIdx[3];
        int aliveCnt = 0;
        for (int ci = 0; ci < 3; ci++) if (game->champion[ci].health > 0) aliveIdx[aliveCnt++] = ci;
        if (aliveCnt == 0) { handleDefeat(game); return; }
     int targetIdx = aliveIdx[rand() % aliveCnt];
     int mdmg = localEnemies[monsterIndex].damage - enemyDebuffValue[monsterIndex];
     if (mdmg < 0) mdmg = 0;
     int mdeal = mdmg;
     double mr = (double)rand() / (double)RAND_MAX;
     int mcrit = mr < CRIT_CHANCE;
     if (mcrit) mdeal = (int)(mdeal * CRIT_DAMAGE + 0.5);
     game->champion[targetIdx].health -= mdeal;
        if (game->champion[targetIdx].health < 0) game->champion[targetIdx].health = 0;
     printf("\n%s attacks Champion %d for %d damage%s! (HP: %d/%d)\n",
         localEnemies[monsterIndex].name, targetIdx + 1,
         mdeal, (mcrit ? " [CRITICAL]" : ""),
               game->champion[targetIdx].health,
               game->champion[targetIdx].maxHealth);
        aliveChampionsCheck = 0;
        for (int ci = 0; ci < 3; ci++) if (game->champion[ci].health > 0) aliveChampionsCheck++;
        if (aliveChampionsCheck == 0) { handleDefeat(game); return; }

>>>>>>> Stashed changes
        monsterIndex++;
        if (monsterIndex >= enemyCount) monsterIndex = 0;
        
        printCombatStatus(game, localEnemies, enemyCount);
        for (int i = 0; i < 3; i++) {
            if (champBuffTurns[i] > 0) {
                champBuffTurns[i]--;
                if (champBuffTurns[i] == 0) {
                    champBuffValue[i] = 0;
                    champCritBonus[i] = 0.0;
                }
            }
        }
        for (int j = 0; j < enemyCount; j++) {
            if (enemyDebuffTurns[j] > 0) {
                enemyDebuffTurns[j]--;
                if (enemyDebuffTurns[j] == 0) enemyDebuffValue[j] = 0;
            }
        }
    }
}
int selectTarget(Monster enemies[], int enemyCount) {
    while (1) {
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║ %-44s ║\n", "*** Select Target ***");
        printf("╠══════════════════════════════════════════════╣\n");
        int aliveMap[enemyCount];
        int counter = 0;
        int i;
        for (i = 0; i < enemyCount; i++) {
            if (enemies[i].health > 0){
                aliveMap[i] = 1;
                char line[128];
                snprintf(line, sizeof(line), "[%d] %s (HP: %d/%d)", 
                       ++counter, enemies[i].name, 
                       enemies[i].health, enemies[i].maxHealth);
                printf("║ %-44s ║\n", line);
            } else {
                aliveMap[i] = 0;
            }
        }
        printf("║ %-44s ║\n", "[0] Cancel");
        printf("╚══════════════════════════════════════════════╝\n");
        printf("Select: ");
        
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
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║ %-44s ║\n", "*** VIEW STATS ***");
        printf("╠══════════════════════════════════════════════╣\n");
        printf("║ %-44s ║\n", "[1] View All Champions");
        printf("║ %-44s ║\n", "[2] View Monster");
        printf("║ %-44s ║\n", "[0] Back");
        printf("╚══════════════════════════════════════════════╝\n");
        printf("Choice: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        
        if (choice == 0) break;
        
        if (choice == 1) {
            printf("\n╔══════════════════════════════════════════════╗\n");
            printf("║ %-44s ║\n", "*** All Champions ***");
            printf("╠══════════════════════════════════════════════╣\n");
            int i;
            for (i = 0; i < 3; i++) {
                Champion c = game->champion[i];
                if (c.maxHealth == 0) continue;
                char line[128];
                snprintf(line, sizeof(line), "Champion %d - %s: HP %d/%d | Damage: %d%s",
                       i + 1, champion_string[c.class],
                       c.health, c.maxHealth, c.damage,
                       c.health <= 0 ? " [DEFEATED]" : "");
                printf("║ %-44s ║\n", line);
            }
            printf("╚══════════════════════════════════════════════╝\n");
        } else if (choice == 2) {
            printf("\n╔══════════════════════════════════════════════╗\n");
            printf("║ %-44s ║\n", "*** Select Monster ***");
            printf("╠══════════════════════════════════════════════╣\n");
            int aliveMap[enemyCount];
            int counter = 0; 
            int i;
            for (i = 0; i < enemyCount; i++) {
                if (enemies[i].health > 0) {
                    aliveMap[i] = 1;
                    char line[128];
                    snprintf(line, sizeof(line), "[%d] %s", ++counter, enemies[i].name);
                    printf("║ %-44s ║\n", line);
                } else {
                    aliveMap[i] = 0;
                }
            }
            printf("╚══════════════════════════════════════════════╝\n");
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
    char title[64];
    snprintf(title, sizeof(title), "*** Monster %d ***", index);
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║ %-44s ║\n", title);
    printf("╠══════════════════════════════════════════════╣\n");
    char line[128];
    snprintf(line, sizeof(line), "Name: %s", m->name);
    printf("║ %-44s ║\n", line);
    snprintf(line, sizeof(line), "HP: %d/%d", m->health, m->maxHealth);
    printf("║ %-44s ║\n", line);
    snprintf(line, sizeof(line), "Damage: %d", m->damage);
    printf("║ %-44s ║\n", line);
    printf("╚══════════════════════════════════════════════╝\n");
}
void printCombatStatus(Game *game, Monster enemies[], int enemyCount) {
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║ %-44s ║\n", "*** COMBAT STATUS ***");
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║ %-44s ║\n", "Champions:");
    printf("╠══════════════════════════════════════════════╣\n");
    int i;
    for (i = 0; i < 3; i++) {
        char line[128];
        if (game->champion[i].health > 0) {
            if (champBuffTurns[i] > 0) {
                int critp = (int)(champCritBonus[i] * 100 + 0.5);
                snprintf(line, sizeof(line), "[%d] %s: HP %d/%d | Buff:+%d ATK, +%d%% CRIT (T:%d)", i + 1,
                       champion_string[game->champion[i].class],
                       game->champion[i].health, game->champion[i].maxHealth,
                       champBuffValue[i], critp, champBuffTurns[i]);
            } else {
                snprintf(line, sizeof(line), "[%d] %s: HP %d/%d", i + 1,
                       champion_string[game->champion[i].class],
                       game->champion[i].health, game->champion[i].maxHealth);
            }
        } else {
            snprintf(line, sizeof(line), "[%d] DEFEATED", i + 1);
        }
        printf("║ %-44s ║\n", line);
    }
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║ %-44s ║\n", "Monsters:");
    printf("╠══════════════════════════════════════════════╣\n");
    int j;
    for (j = 0; j < enemyCount; j++) {
        char line[128];
        if (enemies[j].health > 0) {
            if (enemyDebuffTurns[j] > 0) {
                snprintf(line, sizeof(line), "[%d] %s: HP %d/%d | Debuff:-%d (T:%d)", j + 1, enemies[j].name,
                       enemies[j].health, enemies[j].maxHealth,
                       enemyDebuffValue[j], enemyDebuffTurns[j]);
            } else {
                snprintf(line, sizeof(line), "[%d] %s: HP %d/%d", j + 1, enemies[j].name,
                       enemies[j].health, enemies[j].maxHealth);
            }
        } else {
            snprintf(line, sizeof(line), "[%d] %s: DEFEATED", j + 1, enemies[j].name);
        }
        printf("║ %-44s ║\n", line);
    }
    printf("╚══════════════════════════════════════════════╝\n");
}
void createCombat(Game *game) {
    initCombat(game);
}