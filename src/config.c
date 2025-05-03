#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Initialize configuration variables with default values
int PLAYERS_PER_TEAM = 4;
int NUM_PLAYERS = 8;  // 2 * PLAYERS_PER_TEAM
int ENERGY_MIN = 50;
int ENERGY_MAX = 100;
int ENERGY_DECAY_MIN = 1;
int ENERGY_DECAY_MAX = 10;
int FALL_PROBABILITY = 5;
int RECOVERY_TIME_MIN = 1;
int RECOVERY_TIME_MAX = 5;
int WIN_THRESHOLD = 700;
int GAME_TIME = 300;
int MAX_CONSECUTIVE_WINS = 2;
int max_score_to_exit = 890;
float MAX_ROPE_DISPLACEMENT = 4.0f;

// Helper function to trim whitespace
static char* trim_whitespace(char* str) {
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return str;
    
    char* end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    *(end+1) = 0;
    return str;
}
int load_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Warning: Could not open config file '%s', using defaults\n", filename);
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        
        // Skip comments and empty lines
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        
        // Split into key-value pair
        char* separator = strchr(trimmed, '=');
        if (!separator) continue;
        
        *separator = '\0';
        char* key = trim(trimmed);
        char* value = trim(separator + 1);
        
        // Parse configuration values
        if (strcmp(key, "PLAYERS_PER_TEAM") == 0) {
            PLAYERS_PER_TEAM = atoi(value);
            NUM_PLAYERS = 2 * PLAYERS_PER_TEAM;
        }
        else if (strcmp(key, "ENERGY_MIN") == 0) ENERGY_MIN = atoi(value);
        else if (strcmp(key, "ENERGY_MAX") == 0) ENERGY_MAX = atoi(value);
        else if (strcmp(key, "ENERGY_DECAY_MIN") == 0) ENERGY_DECAY_MIN = atoi(value);
        else if (strcmp(key, "ENERGY_DECAY_MAX") == 0) ENERGY_DECAY_MAX = atoi(value);
        else if (strcmp(key, "FALL_PROBABILITY") == 0) FALL_PROBABILITY = atoi(value);
        else if (strcmp(key, "RECOVERY_TIME_MIN") == 0) RECOVERY_TIME_MIN = atoi(value);
        else if (strcmp(key, "RECOVERY_TIME_MAX") == 0) RECOVERY_TIME_MAX = atoi(value);
        else if (strcmp(key, "WIN_THRESHOLD") == 0) WIN_THRESHOLD = atoi(value);
        else if (strcmp(key, "GAME_TIME") == 0) GAME_TIME = atoi(value);
        else if (strcmp(key, "MAX_CONSECUTIVE_WINS") == 0) MAX_CONSECUTIVE_WINS = atoi(value);
        else if (strcmp(key, "max_score_to_exit") == 0) max_score_to_exit = atoi(value);
        else if (strcmp(key, "MAX_ROPE_DISPLACEMENT") == 0) MAX_ROPE_DISPLACEMENT = atof(value);
        else {
            fprintf(stderr, "Warning: Unknown config key '%s'\n", key);
        }
    }

    fclose(file);
    
    // Validate critical values
    if (PLAYERS_PER_TEAM <= 0 || PLAYERS_PER_TEAM > (MAX_PLAYERS/2)) {
        fprintf(stderr, "Invalid PLAYERS_PER_TEAM (%d), using default (4)\n", PLAYERS_PER_TEAM);
        PLAYERS_PER_TEAM = 4;
        NUM_PLAYERS = 8;
    }
    
    if (ENERGY_MIN <= 0 || ENERGY_MIN > ENERGY_MAX) {
        fprintf(stderr, "Invalid ENERGY_MIN (%d), using default (50)\n", ENERGY_MIN);
        ENERGY_MIN = 50;
    }
    
    return 0;
}
