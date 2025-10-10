void initCombat(Game *game, Location *location) {
    printf("\n=== COMBAT START ===\n");
    printf("Monsters appear!\n");
    Node *node = location->monsterList.head;
    while (node != NULL) {
        Monster *m = (Monster *)node->value;
        printf(" - %s (HP: %d, DMG: %d)\n", m->name, m->health, m->damage);
        node = node->next;
    }
    int turn = 0;
    CombatResult combatStatus = COMBAT_CONTINUE;
    while (combatStatus == COMBAT_CONTINUE) {
        if (turn == 0) {
            // Champion turns
            for (int i = 0; i < 3; i++) {
                Champion *c = &game->champion[i];
                if (c->health <= 0) continue;
                int hasAttacked = 0;
                int turnEnded = 0;
                while (!turnEnded) {
                    printf("\n=== Champion %d's Turn (HP: %d/%d) ===\n", 
                           i + 1, c->health, c->maxHealth);
                    printf("[1] Attack %s\n", hasAttacked ? "(Used)" : "");
                    printf("[2] Use Skill\n");
                    printf("[3] Use Item\n");
                    printf("[4] Skip Turn\n");
                    printf("Choose action: ");
                    int choice;
                    scanf("%d", &choice);
                    switch (choice) {
                        case 1:
                            if (hasAttacked) {
                                printf("You've already attacked this turn.\n");
                                break;
                            }
                            {
                                // Show alive monsters
                                printf("Choose target:\n");
                                Node *n = location->monsterList.head;
                                int monsterIndex = 1;
                                while (n != NULL) {
                                    Monster *m = (Monster *)n->value;
                                    if (m->health > 0) {
                                        printf("[%d] %s (HP: %d/%d)\n", 
                                               monsterIndex, m->name, m->health, m->maxHealth);
                                    }
                                    monsterIndex++;
                                    n = n->next;
                                }
                                printf("Select target (1-%d): ", monsterIndex - 1);
                                int targetChoice;
                                scanf("%d", &targetChoice);
                                n = location->monsterList.head;
                                Monster *target = NULL;
                                int currentIndex = 1;
                                while (n != NULL) {
                                    Monster *m = (Monster *)n->value;
                                    if (m->health > 0) {
                                        if (currentIndex == targetChoice) {
                                            target = m;
                                            break;
                                        }
                                        currentIndex++;
                                    }
                                    n = n->next;
                                }
                                if (target == NULL) {
                                    printf("Invalid target!\n");
                                    break;
                                }
                                int dmg = c->damage;  
                                target->health -= dmg;
                                printf("Champion %d attacks %s for %d damage!\n", 
                                       i + 1, target->name, dmg);
                                if (target->health <= 0) {
                                    printf("%s is defeated!\n", target->name);
                                    removeDefeated(&location->monsterList, 1);
                                    if (allMonstersDefeated(location->monsterList)) {
                                        combatStatus = COMBAT_VICTORY;
                                        turnEnded = 1;
                                        break;
                                    }
                                }
                                hasAttacked = 1;
                                turnEnded = 1;
                            }
                            break;
                        case 2:
                            {
                                printf("Choose target:\n");
                                Node *n = location->monsterList.head;
                                int monsterIndex = 1;
                                while (n != NULL) {
                                    Monster *m = (Monster *)n->value;
                                    if (m->health > 0) {
                                        printf("[%d] %s (HP: %d/%d)\n", 
                                               monsterIndex, m->name, m->health, m->maxHealth);
                                    }
                                    monsterIndex++;
                                    n = n->next;
                                }
                                printf("Select target (1-%d): ", monsterIndex - 1);
                                int targetChoice;
                                scanf("%d", &targetChoice);
                                n = location->monsterList.head;
                                Monster *target = NULL;
                                int currentIndex = 1;
                                while (n != NULL) {
                                    Monster *m = (Monster *)n->value;
                                    if (m->health > 0) {
                                        if (currentIndex == targetChoice) {
                                            target = m;
                                            break;
                                        }
                                        currentIndex++;
                                    }
                                    n = n->next;
                                }
                                if (target == NULL) {
                                    printf("Invalid target!\n");
                                    break;
                                }
                                int skillDmg = c->skillDamage;
                                target->health -= skillDmg;
                                printf("Champion %d uses a skill on %s for %d damage!\n", 
                                       i + 1, target->name, skillDmg);
                                if (target->health <= 0) {
                                    printf("%s is defeated!\n", target->name);
                                    removeDefeated(&location->monsterList, 1);
                                    if (allMonstersDefeated(location->monsterList)) {
                                        combatStatus = COMBAT_VICTORY;
                                    }
                                }
                            }
                            break;
                        case 3:
                            {
                                c->health += 20;
                                if (c->health > c->maxHealth)
                                    c->health = c->maxHealth;
                                printf("Champion %d uses an item and heals to %d HP!\n", 
                                       i + 1, c->health);
                            }
                            break;
                        case 4:
                            printf("Champion %d skips their turn.\n", i + 1);
                            turnEnded = 1;
                            break;
                        default:
                            printf("Invalid choice. Try again.\n");
                            break;
                    }
                    if (combatStatus != COMBAT_CONTINUE) break;
                }
                if (combatStatus != COMBAT_CONTINUE) break;
            }
            turn = 1;
        } else {
            printf("\n=== Monsters' Turn ===\n");
            Node *mNode = location->monsterList.head;
            while (mNode != NULL) {
                Monster *m = (Monster *)mNode->value;
                if (m->health > 0) {
                    Champion *target = NULL;
                    for (int attempt = 0; attempt < 10; attempt++) {
                        int targetIndex = rand() % 3;
                        if (game->champion[targetIndex].health > 0) {
                            target = &game->champion[targetIndex];
                            break;
                        }
                    }
                    if /*All champion is Dead mean:*/ (target == NULL) {
                        combatStatus = COMBAT_DEFEAT;
                        break;
                    }
                    int dmg = m->damage;
                    target->health -= dmg;
                    printf("%s attacks Champion %d for %d damage!\n", m->name, (int)(target - game->champion) + 1, dmg);
                    if (target->health <= 0) {
                        printf("Champion %d has fallen!\n", (int)(target - game->champion) + 1);
                        if (allChampionsDefeated(game)) {
                            combatStatus = COMBAT_DEFEAT;
                            break;
                        }
                    }
                }
                mNode = mNode->next;
            }
            turn = 0;
        }
        if (combatStatus == COMBAT_CONTINUE) {
            printCombatStatus(game, location->monsterList);
        }
    }
    if (combatStatus == COMBAT_DEFEAT) {
        printf("\n=== DEFEAT ===\n");
        printf("All champions are defeated!\n");
    } else if (combatStatus == COMBAT_VICTORY) {
        printf("\n=== VICTORY ===\n");
        printf("All monsters are defeated!\n");
    }
}
