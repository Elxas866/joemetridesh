#include "raylib.h"
#include <iostream>
#include <vector>

int main() {
    InitWindow(800, 450, "Joe Metri Desh");
    SetTargetFPS(60);

    float velocityY = 0.0f;
    float enemySpeed = 5.0f; // speed of the enemy
    float g = 1.7f; // gravity

    float playerX = 100.0f; // initial position of the player
    float playerY = 20.0f; // initial position of the player
    float playerSize = 40.0f; // size of the player
    float playerRotation = 0.0f; // rotation of the player
    float playerRotationSpeed = 5.0f; // rotation speed of the player
    
    float initialEnemyX = 750.0f; // initial position of the first enemy
    std::vector<float> enemyXPositions = { initialEnemyX }; // initial position of the first enemy
    float enemyWidth = 30.0f; // width of the enemy
    float enemyHeight = 30.0f; // height of the enemy
    int enemyCount = 1; // number of enemies
    int maxEnemies = 5; // maximum number of enemies

    float groundLevel = 380.0f; // ground level
    bool grounded = false;
    bool gameOver = false;

    while (!WindowShouldClose()) {
        // 1. Update: Logik, Input, Physik berechnen
        velocityY += g;

        playerY += velocityY;
        for (float& x : enemyXPositions) {
            x -= enemySpeed; // move enemy to the left
        }

        if (playerY + playerSize + velocityY >= groundLevel) {
            grounded = true;
            velocityY = 0;
            playerY = groundLevel - playerSize; // reset player position to ground level
        }

        if (IsKeyPressed(KEY_SPACE) && grounded) {
            velocityY = -25; // jump velocity
            grounded = false;
        }

        if (!grounded) {
            playerRotation += playerRotationSpeed; // rotate the player while in the air
        } else {
            playerRotation = 0; // reset rotation when grounded
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

        BeginDrawing();
        ClearBackground(SKYBLUE);
        // 2. Draw: alles zeichnen

        if (gameOver) { 
            DrawText("Game Over!", 300, 200, 40, RED);
            EndDrawing();

            WaitTime(3); // Wait for 3 seconds before closing the window
            CloseWindow();
            return 0; // Exit the game loop
        }

        // ground
        DrawRectangle(0, groundLevel, 800, 450 - groundLevel, BLUE);

        // player
        DrawRectanglePro({playerX + playerSize / 2, playerY + playerSize / 2, playerSize, playerSize}, {playerSize / 2, playerSize / 2}, playerRotation, VIOLET);

        // enemies
        for (float enemyX : enemyXPositions) {
            DrawTriangle({ enemyX + enemyWidth, groundLevel }, { enemyX + enemyWidth / 2, groundLevel - enemyHeight }, { enemyX, groundLevel }, PINK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}