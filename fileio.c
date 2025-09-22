#include "lib/cJson/cJSON.h"
#include "game_object.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define SAVE_DIRECTORY "./saves/"
int current_save;

void initFileIO() {
    current_save = 0;
}

// Make multithreading for this later
void saveGame(Game * game, int autosave) {
    char temp[50];
    sprintf(temp, "%s%s%n", SAVE_DIRECTORY, autosave?"autosave_":"", current_save);
    int fpointer = open(temp, O_WRONLY);
    if (fpointer == -1) {
        printf("error");
    }

    
}

void loadGame(Game * game) {
    
}