#include "fileio.c"
#include "game.c"
#include "explore.c"
#include "combat.c"
#include "game_object.h"

Game game;

void showMainMenu();
void doGameTick();

int main() {
    showMainMenu(0);
}