#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "game_object.h"
#include "fileio.h"
#include <cJSON.h>

#define SAVE_DIRECTORY "./saves/"
#define JSON_DIR "./json"
#define MAP_FILE "./json/map.json"
#define LOCATION_DATA_FILE "./json/location_data.json"
#define CONFIG_FILE "./json/config.json"
#define SHOP_FILE "./json/shop.json"
#define MAX_AUTOSAVE 8
int current_save;

void initFileIO() {
    current_save = 0;
    mkdir(JSON_DIR, S_IRWXU);
    mkdir(SAVE_DIRECTORY, S_IRWXU);
}

// Make multithreading for this later
void saveGame(Game * game, int autosave) {
    char temp[50];
    if (!autosave) {
        sprintf(temp, "%sday%dtime%d.json", SAVE_DIRECTORY, game->day, game->timeOfTheDay);
        printf("Enter the name of the file you want to save (leave blank for default name): ");
        fgets(temp, 50, stdin);
        sprintf(temp, "%s%s", SAVE_DIRECTORY, temp);
    } else {
        // There is a bug where it will always override slot 0 first
        sprintf(temp, "%sautosave_%d.json", SAVE_DIRECTORY, current_save);
        current_save++;
        if (current_save > MAX_AUTOSAVE) {
            current_save = 0;
        }
    }
    int fd = open(temp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printf("error %d\n", errno);
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "level", game->level);
    cJSON_AddNumberToObject(root, "day", game->day);
    cJSON_AddNumberToObject(root, "timeOfTheDay", game->timeOfTheDay);
    cJSON_AddNumberToObject(root, "gold", game->gold);
    cJSON *itemList = cJSON_AddArrayToObject(root, "itemList");
    Node * current = game->itemList.head;
    while (current != NULL) {
        cJSON *item_object = cJSON_CreateObject();
        Item *item = (Item *)current->value;
        cJSON_AddNumberToObject(item_object, "type", item->type);
        cJSON_AddStringToObject(item_object, "name", item->name);
        cJSON_AddNumberToObject(item_object, "value", item->value);
        cJSON_AddNumberToObject(item_object, "effectValue", item->effectValue);
        cJSON_AddItemToArray(itemList, item_object);
    }
    cJSON *champion = cJSON_AddArrayToObject(root, "champions");
    int i;
    for (i = 0; i < 3;i++) {
        cJSON *temp = cJSON_CreateObject();
        cJSON_AddNumberToObject(temp, "health", game->champion[i].health);
        cJSON_AddNumberToObject(temp, "maxHealth", game->champion[i].maxHealth);
        cJSON_AddNumberToObject(temp, "damage", game->champion[i].damage);
        cJSON_AddNumberToObject(temp, "class", game->champion[i].class);
        cJSON_AddNumberToObject(temp, "level", game->champion[i].level);
        cJSON_AddNumberToObject(temp, "xp", game->champion[i].xp);
        cJSON_AddItemToArray(champion, temp);
    }

    char *json_str = cJSON_Print(root);

    // Write JSON string to file
    write(fd, json_str, strlen(json_str));

    // Cleanup
    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);   
}

void loadGame(Game * game) {
    // 8kb for file loading is kinda weird but it's what it's for now
    char filenames[32][256];
    DIR * dir = opendir(SAVE_DIRECTORY);
    if (dir == NULL) {
        
    }
    struct dirent * dirent;
    int count = 0;
    while ((dirent = readdir(dir)) != NULL) {
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) 
            continue;
        strcpy(filenames[count], dirent->d_name);
        count++;
    }
    int choice;
    do {
        for (int i = 0;i < count;i++) {
            printf("%d. %s\n", i+1, filenames[i]);
        }
        printf("%d. %s", count+1, "Quit\n");
        printf("Select the file you want to load: ");
        scanf("%d", &choice);
        choice--;
        if (choice == count) return;
        if (choice < 0 || choice > count) {
            continue;
        }
        break;
    } while (1);
    char filename[300];
    sprintf(filename, "%s%s", SAVE_DIRECTORY, filenames[choice]);
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
    cJSON *itemList = cJSON_GetObjectItem(root, "itemList");
    
    game->level = cJSON_GetObjectItem(root, "level")->valueint;
    game->day = cJSON_GetObjectItem(root, "day")->valueint;
    game->timeOfTheDay = cJSON_GetObjectItem(root, "timeOfTheDay")->valueint;
    game->gold = cJSON_GetObjectItem(root, "gold")->valueint;
    for (int i = 0;i < cJSON_GetArraySize(itemList);i++) {
        cJSON * temp = cJSON_GetArrayItem(itemList, i);
        Item * item = (Item *) malloc(sizeof(Item));
        item->type = cJSON_GetObjectItem(temp, "type")->valueint;
        strcpy(item->name, cJSON_GetObjectItem(temp, "name")->valuestring);
        item->value = cJSON_GetObjectItem(temp, "value")->valueint;
        item->effectValue = cJSON_GetObjectItem(temp, "effectValue")->valueint;
        insert(&(game->itemList), item);
    }
    cJSON *champions = cJSON_GetObjectItem(root, "champions");
    for (int i = 0;i < 3;i ++) {
        cJSON *champion = cJSON_GetArrayItem(champions, i);
        game->champion[i].health = cJSON_GetObjectItem(champion, "health")->valueint;
        game->champion[i].maxHealth = cJSON_GetObjectItem(champion, "maxHealth")->valueint;
        game->champion[i].damage = cJSON_GetObjectItem(champion, "damage")->valueint;
        game->champion[i].class = cJSON_GetObjectItem(champion, "class")->valueint;
        game->champion[i].level = cJSON_GetObjectItem(champion, "level")->valueint;
        game->champion[i].xp = cJSON_GetObjectItem(champion, "xp")->valueint;
    }

    cJSON_Delete(root);
    // temp = cJSON_GetObjectItem(root, "timeOfTheDay");
    // if (cJSON_IsNumber(temp)) {
    //     game->timeOfTheDay = temp->valueint;
    // }
}

void saveMap(Game * game) {
    int fd = open(MAP_FILE, O_WRONLY);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateArray();
    int *pt = game->map;
    for (int i = 0;i < game->mapSize;i++) {
        // char temp[7] = "level";
        // temp[5] = i;
        // temp[6] = '\0';
        cJSON *adjMap = cJSON_CreateIntArray(pt, game->mapSize);
        cJSON_AddItemToArray(root, adjMap);
    }

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);
}

void initMap(Game * game) {
    int fd = open(MAP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateArray();
    for (int i = 0;i < 8;i++) {
        int a[8] = {1, 1, 1, 1, 1, 1, 1, 1};
        cJSON *adjMap = cJSON_CreateIntArray(a, 8);
        cJSON_AddItemToArray(root, adjMap);
    }

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);
}

void loadMap(Game * game) {
    int fd = open(MAP_FILE, O_RDONLY);
    if (fd == -1) {
        initMap(game);
        fd = open(MAP_FILE, O_RDONLY);
    }
    
    struct stat st;
    if (fstat(fd, &st) != 0) {
        printf("Map reading failed! Exiting!");
        game->initialized = 0;
        return;
    } 
    if (st.st_size == 0) {
        initMap(game);
        fd = open(MAP_FILE, O_RDONLY);
    }
    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Map reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    if (!cJSON_IsArray(root)) {
        printf("Map reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    game->mapSize = cJSON_GetArraySize(root);
    game->map = malloc(game->mapSize * game->mapSize * sizeof(int));
    int *pt = game->map;
    
    for (int i = 0;i < game->mapSize;i++) {
        cJSON *adjMap = cJSON_GetArrayItem(root, i);
        for (int j = 0;j < game->mapSize;j++) {
            *pt = cJSON_GetArrayItem(adjMap, j)->valueint;
            pt++;
        }
    }
    cJSON_Delete(root);
}

void initLocationData(Game * game) {
    int fd = open(LOCATION_DATA_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateArray();
    for (int i = 0;i < game->mapSize;i++) {
        cJSON *locationData = cJSON_CreateObject();
        cJSON_AddStringToObject(locationData, "name", "bla");
        cJSON *monsterList = cJSON_AddArrayToObject(locationData, "monsterList");
        cJSON *monster = cJSON_CreateObject();
        cJSON_AddNumberToObject(monster, "health", 100);
        cJSON_AddNumberToObject(monster, "maxHealth", 100);
        cJSON_AddNumberToObject(monster, "damage", 10);
        cJSON_AddStringToObject(monster, "name", "buhbuh");
        cJSON_AddItemToArray(monsterList, monster);
        cJSON_AddItemToArray(root, locationData);
    }

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);
}

void loadLocationData(Game * game) {
    int fd = open(LOCATION_DATA_FILE, O_RDONLY);
    if (fd == -1) {
        initLocationData(game);
        fd = open(LOCATION_DATA_FILE, O_RDONLY);
    }
    
    struct stat st;
    if (fstat(fd, &st) != 0) {
        printf("Location data reading failed! Exiting!\n");
        game->initialized = 0;
        return;
    } 
    if (st.st_size == 0) {
        initLocationData(game);
        fd = open(LOCATION_DATA_FILE, O_RDONLY);
    }
    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Location data reading failed! Exiting!\n");
        printf("Errno %d\n", errno);
        game->initialized = 0;
        return;
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    if (!cJSON_IsArray(root)) {
        printf("Location data reading failed! Exiting!\n");
        game->initialized = 0;
        return;
    }

    game->locationData = (LocationData *) malloc(game->mapSize * sizeof(LocationData));
    LocationData * temp = game->locationData;
    init(&(temp->monsterList));
    
    for (int i = 0;i < game->mapSize;i++) {
        cJSON *locationData = cJSON_GetArrayItem(root, i);
        strcpy(temp->name, cJSON_GetObjectItem(locationData, "name")->valuestring);
        cJSON *monsterList = cJSON_GetObjectItem(locationData, "monsterList");
        for (int j = 0;j < cJSON_GetArraySize(monsterList);j++) {
            cJSON * monster_json = cJSON_GetArrayItem(monsterList, j);
            Monster * monster = (Monster *) malloc(sizeof(Monster));
            monster->health = cJSON_GetObjectItem(monster_json, "health")->valueint;
            monster->maxHealth = cJSON_GetObjectItem(monster_json, "maxHealth")->valueint;
            monster->damage = cJSON_GetObjectItem(monster_json, "damage")->valueint;
            strcpy(monster->name, cJSON_GetObjectItem(monster_json, "name")->valuestring);
            insert(&(temp->monsterList), monster);
        }
        temp++;
    }
    cJSON_Delete(root);
}
// implement location data 

void loadMapAndLocationData(Game * game) {
    loadMap(game);
    loadLocationData(game);
}

void saveConfig(Game * game) {
    int fd = open(CONFIG_FILE, O_WRONLY);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateObject();
    Config config = game->config;
    cJSON_AddNumberToObject(root, "allowSaveInCombat", config.allowSaveInCombat);
    cJSON_AddNumberToObject(root, "difficulty", config.difficulty);
    cJSON_AddNumberToObject(root, "baseLvUpXp", config.baseLvUpXp);
    cJSON_AddNumberToObject(root, "lvUpXpMul", config.lvUpXpMul);
    cJSON_AddNumberToObject(root, "chestGoldMin", config.chestGoldMin);
    cJSON_AddNumberToObject(root, "chestGoldMax", config.chestGoldMax);
    cJSON_AddNumberToObject(root, "chestItemChance", config.chestItemChance);
    cJSON_AddNumberToObject(root, "lockpickChance", config.lockpickChance);
    cJSON_AddNumberToObject(root, "trapDisarmChance", config.trapDisarmChance);
    cJSON_AddNumberToObject(root, "trapDisarmExp", config.trapDisarmExp);
    cJSON_AddNumberToObject(root, "trapDamage", config.trapDamage);

    cJSON_AddNumberToObject(root, "explorationTurn", config.explorationTurn);
    cJSON *scenarioWeights = cJSON_CreateIntArray(config.scenarioWeights, 3);
    cJSON_AddItemToObject(root, "scenarioWeights", scenarioWeights);
    cJSON_AddNumberToObject(root, "maxTimeOfTheDay", config.maxTimeOfTheDay);
    cJSON_AddNumberToObject(root, "teleportPenalty", config.teleportPenalty);
    cJSON_AddNumberToObject(root, "sellValue", config.sellValue);
    cJSON_AddNumberToObject(root, "saveInterval", config.saveInterval);
    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);

}

void initConfig(Game * game) {
    int fd = open(CONFIG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "allowSaveInCombat", 0);
    cJSON_AddNumberToObject(root, "difficulty", 1);
    cJSON_AddNumberToObject(root, "baseLvUpXp", 100);
    cJSON_AddNumberToObject(root, "lvUpXpMul", 1.2);
    cJSON_AddNumberToObject(root, "chestGoldMin", 50);
    cJSON_AddNumberToObject(root, "chestGoldMax", 100);
    cJSON_AddNumberToObject(root, "chestItemChance", 50);
    cJSON_AddNumberToObject(root, "lockpickChance", 80);
    cJSON_AddNumberToObject(root, "trapDisarmChance", 80);
    cJSON_AddNumberToObject(root, "trapDisarmExp", 100);
    cJSON_AddNumberToObject(root, "trapDamage", 10);

    cJSON_AddNumberToObject(root, "explorationTurn", 3);
    int a[3] = {30, 30, 40};
    cJSON *scenarioWeights = cJSON_CreateIntArray(a, 3);
    cJSON_AddItemToObject(root, "scenarioWeights", scenarioWeights);
    cJSON_AddNumberToObject(root, "maxTimeOfTheDay", 4);
    cJSON_AddNumberToObject(root, "teleportPenalty", 1);
    cJSON_AddNumberToObject(root, "sellValue", 0.5);
    cJSON_AddNumberToObject(root, "saveInterval", 10);
    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));
   
    close(fd);
    // cJSON_free(json_str);
    cJSON_Delete(root);
}

void loadConfig(Game * game) {
    // If config file not found init 
    int fd = open(CONFIG_FILE, O_RDONLY);
    if (fd == -1) {
        initConfig(game);
        fd = open(CONFIG_FILE, O_RDONLY);
    }
    
    struct stat st;
    if (fstat(fd, &st) != 0) {
        printf("Config reading failed! Exiting!\n");
        printf("Stat failed errno\n");
        game->initialized = 0;
        return;
    } 
    if (st.st_size == 0) {
        initConfig(game); 
    }
    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Config reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    game->config.allowSaveInCombat = cJSON_GetObjectItem(root, "allowSaveInCombat")->valueint;
    game->config.difficulty = cJSON_GetObjectItem(root, "difficulty")->valueint;
    game->config.baseLvUpXp = cJSON_GetObjectItem(root, "baseLvUpXp")->valueint;
    game->config.lvUpXpMul = cJSON_GetObjectItem(root, "lvUpXpMul")->valueint;
    game->config.chestGoldMin = cJSON_GetObjectItem(root, "chestGoldMin")->valueint;
    game->config.chestGoldMax = cJSON_GetObjectItem(root, "chestGoldMax")->valueint;
    game->config.chestItemChance = cJSON_GetObjectItem(root, "chestItemChance")->valueint;
    game->config.lockpickChance = cJSON_GetObjectItem(root, "lockpickChance")->valueint;
    game->config.trapDisarmChance = cJSON_GetObjectItem(root, "trapDisarmChance")->valueint;
    game->config.trapDisarmExp = cJSON_GetObjectItem(root, "trapDisarmExp")->valueint;
    game->config.trapDamage = cJSON_GetObjectItem(root, "trapDamage")->valueint;

    game->config.explorationTurn = cJSON_GetObjectItem(root, "explorationTurn")->valueint;
    cJSON *scenarioWeights = cJSON_GetObjectItem(root, "scenarioWeights");
    game->config.scenarioWeights[0] = cJSON_GetArrayItem(scenarioWeights, 0)->valueint;
    game->config.scenarioWeights[1] = cJSON_GetArrayItem(scenarioWeights, 1)->valueint;
    game->config.scenarioWeights[2] = cJSON_GetArrayItem(scenarioWeights, 2)->valueint;
    game->config.maxTimeOfTheDay = cJSON_GetObjectItem(root, "maxTimeOfTheDay")->valueint;
    game->config.teleportPenalty = cJSON_GetObjectItem(root, "teleportPenalty")->valueint;
    game->config.sellValue = cJSON_GetObjectItem(root, "sellValue")->valuedouble;
    game->config.saveInterval = cJSON_GetObjectItem(root, "saveInterval")->valueint;

    cJSON_Delete(root);
}

void saveShop(Game * game) {
    int fd = open(SHOP_FILE, O_WRONLY);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateObject();
    cJSON *itemList = cJSON_AddArrayToObject(root, "itemList");
    Node * current = game->shop.itemList.head;
    while (current != NULL) {
        cJSON *item_object = cJSON_CreateObject();
        Item *item = (Item *)current->value;
        cJSON_AddNumberToObject(item_object, "type", item->type);
        cJSON_AddStringToObject(item_object, "name", item->name);
        cJSON_AddNumberToObject(item_object, "value", item->value);
        cJSON_AddNumberToObject(item_object, "effectValue", item->effectValue);
        cJSON_AddItemToArray(itemList, item_object);
    }

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);
}

void initShop(Game * game) {
    int fd = open(SHOP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printf("error when open file %d", errno);
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *itemList = cJSON_AddArrayToObject(root, "itemList");
    Item item;
    item.type = HEALTH_POTION;
    item.effectValue = 1;
    item.name[0] = 'h';
    item.name[1] = 'a';
    item.name[2] = '\0';
    item.value = 50;
    cJSON *item_object = cJSON_CreateObject();
    cJSON_AddNumberToObject(item_object, "type", item.type);
    cJSON_AddStringToObject(item_object, "name", item.name);
    cJSON_AddNumberToObject(item_object, "value", item.value);
    cJSON_AddNumberToObject(item_object, "effectValue", item.effectValue);
    cJSON_AddItemToArray(itemList, item_object);

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_Delete(root);
}

void loadShop(Game * game) {
    int fd = open(SHOP_FILE, O_RDONLY);
    if (fd == -1) {
        initShop(game);
        fd = open(SHOP_FILE, O_RDONLY);
    }
    
    struct stat st;
    if (fstat(fd, &st) != 0) {
        printf("Shop reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    if (st.st_size == 0) {
        initShop(game);
        fd = open(SHOP_FILE, O_RDONLY);
    }
    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Shop reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    cJSON *itemList = cJSON_GetObjectItem(root, "itemList");
    cJSON *temp;
    init(&(game->shop.itemList));
    
    for (int i = 0;i < cJSON_GetArraySize(itemList);i++) {
        temp = cJSON_GetArrayItem(itemList, i);
        Item * item = (Item *) malloc(sizeof(Item));
        item->type = cJSON_GetObjectItem(temp, "type")->valueint;
        strcpy(item->name, cJSON_GetObjectItem(temp, "name")->valuestring);
        item->value = cJSON_GetObjectItem(temp, "value")->valueint;
        item->effectValue = cJSON_GetObjectItem(temp, "effectValue")->valueint;
        insert(&(game->shop.itemList), item);
    }
    cJSON_Delete(root);
}