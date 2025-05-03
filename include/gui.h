#ifndef GUI_H
#define GUI_H

#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define PLAYERS_PER_TEAM 4
// Global variables
extern float scaleFactor;
extern float baseBridgeHeight;
// Game state variables
// Animation control
extern float waveAngle;
extern int waveDirection;
extern const float waveSpeed;
extern const float maxAngle;
extern float ropeDisplacement;

// Game state variables
extern bool gameStarted;
extern float gameStartTimer;
extern const float GAME_START_DISPLAY_TIME;  // Declaration only
extern time_t gameStartTime;


void drawHumanPlayer(
    float x, float y, 
    float r, float g, float b, float alpha,
    int playerNumber, 
    float* handX, float* handY, 
    bool isLeftTeam
);
void drawRefereeWithFlag(float x, float y, float waveAngle);
void drawRope(float leftTeamHands[][2], float rightTeamHands[][2], int teamSize);
void drawTree(float x, float y);
void drawBridge();
void drawGameResult(const char* resultMessage);
void drawRoundedRectangle(float x, float y, float width, float height, float radius);
void drawSun();
void drawGrass();
void drawSky();
void drawText(float x, float y, const char* text);
void drawTeamInfoDisplay(int team1Energy[], int team2Energy[], int team1Effort, int team2Effort, const char* leadingTeam, int gameTime, int score);
void startGame(void);
void timer(int value);
void keyboard(unsigned char key, int x, int y);
void drawGameStartMessage(void);
void drawCelebration();
void drawTextCentered(float x, float y, const char* text, float scale);
int comparePlayers(const void* a, const void* b);
#endif
