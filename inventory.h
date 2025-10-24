#pragma once
#include "game_object.h"
void openInventoryMenu(Game *game);
void addItemToInventory(Game *game, Item *item);
void handleMonsterDrops(Game *game, LinkedList *drops, int autoPickup);
Item *createItemInstance(const Item *templateItem, int qty);
void serializeInventory(Game *game, FILE *fp);
void deserializeInventory(Game *game, FILE *fp);
void printInventory(Game *game);
