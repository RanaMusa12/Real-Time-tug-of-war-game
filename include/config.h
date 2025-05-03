#ifndef CONFIG_H
#define CONFIG_H
#include <sys/types.h>  // for pid_t
#include <stdbool.h>    // for bool


#define PLAYERS_PER_TEAM 4
#define NUM_PLAYERS (2 * PLAYERS_PER_TEAM)
// Player energy settings
#define ENERGY_MIN 50
#define ENERGY_MAX 100
#define ENERGY_DECAY_MIN 1
#define ENERGY_DECAY_MAX 10
#define MAX_PLAYERS 8

// Player falling/recovery settings
#define FALL_PROBABILITY 5       // 5% chance to fall
#define RECOVERY_TIME_MIN 1      // 1 second min recovery
#define RECOVERY_TIME_MAX 5      // 5 seconds max recovery

// Game rules
#define WIN_THRESHOLD 700        // Effort needed to win
#define GAME_TIME 300          
#define MAX_CONSECUTIVE_WINS 2   // Best of 3 rounds
#define max_score_to_exit 890 

#define MAX_ROPE_DISPLACEMENT 4.0f  // Maximum rope displacement from center
#define MAX_ROUND_WINS 5 

typedef struct {
    int id;
    int energy;
    int is_fallen;
    int pipe_fd;
    int win_state;
    int current_round;
} Player;
typedef struct {
    int effort_levels[NUM_PLAYERS];
    int team1_effort;
    int team2_effort;
    int round_wins[2];
    int current_round;
    int last_round_winner;
    int consecutive_wins;
    int pipes[NUM_PLAYERS][2];
    pid_t player_pids[NUM_PLAYERS];
    pid_t players[MAX_PLAYERS];  // or whatever size you need
} GameState;

extern GameState state;
extern bool gameOver;
extern int winningTeam;
extern float celebrationTimer;
// Function declarations
void initialize_game();
void start_players();
void calculate_team_effort();
void referee_decision();
void handle_sigchld(int sig);
void cleanup();
void send_pull_command();
void update_gui_data();
void game_loop(int value);
int load_config(const char *filename) ;

#endif 
