 #include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define NUM_PINS 10
#define THROWS_PER_ROUND 3

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    bool fallen;
    bool animating;
} Pin;

void SetupPins(Pin pins[], int screenWidth, int round) {
    int pinIndex = 0;
    if (round == 1) {
        // Triangle pattern
        for (int row = 0; row < 4; row++) {
            for (int i = 0; i <= row; i++) {
                if (pinIndex < NUM_PINS) {
                    pins[pinIndex].position.x = screenWidth / 2 + (i - row / 2.0f) * 35;
                    pins[pinIndex].position.y = 120 + row * 35;
                    pins[pinIndex].fallen = false;
                    pins[pinIndex].animating = false;
                    pins[pinIndex].velocity = (Vector2){0, 0};
                    pins[pinIndex].rotation = 0;
                    pinIndex++;
                }
            }
        }
    } else if (round == 2) {
        // Diamond pattern
        Vector2 start = {screenWidth / 2, 150};
        int offsets[NUM_PINS][2] = {
            {0,0}, {-35,35}, {35,35}, {-70,70}, {0,70},
            {70,70}, {-35,105}, {35,105}, {0,140}, {0,175}
        };
        for (int i = 0; i < NUM_PINS; i++) {
            pins[i].position.x = start.x + offsets[i][0];
            pins[i].position.y = start.y + offsets[i][1];
            pins[i].fallen = false;
            pins[i].animating = false;
            pins[i].velocity = (Vector2){0, 0};
            pins[i].rotation = 0;
        }
    } else {
        // Circle pattern
        Vector2 center = {screenWidth / 2, 200};
        float radius = 70;
        for (int i = 0; i < NUM_PINS; i++) {
            float angle = (2 * PI / NUM_PINS) * i;
            pins[i].position.x = center.x + radius * cosf(angle);
            pins[i].position.y = center.y + radius * sinf(angle);
            pins[i].fallen = false;
            pins[i].animating = false;
            pins[i].velocity = (Vector2){0, 0};
            pins[i].rotation = 0;
        }
    }
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "2D Bowling Game - Final Score Display");
    InitAudioDevice();

    Sound hitSound = LoadSound("strike.wav");
    Texture2D background = LoadTexture("background.png");

    Vector2 ballPos = { screenWidth / 2, screenHeight - 80 };
    bool ballThrown = false;
    float t = 0.0f;
    float throwAngle = 0.0f;
    const float maxAngle = PI / 6;

    Vector2 ellipseCenter = { screenWidth / 2, screenHeight + 50 };
    float a = 100;
    float b = 500;
    float baseSpeed = 0.02f;
    float ballSpeed = baseSpeed;

    float power = 0.0f;
    float maxPower = 1.0f;
    bool charging = false;

    int currentRound = 1;
    int throwsLeft = THROWS_PER_ROUND;
    int score = 0;
    int totalScore = 0;

    bool straightThrow = false;

    bool showFinalScore = false;
    double finalScoreStartTime = 0.0;

    Pin pins[NUM_PINS];
    SetupPins(pins, screenWidth, currentRound);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (!showFinalScore) {
            if (IsKeyPressed(KEY_L)) straightThrow = !straightThrow;

            if (!ballThrown) {
                if (IsKeyDown(KEY_LEFT)) throwAngle -= 0.02f;
                if (IsKeyDown(KEY_RIGHT)) throwAngle += 0.02f;
                if (throwAngle > maxAngle) throwAngle = maxAngle;
                if (throwAngle < -maxAngle) throwAngle = -maxAngle;
            }

            if (IsKeyDown(KEY_SPACE) && !ballThrown) {
                charging = true;
                power += 0.01f;
                if (power > maxPower) power = maxPower;
            }

            if (IsKeyReleased(KEY_SPACE) && charging) {
                ballThrown = true;
                charging = false;
                t = 0.0f;
                ballSpeed = baseSpeed + power * 0.05f;
            }

            if (ballThrown) {
                t += ballSpeed;

                if (straightThrow) {
                    ballPos.x += sinf(throwAngle) * 8.0f;
                    ballPos.y -= cosf(throwAngle) * 8.0f;
                } else {
                    float x = a * cos(t);
                    float y = b * sin(t);
                    ballPos.x = ellipseCenter.x + x * cos(throwAngle) - y * sin(throwAngle);
                    ballPos.y = ellipseCenter.y - x * sin(throwAngle) - y * cos(throwAngle);
                }

                for (int i = 0; i < NUM_PINS; i++) {
                    if (!pins[i].fallen && CheckCollisionCircles(ballPos, 15, pins[i].position, 20)) {
                        pins[i].fallen = true;
                        score++;
                        totalScore++;
                        pins[i].animating = true;
                        pins[i].velocity = (Vector2){GetRandomValue(-5, 5), GetRandomValue(5, 10)};
                        pins[i].rotation = GetRandomValue(0, 360);
                        PlaySound(hitSound);
                    }
                }

                if ((straightThrow && (ballPos.y < 50 || ballPos.x < 0 || ballPos.x > screenWidth)) || (!straightThrow && t >= PI / 2)) {
                    ballThrown = false;
                    power = 0.0f;
                    throwsLeft--;
                    ballPos = (Vector2){screenWidth / 2, screenHeight - 80};

                    bool allFallen = true;
                    for (int i = 0; i < NUM_PINS; i++) {
                        if (!pins[i].fallen) { allFallen = false; break; }
                    }

                    if (throwsLeft <= 0 || allFallen) {
                        if (currentRound == 3) {
                            // Show final score screen
                            showFinalScore = true;
                            finalScoreStartTime = GetTime();
                        } else {
                            currentRound++;
                            SetupPins(pins, screenWidth, currentRound);
                            throwsLeft = THROWS_PER_ROUND;
                            score = 0;
                            ballPos = (Vector2){screenWidth / 2, screenHeight - 80};
                        }
                    }
                }
            }
        } else {
            // Show final score for 3 seconds then reset game
            if (GetTime() - finalScoreStartTime > 3.0) {
                currentRound = 1;
                throwsLeft = THROWS_PER_ROUND;
                score = 0;
                totalScore = 0;
                SetupPins(pins, screenWidth, currentRound);
                ballPos = (Vector2){screenWidth / 2, screenHeight - 80};
                showFinalScore = false;
            }
        }

        for (int i = 0; i < NUM_PINS; i++) {
            if (pins[i].animating) {
                pins[i].position.x += pins[i].velocity.x;
                pins[i].position.y += pins[i].velocity.y;
                pins[i].velocity.y += 0.3f;
                pins[i].rotation += 10;
                if (pins[i].position.y > screenHeight + 50)
                    pins[i].animating = false;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(background, (Rectangle){0, 0, background.width, background.height},
                       (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < NUM_PINS; i++) {
            if (!pins[i].fallen || pins[i].animating) {
                DrawCircleV(pins[i].position, 20, WHITE);
                DrawCircleV(pins[i].position, 8, RED);
            }
        }

        DrawCircleV(ballPos, 15, BLUE);

        if (!ballThrown && !showFinalScore) {
            Vector2 guideEnd = {ballPos.x + 50 * sinf(throwAngle), ballPos.y - 50 * cosf(throwAngle)};
            DrawLineEx(ballPos, guideEnd, 2, straightThrow ? RED : DARKBLUE);
        }

        if (charging) {
            DrawRectangle(50, screenHeight - 40, (int)(200 * (power / maxPower)), 20, GREEN);
            DrawRectangleLines(50, screenHeight - 40, 200, 20, BLACK);
        }

        // Draw current score and info always (except during final score display)
        if (!showFinalScore) {
            DrawText(TextFormat("Round: %d", currentRound), 10, 10, 20, BLACK);
            DrawText(TextFormat("Throws Left: %d", throwsLeft), 10, 35, 20, BLACK);
            DrawText(TextFormat("Pins Knocked: %d", score), 10, 60, 20, BLACK);
            DrawText(TextFormat("Total Score: %d", totalScore), 10, 85, 20, BLACK);
        }

        // Final score display
        if (showFinalScore) {
            // Dark overlay
            DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 150});
            // Large final score text centered
            const char *finalScoreText = TextFormat("FINAL SCORE: %d", totalScore);
            int fontSize = 60;
            int textWidth = MeasureText(finalScoreText, fontSize);
            DrawText(finalScoreText, (screenWidth - textWidth) / 2, screenHeight / 2 - fontSize / 2, fontSize, YELLOW);
        }

        EndDrawing();
    }

    UnloadTexture(background);
    UnloadSound(hitSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}


