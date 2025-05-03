#include "gui.h"
#include "config.h"
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

Player players[NUM_PLAYERS];
GameState state;
bool gameOver = false;
int winningTeam = 0;
float celebrationTimer = 5.0f;
float ropeDisplacement = 0.0f;      // Current rope displacement (-left, +right)


GameState state;


float waveAngle = -45.0f;
int waveDirection = 1;
const float waveSpeed = 1.5f;
const float maxAngle = 45.0f;
// Game state variables
bool gameStarted = false;
float gameStartTimer = 0.0f;
const float GAME_START_DISPLAY_TIME = 3.0f;  // Definition here



void initialize_game() {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (pipe(state.pipes[i]) == -1) {
            perror("pipe creation failed");
            exit(1);
        }
        printf("Created pipe for player %d: readFD=%d, writeFD=%d\n", 
              i, state.pipes[i][0], state.pipes[i][1]);
              
              // Initialize player data
        players[i].id = i;  
        players[i].energy = ENERGY_MAX;
        players[i].is_fallen = 0;
        players[i].win_state = 0;
        players[i].current_round = 1;
    }
    qsort(players, PLAYERS_PER_TEAM, sizeof(Player), comparePlayers); // Team 1
    qsort(players + PLAYERS_PER_TEAM, PLAYERS_PER_TEAM, sizeof(Player), comparePlayers); // Team 2
    state.current_round=1;
}
// Comparison function for sorting players by energy (lowest to highest)
int comparePlayers(const void* a, const void* b) {
    Player* pa = (Player*)a;
    Player* pb = (Player*)b;
    return pa->energy - pb->energy;
}

void calculate_team_effort() {
    state.team1_effort = 0;
    state.team2_effort = 0;

    //printf("\n=== EFFORT CALCULATION (Round %d) ===\n", state.current_round);
    //printf("Format: Team[ID] Energy→Effort (PositionFactor×Energy)\n\n");

    for (int i = 0; i < NUM_PLAYERS; i++) {
        // Read the player's current energy level
        int energy;
        int bytes = read(state.pipes[i][0], &energy, sizeof(int));
        
        if (bytes > 0) {
            players[i].energy = energy;  // Update player's energy
            
            // Calculate position factor (4 for anchor, 1 for front)
            int position_in_team = i % PLAYERS_PER_TEAM;
            int position_factor = 4 - position_in_team;
            
            // Calculate actual effort contribution
            int effort = energy * position_factor;
            state.effort_levels[i] = effort;  // Store calculated effort
            
            // Add to team total
            if (i < PLAYERS_PER_TEAM) {
                state.team1_effort += effort;
               // printf("Team1[P%d]: %d→%d (×%d) | ", 
                 //     i+1, energy, effort, position_factor);
            } else {
                state.team2_effort += effort;
               // printf("Team2[P%d]: %d→%d (×%d) | ",
                 //     i+1-PLAYERS_PER_TEAM, energy, effort, position_factor);
            }
            
            if ((i+1) % PLAYERS_PER_TEAM == 0) printf("\n");
        } else {
            printf("Player %d failed to report energy! ", i+1);
            if (bytes < 0) perror("Read error");
            
            // Use last known energy if read fails
            int energy = players[i].energy;
            int position_factor = 4 - (i % PLAYERS_PER_TEAM);
            state.effort_levels[i] = energy * position_factor;
        }
    }

    printf("\nTEAM TOTALS:\n");
    printf("Team 1: %d (from %d base energy)\n", 
          state.team1_effort, 
          state.team1_effort / 10); // Approximate base energy sum
    printf("Team 2: %d (from %d base energy)\n",
          state.team2_effort,
          state.team2_effort / 10);
    
    // Calculate rope displacement
    ropeDisplacement = (float)(state.team1_effort - state.team2_effort) / WIN_THRESHOLD;
    printf("Rope displacement: %.2f%%\n", ropeDisplacement * 100);
    printf("================================\n\n");
}

void start_players() {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        state.players[i] = fork();
        if (state.players[i] == 0) {
            // Child process (player)
            close(state.pipes[i][0]); // Close read end
            
            char id_str[10], pipe_fd_str[10];
            snprintf(id_str, sizeof(id_str), "%d", i);
            snprintf(pipe_fd_str, sizeof(pipe_fd_str), "%d", state.pipes[i][1]);
            
            // Use full path to player executable
            execl("./bin/player", "player", id_str, pipe_fd_str, NULL);
            perror("execl failed");
            exit(1);
        }
        // Parent closes write end
        close(state.pipes[i][1]);
    }
}
// Send pull command to all players
void send_pull_command() {
    char cmd = 'P'; // Pull command
    for (int i = 0; i < NUM_PLAYERS; i++) {
        write(state.pipes[i][1], &cmd, 1);
    }
}


// Determine round winner and check for game end
void referee_decision() {

    if (state.team1_effort >= WIN_THRESHOLD && 
        state.team1_effort > state.team2_effort) {
        printf("Team 1 Wins the Round!\n");
        state.round_wins[0]++;
        state.consecutive_wins = (state.last_round_winner == 1) ? 
            state.consecutive_wins + 1 : 1;
        state.last_round_winner = 1;
        
        // Update player win states for Team 1 (winners) and Team 2 (losers)
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (players[i].id != -1) { // if player slot is active
                if (i < MAX_PLAYERS / 2) { // Team 1 players
                    players[i].win_state = 1; // won
                } else { // Team 2 players
                    players[i].win_state = 2; // lost
                }
                players[i].current_round = state.current_round;
            }
        }
              // Check if team1 reached max round wins
        if (state.round_wins[0] >= MAX_ROUND_WINS) {
            printf("Game over! Team 1 reached %d round wins.\n", MAX_ROUND_WINS);
            gameOver = true;
            cleanup();
            return;
        }

        // Check if team1's effort exceeds max score to exit
        if (state.team1_effort >= max_score_to_exit) {
            printf("Game over! Team 1 reached the maximum score of %d.\n", max_score_to_exit);
            gameOver = true;
            cleanup();
        }
    } else if (state.team2_effort >= WIN_THRESHOLD && 
               state.team2_effort > state.team1_effort) {
        printf("Team 2 Wins the Round!\n");
        state.round_wins[1]++;
        state.consecutive_wins = (state.last_round_winner == 2) ? 
            state.consecutive_wins + 1 : 1;
        state.last_round_winner = 2;
        
        // Update player win states for Team 2 (winners) and Team 1 (losers)
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (players[i].id != -1) { // if player slot is active
                if (i >= MAX_PLAYERS / 2) { // Team 2 players
                    players[i].win_state = 1; // won
                } else { // Team 1 players
                    players[i].win_state = 2; // lost
                }
                players[i].current_round = state.current_round;
            }
        }
          if (state.round_wins[1] >= MAX_ROUND_WINS) {
            printf("Game over! Team 2 reached %d round wins.\n", MAX_ROUND_WINS);
            gameOver = true;
            cleanup();
            return;
        }

        // Check if team2's effort exceeds max score to exit
        if (state.team2_effort >= max_score_to_exit) {
            printf("Game over! Team 2 reached the maximum score of %d.\n", max_score_to_exit);
            gameOver = true;
            cleanup();
        }
    } else {
        return; // No winner yet
    }
     
        
    
    // Print player states
    printf("Player states after round %d:\n", state.current_round);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].id != -1) {
            printf("Player %d: %s\n", players[i].id, 
                   players[i].win_state == 1 ? "WON" : 
                   players[i].win_state == 2 ? "LOST" : "NO RESULT");
        }
    }
         if (state.round_wins[1] >= MAX_ROUND_WINS) {
            printf("Game over! Team 2 reached %d round wins.\n", MAX_ROUND_WINS);
            gameOver = true;
            cleanup();
            return;
        }
    
    // Check for consecutive wins
    if (state.consecutive_wins == MAX_CONSECUTIVE_WINS) {
        printf("Game over! Team %d won %d consecutive rounds.\n", 
               state.last_round_winner, MAX_CONSECUTIVE_WINS);
        gameOver = true;
        cleanup();
    }
    
    // Start new round
    state.current_round++;
    printf("Starting new round %d...\n", state.current_round);
    
        // Sort players for the new round
    qsort(players, PLAYERS_PER_TEAM, sizeof(Player), comparePlayers); // Team 1
    qsort(players + PLAYERS_PER_TEAM, PLAYERS_PER_TEAM, sizeof(Player), comparePlayers); // Team 
    
    // Reset player win states for new round
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].id != -1) {
            players[i].win_state = 0; // reset to no result
        }
    }
}

// Handle SIGCHLD signal for player processes
void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Cleanup resources
void cleanup() {
    // Send termination signal to players
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (state.players[i] > 0) {
            kill(state.players[i], SIGTERM);
        }
    }
    
    // Close pipes
    for (int i = 0; i < NUM_PLAYERS; i++) {
        close(state.pipes[i][0]);
    }
}

void update_gui_data() {
    // Get current energy levels from game state
    int team1Energy[PLAYERS_PER_TEAM];
    int team2Energy[PLAYERS_PER_TEAM];
    
    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        team1Energy[i] = state.effort_levels[i];
        team2Energy[i] = state.effort_levels[i + PLAYERS_PER_TEAM];
    }

    // Calculate leading team
    const char* leadingTeam = "Tie";
    if (state.team1_effort > state.team2_effort) leadingTeam = "Team 1";
    else if (state.team2_effort > state.team1_effort) leadingTeam = "Team 2";

    // Calculate remaining time
    int remainingTime = GAME_TIME;
    if (gameStarted && gameStartTime > 0) {
        remainingTime = GAME_TIME - (time(NULL) - gameStartTime);
        if (remainingTime < 0) remainingTime = 0;
    }

    // Update GUI display
    drawTeamInfoDisplay(
        team1Energy,
        team2Energy,
        state.team1_effort,
        state.team2_effort,
        leadingTeam,
        remainingTime,
        (state.round_wins[0] * 100) + (state.round_wins[1] * 100)
    );

    // Force redraw
    glutPostRedisplay();
}

// Main game loop (called by GLUT timer)
void game_loop(int value) {
  if (gameOver) {
        celebrationTimer -= 0.1f; // Decrease timer
        if (celebrationTimer <= 0) {
            cleanup();
            exit(0);
        }
        glutPostRedisplay();
        glutTimerFunc(100, game_loop, 0); // Faster updates for celebration
        return;
    }
    static time_t start_time = 0;
    if (start_time == 0) {
        start_time = time(NULL);
    }
    
    // Check game time
    if (time(NULL) - start_time >= GAME_TIME) {
        printf("Game over! Time limit reached.\n");
        
        if (state.round_wins[0] > state.round_wins[1]) {
            printf("Team 1 wins with %d rounds!\n", state.round_wins[0]);
        } else if (state.round_wins[1] > state.round_wins[0]) {
            printf("Team 2 wins with %d rounds!\n", state.round_wins[1]);
        } else {
            printf("The game is a tie!\n");
        }
        
        cleanup();
        gameOver = true;
    }
    
    // Update game state
    calculate_team_effort();
    referee_decision();
    
    int effortDiff = state.team1_effort - state.team2_effort;
    float targetDisplacement = (float)effortDiff / (float)WIN_THRESHOLD * MAX_ROPE_DISPLACEMENT;
    
    // Smooth the displacement change
    ropeDisplacement += (targetDisplacement - ropeDisplacement) * 0.1f;
    
    
    update_gui_data();
    
    // Continue the game loop
    glutTimerFunc(1000, game_loop, 0);
}

// Main display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!gameOver) {
        // Draw normal game view
        drawSky();
        drawBridge();
        drawSun();
        drawGrass();
        drawTeamInfoDisplay(
            state.effort_levels,                    // Team 1 energies
            state.effort_levels + PLAYERS_PER_TEAM, // Team 2 energies 
            state.team1_effort,                     // Team 1 total effort
            state.team2_effort,                     // Team 2 total effort
            (state.team1_effort > state.team2_effort) ? "Team 1" : 
            (state.team2_effort > state.team1_effort) ? "Team 2" : "Tie",
            GAME_TIME - (time(NULL) - gameStartTime), // Remaining time
            (state.round_wins[0] * 100) + (state.round_wins[1] * 100) // Score
        );

        // Draw trees
        drawTree(-0.8f, -0.5f);
        drawTree(0.8f, -0.5f);
        drawTree(0.5f, -0.5f);
        drawTree(0.3f, -0.5f);
        drawTree(0.0f, -0.5f);
        drawTree(-0.3f, -0.5f);
        drawTree(-0.5f, -0.5f);

        // Variables to store hand positions
        float leftTeamHands[PLAYERS_PER_TEAM][2];
        float rightTeamHands[PLAYERS_PER_TEAM][2];
        
       
// Draw left team (red) - strongest at the end (rightmost position)
float teamASpacing = 0.15f;
float teamAStartX = -0.8f;
for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
    // Reverse the drawing order (3 2 1 0)
    int drawIndex = PLAYERS_PER_TEAM - 1 - i;
    float x = teamAStartX + drawIndex * teamASpacing;
    
    if (players[i].energy <= 0 || players[i].is_fallen) {
        drawHumanPlayer(x, -0.1f, 1.0f, 0.0f, 0.0f, 1.0f, i,
                      &leftTeamHands[drawIndex][0], &leftTeamHands[drawIndex][1], true);
    } else {
        drawHumanPlayer(x, -0.1f, 1.0f, 0.0f, 0.0f, 1.0f, i,
                      &leftTeamHands[drawIndex][0], &leftTeamHands[drawIndex][1], true);
    }
}

// Draw right team (blue) - strongest at the end (leftmost position)
float teamBSpacing = 0.15f;
float teamBStartX = 0.5f;
for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
    int playerIdx = i + PLAYERS_PER_TEAM;
    // Keep normal order (0 1 2 3) for right team
    float x = teamBStartX + i * teamBSpacing;
    
    if (players[playerIdx].energy <= 0 || players[playerIdx].is_fallen) {
        drawHumanPlayer(x, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f, i,
                      &rightTeamHands[i][0], &rightTeamHands[i][1], false);
    } else {
        drawHumanPlayer(x, -0.1f, 0.0f, 0.0f, 1.0f, 1.0f, i,
                      &rightTeamHands[i][0], &rightTeamHands[i][1], false);
    }
}

        // Draw rope
        drawRope(leftTeamHands, rightTeamHands, PLAYERS_PER_TEAM);

        // Update referee flag animation
        waveAngle += waveDirection * waveSpeed;
        if (waveAngle > maxAngle) {
            waveDirection = -1;
        } else if (waveAngle < -maxAngle) {
            waveDirection = 1;
        }
        drawRefereeWithFlag(0.0f, 0.2f, waveAngle);

        // Show start message 
        if (gameStartTimer > 0) {
            drawGameStartMessage();
            gameStartTimer -= 0.016f;
        }
    } else {
     
        //  draw the celebration overlay
        drawCelebration();
        
    }

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    // Initialize game
    initialize_game();
    
    // Set up signal handlers
    signal(SIGCHLD, handle_sigchld);
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    
    // Start players
    start_players();
    
    // Initialize GUI
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tug of War");
    glClearColor(0.86f, 0.82f, 1.0f, 1.0f);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    
    // Start game loop
    glutTimerFunc(1000, game_loop, 0);
    
    // Start GLUT main loop
    glutMainLoop();
    
    return 0;
}
void notify_teams(int winner) {
    char msg[10];
    sprintf(msg, "%s", (winner == 1) ? "WIN" : "LOSE");
    
    // Send notification to all players through their pipes
    for (int i = 0; i < NUM_PLAYERS; i++) {
        // Determine if this player is on the winning team
        int is_winner = ((i < PLAYERS_PER_TEAM && winner == 1) || 
                        (i >= PLAYERS_PER_TEAM && winner == 2));
        
        char full_msg[20];
        snprintf(full_msg, sizeof(full_msg), "%s %d", 
                is_winner ? "WIN" : "LOSE", 
                state.current_round);
        
        // Write to the players pipe
        write(state.pipes[i][1], full_msg, strlen(full_msg) + 1);
        
        printf("Notified player %d: %s\n", i, full_msg);
    }
}
