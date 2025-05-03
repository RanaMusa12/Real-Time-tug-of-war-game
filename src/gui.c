#include "gui.h"
#include "config.h"
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// External declarations
extern Player players[NUM_PLAYERS];
extern GameState state;
extern float ropeDisplacement;
// Global variables
float scaleFactor = 0.6f; // Scale down elements to increase spacing
float baseBridgeHeight = -0.15f;
time_t gameStartTime = 0;  // Add this initialization

// Function to draw text on the screen
void drawText(float x, float y, const char* text) {
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for text
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawTeamInfoDisplay(int team1Energy[], int team2Energy[], 
                        int team1Effort, int team2Effort,
                        const char* leadingTeam, 
                        int gameTime, int score) {
    float leftStartX = -0.95f;
    float rightStartX = 0.7f;
    float startY = 0.9f;
    float lineHeight = 0.05f;
      char roundText[50];
    sprintf(roundText, "Round: %d", state.current_round);
    drawText(-0.1f, 0.9f, roundText);
    startY -= lineHeight;
    char scoreText1[50];
    sprintf(scoreText1, "Score: %d - %d", state.round_wins[0], state.round_wins[1]);
    drawText(-0.1f, 0.8f, scoreText1);

    // Team 1 Energy
    drawText(leftStartX, startY, "Team 1 Players:");
    startY -= lineHeight;
    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        char energyText[50];
        sprintf(energyText, "P%d: %d Energy", i, team1Energy[i]);
        drawText(leftStartX, startY, energyText);
        startY -= lineHeight;
    }
	    // Game Info
    drawText(leftStartX, startY, "\n");
    startY -= lineHeight;
    // Team 1 Effort
    char effortText1[50];
    sprintf(effortText1, "Total Effort: %d", team1Effort);
    drawText(leftStartX, startY, effortText1);
    startY -= lineHeight;
    // Game Info
    drawText(leftStartX, startY, "----------------");
    startY -= lineHeight;
    // Team 2 Energy
    drawText(leftStartX, startY, "Team 2 Players:");
    startY -= lineHeight;
    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        char energyText[50];
        sprintf(energyText, "P%d: %d Energy", i, team2Energy[i]);
        drawText(leftStartX, startY, energyText);
        startY -= lineHeight;
    }

    // Team 2 Effort
    char effortText2[50];
    sprintf(effortText2, "Total Effort: %d", team2Effort);
    drawText(leftStartX, startY, effortText2);
    startY -= lineHeight;

    // Game Info
    drawText(leftStartX, startY, "----------------");
    startY -= lineHeight;
    
    char leaderText[50];
    sprintf(leaderText, "Leading: %s", leadingTeam);
    drawText(-0.1f, 0.7f, leaderText);
    startY -= lineHeight;

 char timeText[50];
sprintf(timeText, "Time: %.1f seconds", (float)GAME_TIME); // Cast to float
drawText(rightStartX, 0.5f, timeText);

    char scoreText[50];
    sprintf(scoreText, "Score: %d", max_score_to_exit);
    drawText(rightStartX, 0.5f - lineHeight, scoreText);

}







void drawGrass() {
    glColor3f(0.0f, 0.5f, 0.0f); // Set color to green

    // Draw a quad for the grass
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f); // Bottom-left corner
    glVertex2f(1.0f, -1.0f);  // Bottom-right corner
    glVertex2f(1.0f, -0.3f);  // Top-right corner (double the margin)
    glVertex2f(-1.0f, -0.3f); // Top-left corner (double the margin)
    glEnd();
}
void drawSky() {
    // Draw the sky using a gradient
    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.92f); // Light blue (top)
    glVertex2f(-1.0f, 1.0f);        // Top-left corner
    glVertex2f(1.0f, 1.0f);         // Top-right corner
    glColor3f(1.0f, 1.0f, 1.0f);    // White (bottom)
    glVertex2f(1.0f, -0.2f);        // Bottom-right corner
    glVertex2f(-1.0f, -0.2f);       // Bottom-left corner
    glEnd();
}


// Draw the main referee with a flag
void drawRefereeWithFlag(float x, float y, float waveAngle) {
    float refereeScale = 1.3f;
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for referee
    glPushMatrix();
    glTranslatef(x, y * scaleFactor, 0.0f);
    glScalef(scaleFactor * refereeScale, scaleFactor * refereeScale, scaleFactor);

    // Head
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    glutSolidSphere(0.06, 20, 20);

    // Eyes and mouth
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(-0.015f, 0.02f, 0.0f);
    glutSolidSphere(0.005, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.015f, 0.02f, 0.0f);
    glutSolidSphere(0.005, 10, 10);
    glPopMatrix();

    glBegin(GL_LINE_STRIP);
    for (float angle = 0; angle <= 180; angle += 10) {
        float rad = -1 * angle * (3.14159f / 180.0f);
        float x = 0.015f * cos(rad);
        float y = -0.01f + 0.01f * sin(rad);
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();

    // Body
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.09f);
    glVertex2f(0.0f, -0.08f);
    glEnd();

    // Arms
    glBegin(GL_LINES);
    glVertex2f(-0.08f, 0.05f);
    glVertex2f(0.0f, 0.05f);
    glVertex2f(0.0f, 0.05f);
    glVertex2f(0.08f, 0.07f);
    glEnd();

    // Flag
    glPushMatrix();
    glTranslatef(0.1f, 0.07f, 0.0f);
    glRotatef(waveAngle, 0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 0.0f, 0.0f); // Red flag
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.06f, 0.0f);
    glVertex2f(0.06f, 0.12f);
    glVertex2f(0.0f, 0.12f);
    glEnd();
    glPopMatrix();

    // Legs
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, -0.08f);
    glVertex2f(-0.06f, -0.2f);
    glVertex2f(0.0f, -0.08f);
    glVertex2f(0.06f, -0.2f);
    glEnd();

    // Base
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, -0.22f);
    glVertex2f(0.1f, -0.22f);
    glVertex2f(0.1f, -0.25f);
    glVertex2f(-0.1f, -0.25f);
    glEnd();

    // Title
    glColor3f(0, 0, 0);
    glRasterPos2f(-0.06f, 0.3f);
    char numStr[20];
    sprintf(numStr, "Referee");
    for (char *c = numStr; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }

    glPopMatrix();
}
void drawHumanPlayer(float x, float y, float r, float g, float b, float alpha, 
                    int playerId, float* handX, float* handY, bool isLeftTeam) {
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Calculate dynamic position offset based on rope displacement
    float positionOffset = ropeDisplacement * 0.2f;
    if (isLeftTeam) {
        x -= positionOffset; // Left team moves left when rope moves left
    } else {
        x += positionOffset; // Right team moves right when rope moves right
    }

    // Calculate dynamic lean angle based on rope displacement
    float leanAngle = ropeDisplacement * 30.0f;
    if (isLeftTeam) leanAngle *= -1.0f;

    // Get the actual player from the players array
    Player* player = &players[playerId];
    int energy = player->energy;
    int max_energy = ENERGY_MAX;
    
    glPushMatrix();
    glTranslatef(x, y * scaleFactor, 0.0f);
    glScalef(scaleFactor, scaleFactor, scaleFactor);

    // ================== DRAW ENERGY INFO ==================
    // Energy bar background (gray)
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(-0.06f, 0.25f);
    glVertex2f(0.06f, 0.25f);
    glVertex2f(0.06f, 0.27f);
    glVertex2f(-0.06f, 0.27f);
    glEnd();
    
    // Energy level (color changes based on energy)
    float energyPercent = (float)energy / max_energy;
    glColor3f(1.0f - energyPercent, energyPercent, 0.0f); // Red to green gradient
    glBegin(GL_QUADS);
    glVertex2f(-0.06f, 0.25f);
    glVertex2f(-0.06f + 0.12f * energyPercent, 0.25f);
    glVertex2f(-0.06f + 0.12f * energyPercent, 0.27f);
    glVertex2f(-0.06f, 0.27f);
    glEnd();
    
    // Energy text
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(-0.05f, 0.23f);
    char energyText[20];
    sprintf(energyText, "P%d: %d", playerId, energy);
    for (char* c = energyText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
    }

    // ================== DRAW PLAYER BODY ==================
    glColor4f(r, g, b, alpha);
    
    // Head with dynamic expression based on energy
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    glutSolidSphere(0.05, 20, 20);

    // Eyes
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(-0.015f, 0.02f, 0.0f);
    glutSolidSphere(0.005, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.015f, 0.02f, 0.0f);
    glutSolidSphere(0.005, 10, 10);
    glPopMatrix();

    // Mouth - changes expression based on energy
    glBegin(GL_LINE_STRIP);
    float mouthStart = energy > max_energy/2 ? 30.0f : 0.0f; // Smile if high energy
    float mouthEnd = energy > max_energy/2 ? 150.0f : 180.0f;
    for (float angle = mouthStart; angle <= mouthEnd; angle += 10) {
        float rad = -1 * angle * (3.14159f / 180.0f);
        float x = 0.015f * cos(rad);
        float y = -0.01f + 0.01f * sin(rad);
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();

    // Player number at feet 
    glColor4f(r, g, b, 1.0f);
    glRasterPos2f(-0.02f, -0.24f);
    char numStr[12];
    sprintf(numStr, "#%d", playerId);
    for (const char* c = numStr; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Body with dynamic leaning
    glPushMatrix();
    glRotatef(leanAngle, 0.0f, 0.0f, 1.0f);
    glColor4f(0, 0, 0, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.1f);
    glVertex2f(0.0f, -0.05f);
    glEnd();
    glPopMatrix();

    // Arms with dynamic tension
    glBegin(GL_LINES);
    if (isLeftTeam) {
        // Left team - right arm extended with tension
        float armTension = ropeDisplacement > 0 ? 0.03f : 0.0f;
        glVertex2f(0.0f, 0.05f);
        glVertex2f(0.08f + armTension, 0.05f);
        *handX = x + (0.08f + armTension) * scaleFactor;
    } else {
        // Right team - left arm extended with tension
        float armTension = ropeDisplacement < 0 ? 0.03f : 0.0f;
        glVertex2f(-0.08f - armTension, 0.05f);
        glVertex2f(0.0f, 0.05f);
        *handX = x - (0.08f + armTension) * scaleFactor;
    }
    *handY = y * scaleFactor + 0.05f * scaleFactor;
    glEnd();

    // Other arm (bent)
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.05f);
    glVertex2f(isLeftTeam ? -0.04f : 0.04f, 0.08f);
    glEnd();

    // Legs with dynamic stance based on pulling
    float legSpread = 0.05f + fabs(leanAngle)/300.0f;
    float backLegBend = fabs(leanAngle)/500.0f;
    
    glBegin(GL_LINES);
    // Back leg (more bent when pulling)
    glVertex2f(0.0f, -0.05f);
    glVertex2f(-legSpread, -0.15f - backLegBend);
    // Front leg
    glVertex2f(0.0f, -0.05f);
    glVertex2f(legSpread, -0.15f);
    glEnd();

    // Position indicator if at end of line or fallen
    if (energy == 0 || player->is_fallen) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red "X" for zero energy or fallen
        glBegin(GL_LINES);
        glVertex2f(-0.1f, 0.3f);
        glVertex2f(0.1f, 0.1f);
        glVertex2f(0.1f, 0.3f);
        glVertex2f(-0.1f, 0.1f);
        glEnd();
    }

    glPopMatrix();
    glDisable(GL_BLEND);
}
void drawRope(float leftTeamHands[][2], float rightTeamHands[][2], int teamSize) {
    glColor3f(0.5f, 0.35f, 0.05f); // Brown color for rope
    glLineWidth(5.0f);
    
    // Calculate center point displacement
    float centerX = (leftTeamHands[teamSize-1][0] + rightTeamHands[0][0]) / 2.0f + ropeDisplacement;
    float centerY = (leftTeamHands[teamSize-1][1] + rightTeamHands[0][1]) / 2.0f;
    
    // Draw left team rope to displaced center
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < teamSize; i++) {
        float offset = -0.02f * i; // Offset to the left for left team
        glVertex2f(leftTeamHands[i][0] + offset, leftTeamHands[i][1]);
    }
    // Add extra point at displaced center
    glVertex2f(centerX, centerY);
    glEnd();
    
    // Draw right team rope to displaced center
    glBegin(GL_LINE_STRIP);
    // Start from displaced center
    glVertex2f(centerX, centerY);
    for (int i = 0; i < teamSize; i++) {
        float offset = 0.02f * i; // Offset to the right for right team
        glVertex2f(rightTeamHands[i][0] + offset, rightTeamHands[i][1]);
    }
    glEnd();
    
    // Draw center marker
    glColor3f(1.0f, 0.0f, 0.0f); // Red center marker
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glVertex2f(0.0f, centerY); // Original center line
    glEnd();
}
// Draw a tree
void drawTree(float x, float y) {
    float scale = 0.5f;
    glColor3f(0.5f, 0.3f, 0.0f); // Brown trunk
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.04f, 0.0f);
    glVertex2f(0.04f, 0.0f);
    glVertex2f(0.03f, 0.2f);
    glVertex2f(-0.03f, 0.2f);
    glEnd();
    glPopMatrix();

    // Foliage
    glColor3f(0.0f, 0.6f, 0.1f); // Dark green
    glPushMatrix();
    glTranslatef(x, y + 0.25f * scale, 0.0f);
    glutSolidCone(0.12f * scale, 0.2f * scale, 20, 20);
    glPopMatrix();

    glColor3f(0.0f, 0.8f, 0.2f); // Light green
    glPushMatrix();
    glTranslatef(x, y + 0.4f * scale, 0.0f);
    glutSolidCone(0.09f * scale, 0.15f * scale, 20, 20);
    glPopMatrix();

    glColor3f(0.0f, 0.9f, 0.3f); // Bright green
    glPushMatrix();
    glTranslatef(x, y + 0.5f * scale, 0.0f);
    glutSolidCone(0.06f * scale, 0.1f * scale, 20, 20);
    glPopMatrix();
}

// Draw the bridge
void drawBridge() {
    // Draw the street (gray color)
    glColor3f(0.5f, 0.5f, 0.5f); // Gray color for the street
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, baseBridgeHeight * scaleFactor - 0.1f); // Bottom-left corner
    glVertex2f(1.0f, baseBridgeHeight * scaleFactor - 0.1f);  // Bottom-right corner
    glVertex2f(1.0f, baseBridgeHeight * scaleFactor);         // Top-right corner
    glVertex2f(-1.0f, baseBridgeHeight * scaleFactor);        // Top-left corner
    glEnd();

    // Draw the middle line (white color)
    glColor3f(1.0f, 1.0f, 1.0f); 
    glLineWidth(2.0f); 
    glBegin(GL_LINES);
    glVertex2f(-1.0f, baseBridgeHeight * scaleFactor - 0.05f); 
    glVertex2f(1.0f, baseBridgeHeight * scaleFactor - 0.05f);  
    glEnd();
    
    
       if (fabs(ropeDisplacement) > 0.01f) {
        glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
        if (ropeDisplacement > 0) {
            // Team 1 is pulling harder
            glVertex2f(0.0f, baseBridgeHeight * scaleFactor);
            glVertex2f(ropeDisplacement * 2.0f, baseBridgeHeight * scaleFactor);
            glVertex2f(ropeDisplacement * 2.0f, baseBridgeHeight * scaleFactor - 0.1f);
            glVertex2f(0.0f, baseBridgeHeight * scaleFactor - 0.1f);
        } else {
            // Team 2 is pulling harder
            glVertex2f(0.0f, baseBridgeHeight * scaleFactor);
            glVertex2f(ropeDisplacement * 2.0f, baseBridgeHeight * scaleFactor);
            glVertex2f(ropeDisplacement * 2.0f, baseBridgeHeight * scaleFactor - 0.1f);
            glVertex2f(0.0f, baseBridgeHeight * scaleFactor - 0.1f);
        }
        glEnd();
    }
}

#define _USE_MATH_DEFINES // Enable math constants


void drawSun() {
    const float sunRadius = 0.1f; // Radius of the sun
    const float sunX = 0.8f;     // X position of the sun
    const float sunY = 0.8f;     // Y position of the sun
    const int numSegments = 100; // Number of segments to draw the circle

    // Draw the sun (yellow circle)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(sunX, sunY); // Center of the circle
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * M_PI * (float)i / (float)numSegments; // Calculate the angle
        float x = sunRadius * cosf(angle); // X coordinate of the circle's edge
        float y = sunRadius * sinf(angle); // Y coordinate of the circle's edge
        glVertex2f(sunX + x, sunY + y);   // Draw the vertex
    }
    glEnd();

    // Draw sun rays (yellow lines extending outward)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
    glLineWidth(2.0f); // Set the line width
    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) { // Draw 12 rays
        float angle = 2.0f * M_PI * (float)i / 12.0f; // Angle for each ray
        float rayLength = sunRadius * 1.5f; // Length of the rays
        float endX = sunX + rayLength * cosf(angle); // Endpoint X of the ray
        float endY = sunY + rayLength * sinf(angle); // Endpoint Y of the ray
        glVertex2f(sunX, sunY); // Start of the ray (center of the sun)
        glVertex2f(endX, endY);  // End of the ray
    }
    glEnd();
}
// Draw the game result
void drawCelebration() {
    // Draw semi-transparent overlay
    
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
    glEnd();
    
    // Draw celebration text
    glColor3f(1.0f, 1.0f, 1.0f);
    
    if(state.round_wins[0] > state.round_wins[1]){
     drawTextCentered(0.0f, 0.3f, "Team 1 won !!!!!!!! ", 0.0008f);
    }
     else if(state.round_wins[0] < state.round_wins[1]){
     drawTextCentered(0.0f, 0.4f, "Team 2 won !!!!!!!! ", 0.0008f);
    }
    else {
        drawTextCentered(0.0f, 0.2f, "GAME OVER - IT'S A TIE!", 0.0008f);
    }
      
        
        // Draw confetti or other celebration elements
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 50; i++) {
            glColor3f((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
            glBegin(GL_QUADS);
                float x = -0.9f + 1.8f * (float)rand()/RAND_MAX;
                float y = -0.5f + 1.0f * (float)rand()/RAND_MAX;
                float size = 0.02f + 0.03f * (float)rand()/RAND_MAX;
                glVertex2f(x, y);
                glVertex2f(x+size, y);
                glVertex2f(x+size, y+size);
                glVertex2f(x, y+size);
            glEnd();
        }
        glPopMatrix();
    
    
    // Draw "Game Over" text
    drawTextCentered(0.0f, 0.0f, "GAME OVER", 0.001f);
    
    // Draw score
    char scoreText[50];
    sprintf(scoreText, "Final Score: %d - %d", state.round_wins[0], state.round_wins[1]);
    drawTextCentered(0.0f, -0.1f, scoreText, 0.0006f);
    
    // Draw countdown to exit
    char countdownText[30];
    sprintf(countdownText, "Closing in %.1f seconds...", celebrationTimer);
    drawTextCentered(0.0f, -0.2f, countdownText, 0.0005f);
}

// Draw a rounded rectangle
void drawRoundedRectangle(float x, float y, float width, float height, float radius) {
    glBegin(GL_POLYGON);

    // Top edge
    for (int i = 0; i <= 90; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + width / 2 - radius + radius * cos(angle), y + height / 2 - radius + radius * sin(angle));
    }

    // Right edge
    for (int i = 90; i <= 180; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + width / 2 - radius + radius * cos(angle), y - height / 2 + radius + radius * sin(angle));
    }

    // Bottom edge
    for (int i = 180; i <= 270; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x - width / 2 + radius + radius * cos(angle), y - height / 2 + radius + radius * sin(angle));
    }

    // Left edge
    for (int i = 270; i <= 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x - width / 2 + radius + radius * cos(angle), y + height / 2 - radius + radius * sin(angle));
    }

    glEnd();
}

void drawGameStartMessage() {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
    glLineWidth(3.0f);
    
    // Position text in center of screen
    glRasterPos2f(-0.3f, 0.5f);
    
    char* message = "GAME STARTS NOW!";
    for (char* c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void startGame() {
    gameStarted = true;
    gameStartTimer = GAME_START_DISPLAY_TIME;
    
}
void timer(int value) {
    if (!gameStarted) {
        startGame();
    }
    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0); // Check every second
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27:  // ESC key
            exit(0);
            break;
        case ' ':
            if (!gameStarted) {
                gameStarted = true;
                gameStartTimer = GAME_START_DISPLAY_TIME;
                startGame();  // Callgame start function
            }
            break;
    }
}
void drawTextCentered(float x, float y, const char* text, float scale) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawAudienceMember(float x, float y, float r, float g, float b, int hasFlag, int flagColor) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    
    // Head (circle)
    glColor3f(1.0f, 0.8f, 0.6f); // Skin color
    glBegin(GL_TRIANGLE_FAN);
    for(int i = 0; i < 360; i += 20) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.03f*cos(angle), 0.2f + 0.03f*sin(angle));
    }
    glEnd();

    // Body (single vertical line)
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.2f);  // Top (connects to head)
    glVertex2f(0.0f, 0.05f); // Bottom
    glEnd();

    // Arms (single line each)
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.15f); // Shoulder
    glVertex2f(-0.1f, 0.1f); // Left arm
    glVertex2f(0.0f, 0.15f); // Shoulder
    glVertex2f(0.1f, 0.1f);  // Right arm
    glEnd();

    // Legs (single line each)
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.05f); // Hip
    glVertex2f(-0.05f, -0.1f); // Left leg
    glVertex2f(0.0f, 0.05f);  // Hip
    glVertex2f(0.05f, -0.1f);  // Right leg
    glEnd();

    
    if(hasFlag) {
        // Flag pole
        glColor3f(0.5f, 0.35f, 0.05f); // Brown
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(0.1f, 0.1f); // Hand
        glVertex2f(0.1f, -0.05f); // Bottom of pole
        glEnd();
        
        // Flag
        if(flagColor == 0) { // Red flag
            glColor3f(1.0f, 0.0f, 0.0f);
        } else { // Blue flag
            glColor3f(0.0f, 0.0f, 1.0f);
        }
        glBegin(GL_TRIANGLES);
        glVertex2f(0.1f, 0.1f); // Pole top
        glVertex2f(0.1f, 0.0f); // Pole middle
        glVertex2f(0.2f, 0.05f); // Flag tip
        glEnd();
    }

    glPopMatrix();
}
void drawAudience() {
    float row1[8];
    for(int i = 0; i < 8; i++) {
        row1[i] = -0.9f + (1.8f * i) / 7.0f;
    }
    
    // Second row positions (7 members, offset between first row)
    float row2[7];
    for(int i = 0; i < 7; i++) {
        row2[i] = -0.85f + (1.7f * i) / 6.0f;
    }
    
    // Draw first row
    for(int i = 0; i < 8; i++) {
        int hasFlag = (i == 2); // Only first row has red flag
        float r = 0.8f - (0.6f * i/8.0f);  // More red on left
        float g = 0.2f;
        float b = 0.2f + (0.6f * i/8.0f); // More blue on right
        drawAudienceMember(row1[i], -0.7f, r, g, b, hasFlag, 0);
    }
    
    // Draw second row
    for(int i = 0; i < 7; i++) {
        int hasFlag = (i == 5); // Only second row has blue flag
        float r = 0.2f + (0.6f * (6-i)/6.0f);  // More red on left
        float g = 0.2f;
        float b = 0.8f - (0.6f * (6-i)/6.0f); // More blue on right
        drawAudienceMember(row2[i], -0.85f, r, g, b, hasFlag, 1);
    }
}
