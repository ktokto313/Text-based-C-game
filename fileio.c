#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_object.h"
#include "fileio.h"
#include <cJSON.h>

#define SAVE_DIRECTORY "./saves/"
#define MAP_FILE "./map.json"
int current_save;

void initFileIO() {
    current_save = 0;
}

// Make multithreading for this later
void saveGame(Game * game) {
    char temp[50];
    sprintf(temp, "%s%s%d\n", SAVE_DIRECTORY, /*autosave*/1?"autosave_":"", current_save);
    int fpointer = open(temp, O_WRONLY);
    if (fpointer == -1) {
        printf("error");
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "level", game->level);
    cJSON_AddNumberToObject(root, "day", game->day);
    cJSON_AddNumberToObject(root, "timeOfTheDay", game->timeOfTheDay);
    cJSON *champion = cJSON_AddArrayToObject(root, "champions");
    int i;
    for (i = 0; i < 3;i++) {
        cJSON *temp = cJSON_CreateObject();
        cJSON_AddNumberToObject(temp, "health", game->champion[i].health);
        cJSON_AddNumberToObject(temp, "maxHealth", game->champion[i].maxHealth);
        cJSON_AddNumberToObject(temp, "damage", game->champion[i].damage);
        cJSON_AddItemToArray(champion, temp);       
    }

    char *json_str = cJSON_Print(root);

    // Write JSON string to file
    write(fpointer, json_str, strlen(json_str));

    // Cleanup
    close(fpointer);
    cJSON_free(json_str);
    cJSON_Delete(root);   
}

void loadGame(Game * game) {
    char filename[20] = "buhbuh\0";
    // Unsafe but handled in main
    int fd = open(filename, O_RDONLY);
    struct stat st;

    if (fstat(fd, &st) != 0) {
        //Handle
    } 

    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Read fail! Please try another file");
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    
    cJSON *temp = cJSON_GetObjectItem(root, "level");
    if (cJSON_IsNumber(temp)) {
        game->level = temp->valueint;
    }
    temp = cJSON_GetObjectItem(root, "day");
    if (cJSON_IsNumber(temp)) {
        game->day = temp->valueint;
    }
    temp = cJSON_GetObjectItem(root, "timeOfTheDay");
    if (cJSON_IsNumber(temp)) {
        game->timeOfTheDay = temp->valueint;
    }
    cJSON *champions = cJSON_GetObjectItem(root, "champions");
    cJSON *item = NULL;
    for (int i = 0;i < 3;i ++) {
        item = cJSON_GetArrayItem(champions, i);
        temp = cJSON_GetObjectItem(item, "health");
        if (cJSON_IsNumber(temp)) {
            game->champion[i].health = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, "maxHealth");
        if (cJSON_IsNumber(temp)) {
            game->champion[i].maxHealth = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, "damage");
        if (cJSON_IsNumber(temp)) {
            game->champion[i].damage = temp->valueint;
        }
    }

    free(root);
    // temp = cJSON_GetObjectItem(root, "timeOfTheDay");
    // if (cJSON_IsNumber(temp)) {
    //     game->timeOfTheDay = temp->valueint;
    // }
}

void saveMap(Game * game) {
    
}

void loadMapAndLocationData(Game * game) {
   
}

void loadConfig(Game * game) {
    // If config file not found init config
}

void loadShop(Game * game) {}