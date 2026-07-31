#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    InitWindow(800, 450, "Joe Metri Desh");
    SetTargetFPS(60);

    float velocityY = 0.0f;
    float enemySpeed = 5.0f; // speed of the enemy
    float g = 1.7f; // gravity

    float playerX = 100.0f; // initial position of the player
    float playerY = 20.0f; // initial position of the player
    float playerSize = 40.0f; // size of the player
    float scaleX = 1.0f; // scale factor for the player in the x direction
    float scaleY = 1.0f; // scale factor for the player in the y direction
    
    float initialEnemyX = 750.0f; // initial position of the first enemy
    std::vector<float> enemyXPositions = { initialEnemyX }; // initial position of the first enemy
    float enemyWidth = 30.0f; // width of the enemy
    float enemyHeight = 30.0f; // height of the enemy
    int enemyCount = 1; // number of enemies
    int maxEnemies = 5; // maximum number of enemies

    float groundLevel = 380.0f; // ground level
    bool grounded = false;
    bool gameOver = false;

    Rectangle restartButton = { 300, 250, 200, 50 };
    Rectangle quitButton = { 300, 320, 200, 50 };

    while (!WindowShouldClose()) {
        // 1. Update: Logik, Input, Physik berechnen
        velocityY += g;
        playerY += velocityY;

        scaleX += (1.0f - scaleX) * 0.2f;
        scaleY += (1.0f - scaleY) * 0.2f;


        for (float& x : enemyXPositions) {
            x -= enemySpeed; // move enemy to the left
        }

        if (playerY + playerSize + velocityY >= groundLevel) {
            velocityY = 0;
            playerY = groundLevel - playerSize; // reset player position to ground level
            if (!grounded) {
                scaleX = 1.3f;
                scaleY = 0.7f;
            }
            grounded = true;
        }

        if (IsKeyPressed(KEY_SPACE) && grounded) {
            velocityY = -25; // jump velocity
            scaleX = 0.3f; // scale down the player in the x direction
            scaleY = 1.7f; // scale up the player in the y direction
            grounded = false;
        }

        if (enemyXPositions.front() < -enemyWidth) {
            enemyXPositions.erase(enemyXPositions.begin()); // remove the first enemy if it goes off screen
            enemyCount--;
        }

        while (enemyCount < maxEnemies && enemyXPositions.back() < 600 && GetRandomValue(0, 100) < 5) { // chance to spawn a new enemy
            enemyXPositions.push_back(initialEnemyX); // add a new enemy at the right edge
            enemyCount++;
        }

        
        for (float enemyX : enemyXPositions) {
            if (CheckCollisionRecs({ playerX, playerY, playerSize, playerSize }, { enemyX, groundLevel - enemyHeight, enemyWidth, enemyHeight })) {
                // Collision detected
                gameOver = true;
                continue; // Skip further checks if game is over
            }
        }

        if (gameOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, restartButton)) {
                // Restart the game
                gameOver = false;
                playerX = 100.0f;
                playerY = 20.0f;
                velocityY = 0.0f;
                enemyXPositions.clear();
                enemyXPositions.push_back(initialEnemyX);
                enemyCount = 1;
                scaleX = 1.0f;
                scaleY = 1.0f;

                enemySpeed = 5.0f; // reset enemy speed
            }

            if (CheckCollisionPointRec(mousePos, quitButton)) {
                // Quit the game
                CloseWindow();
                return 0;
            }
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);
        // 2. Draw: alles zeichnen

        if (gameOver) { 
            velocityY = 0; // stop the player from moving
            enemySpeed = 0; // stop the enemies from moving

            DrawText("Game Over!", 300, 200, 40, RED);
            
            // restart button
            DrawRectangleRec(restartButton, DARKGRAY);
            DrawText("Restart", 350, 260, 30, WHITE);

            // quit button
            DrawRectangleRec(quitButton, DARKGRAY);
            DrawText("Quit", 370, 330, 30, WHITE);
        }

        // ground
        DrawRectangle(0, groundLevel, 800, 450 - groundLevel, BLUE);

        // player
        DrawRectanglePro({playerX + playerSize / 2, playerY + playerSize / 2, playerSize * scaleX, playerSize * scaleY}, {playerSize * scaleX / 2, playerSize * scaleY / 2}, 0, VIOLET);

        // enemies
        for (float enemyX : enemyXPositions) {
            DrawTriangle({ enemyX + enemyWidth, groundLevel }, { enemyX + enemyWidth / 2, groundLevel - enemyHeight }, { enemyX, groundLevel }, PINK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}