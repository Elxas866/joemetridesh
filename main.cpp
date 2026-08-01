#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>
#include "gameMode.h"
#include "enemy.h"

int main() {
    InitWindow(800, 450, "Joe Metri Desh");
    InitAudioDevice();
    SetTargetFPS(60);

    Music bgMusic = LoadMusicStream("1600007_Framb039s-Kitchen.mp3");
    PlayMusicStream(bgMusic);

    int score = 0;
    GameMode gameMode = GameMode::CUBE; // default game mode

    float groundLevel = 380.0f; // ground level
    bool grounded = false;
    bool gameOver = false;

    float velocityY = 0.0f;
    float enemySpeed = 5.0f; // speed of the enemy
    float g = 1.7f; // gravity
    float thrust = 1.3f; // thrust for ship mode

    float playerX = 100.0f; // initial position of the player
    float playerY = 20.0f; // initial position of the player
    float playerWidth = 40.0f; // width of the player
    float playerHeight = 40.0f; // height of the player
    float scaleX = 1.0f; // scale factor for the player in the x direction
    float scaleY = 1.0f; // scale factor for the player in the y direction
    float tiltAngle = 0.0f; // tilt angle of the player
    
    Enemy initialEnemy = { 800.0f, groundLevel, 30.0f, 30.0f, EnemyType::SPIKE }; // initial position of the first enemy
    std::vector<Enemy> enemies = { initialEnemy }; // initial position of the first enemy
    int enemyCount = 1; // number of enemies
    int maxEnemies = 5; // maximum number of enemies
    int portalCount = 0; // number of portals

    Rectangle restartButton = { 300, 250, 200, 50 };
    Rectangle quitButton = { 300, 320, 200, 50 };

    while (!WindowShouldClose()) {
        // 1. Update: Logik, Input, Physik berechnen
        UpdateMusicStream(bgMusic);

        velocityY += g;
        playerY += velocityY;
        tiltAngle += (20.0f - tiltAngle) * 0.1f; // gradually return to 0 degrees

        scaleX += (1.0f - scaleX) * 0.2f;
        scaleY += (1.0f - scaleY) * 0.2f;


        for (Enemy& enemy : enemies) {
            enemy.x -= enemySpeed; // move enemy to the left
        }

        if (playerY + playerHeight + velocityY >= groundLevel) {
            velocityY = 0;
            playerY = groundLevel - playerHeight; // reset player position to ground level
            if (!grounded) {
                scaleX = 1.3f;
                scaleY = 0.7f;
            }
            grounded = true;
        }

        if (gameMode == GameMode::CUBE) {
            if (IsKeyPressed(KEY_SPACE) && grounded) {
                velocityY = -25; // jump velocity
                scaleX = 0.3f; // scale down the player in the x direction
                scaleY = 1.7f; // scale up the player in the y direction
                grounded = false;
            }
        } else if (gameMode == GameMode::SHIP) {
            if (IsKeyDown(KEY_SPACE)) {
                tiltAngle = -20.0f; // tilt the player upwards
                velocityY -= thrust;
                scaleX = 1.0f; // reset scale in the x direction
                scaleY = 1.0f; // reset scale in the y direction
            }
        }


        if (enemies.front().x < -initialEnemy.width) { // if the first enemy goes off screen
            enemies.erase(enemies.begin()); // remove the first enemy if it goes off screen
            enemyCount--;

            if (enemies.front().type == EnemyType::SHIP_PORTAL || enemies.front().type == EnemyType::CUBE_PORTAL) {
                portalCount--; // decrease portal count if a portal goes off screen
            }
        }

        while (enemyCount < maxEnemies && enemies.back().x < 600 && GetRandomValue(0, 100) < 5) { // chance to spawn a new enemy
            Enemy newEnemy;
            switch (gameMode) {
                case GameMode::CUBE:
                    newEnemy = { 800.0f, groundLevel, 30.0f, 30.0f, EnemyType::SPIKE };
                    break;
                case GameMode::SHIP:
                    newEnemy = { 800.0f, (float) GetRandomValue(0, groundLevel), 30.0f, 30.0f, EnemyType::CUBE };
                    break;
                default:
                    newEnemy = { 800.0f, groundLevel, 30.0f, 30.0f, EnemyType::SPIKE };
                    break;
            }
            enemies.push_back(newEnemy); // add a new enemy at the right edge
            enemyCount++;
            score += 10 + GetRandomValue(0, 10);
        }

        // spawn portals
        if (portalCount < 1 && GetRandomValue(0, 1000) < 1) { // chance to spawn a portal
            Enemy newEnemy;
            switch (gameMode) {
                case GameMode::CUBE:
                    newEnemy = { 800.0f, groundLevel, 30.0f, 50.0f, EnemyType::SHIP_PORTAL };
                    break;
                case GameMode::SHIP:
                    newEnemy = { 800.0f, (float) GetRandomValue(0, groundLevel), 30.0f, 50.0f, EnemyType::CUBE_PORTAL };
                    break;
                default:
                    newEnemy = { 800.0f, groundLevel, 30.0f, 50.0f, EnemyType::SHIP_PORTAL };
                    break;
            }
            enemies.push_back(newEnemy); // add a new portal at the right edge
            enemyCount++;
            portalCount++;
        }

        
        for (const Enemy& enemy : enemies) {
            switch (enemy.type) {
                case EnemyType::SPIKE:
                    if (CheckCollisionRecs({ playerX, playerY, playerWidth, playerHeight }, { enemy.x, enemy.y - enemy.height, enemy.width, enemy.height })) {
                        // Collision detected
                        gameOver = true;
                        StopMusicStream(bgMusic);
                        continue; // Skip further checks if game is over
                    }
                    break;
                case EnemyType::CUBE:
                    if (CheckCollisionRecs({ playerX, playerY, playerWidth, playerHeight }, { enemy.x, enemy.y, enemy.width, -enemy.height })) {
                        // Collision detected
                        gameOver = true;
                        StopMusicStream(bgMusic);
                        continue; // Skip further checks if game is over
                    }
                    break;
                case EnemyType::SHIP_PORTAL:
                    if (CheckCollisionRecs({ playerX, playerY, playerWidth, playerHeight }, { enemy.x, enemy.y - enemy.height, enemy.width, enemy.height })) {
                        // Collision detected
                        gameMode = GameMode::SHIP; // switch to ship mode
                        g = 0.5f; // set gravity for ship mode
                        continue; // Skip further checks if game mode changed
                    }
                    break;
                case EnemyType::CUBE_PORTAL:
                    if (CheckCollisionRecs({ playerX, playerY, playerWidth, playerHeight }, { enemy.x, enemy.y - enemy.height, enemy.width, enemy.height })) {
                        // Collision detected
                        gameMode = GameMode::CUBE; // switch to cube mode
                        g = 1.7f; // reset gravity for cube mode
                        continue; // Skip further checks if game mode changed
                    }
                    break;
            }
        }

        if (gameOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, restartButton)) {
                // Restart the game
                gameOver = false;
                gameMode = GameMode::CUBE; // reset to default game mode
                g = 1.7f; // reset gravity for cube mode
                playerX = 100.0f;
                playerY = 20.0f;
                velocityY = 0.0f;
                enemies.clear();
                enemies.push_back(initialEnemy);
                enemyCount = 1;
                scaleX = 1.0f;
                scaleY = 1.0f;

                score = 0; // reset score
                enemySpeed = 5.0f; // reset enemy speed

                PlayMusicStream(bgMusic); // restart background music
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

        // score
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, ORANGE);

        // ground
        DrawRectangle(0, groundLevel, 800, 450 - groundLevel, BLUE);

        // player
        switch (gameMode) {
            case GameMode::CUBE:
                if (playerWidth > 40) {
                    playerWidth = 40; // reset player width to original size
                }
                DrawRectanglePro({playerX + playerWidth / 2, playerY + playerHeight / 2, playerWidth * scaleX, playerHeight * scaleY}, {playerWidth * scaleX / 2, playerHeight * scaleY / 2}, 0, VIOLET);
                break;
            case GameMode::SHIP:
                if (playerWidth == 40) {
                    playerWidth = playerWidth * 2; // change width for ship mode
                }
                DrawRectanglePro({playerX + playerWidth / 2, playerY + playerHeight / 2, playerWidth * scaleX, playerHeight * scaleY}, {playerWidth * scaleX / 2, playerHeight * scaleY / 2}, tiltAngle, VIOLET);
                break;
        }

        // enemies
        for (const Enemy& enemy : enemies) {
            switch (enemy.type) {
                case EnemyType::SPIKE:
                    DrawTriangle({ enemy.x + enemy.width, enemy.y }, { enemy.x + enemy.width / 2, enemy.y - enemy.height }, { enemy.x, enemy.y }, PINK);
                    break;
                case EnemyType::CUBE:
                    DrawRectangle(enemy.x, enemy.y, enemy.width, enemy.height, PINK);
                    break;
                case EnemyType::SHIP_PORTAL:
                    DrawEllipse(enemy.x + enemy.width / 2, enemy.y - enemy.height / 2, enemy.width / 2, enemy.height / 2, GREEN);
                    break;
                case EnemyType::CUBE_PORTAL:
                    DrawEllipse(enemy.x + enemy.width / 2, enemy.y - enemy.height / 2, enemy.width / 2, enemy.height / 2, YELLOW);
                    break;
            }

            //DrawRectangleLines(enemy.x, enemy.y, enemy.width, -enemy.height, BLACK); // show hitbox for debugging
        }

        EndDrawing();
    }

    UnloadMusicStream(bgMusic);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}