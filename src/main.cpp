
#include "raylib.h"
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include "types.hpp"

Interaction* activeInteraction;
bool isInteracting = false;

std::vector<bool*> flags;

void checkIfPlayerCanMove() {
    for (bool* menu: flags)
    {
        if(*menu) {
            playerCanMove = false;
            return;
        }
    }
    playerCanMove = true;
}

FocusableEntity testEntity;

TextButton testButton(10,10,50,50,"Hello!");

int main(void)
{
    flags.push_back(&settingsActive);
    flags.push_back(&isInteracting);
    // Setup
    Interaction testInteraction = Interaction();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "raylib [core] example - keyboard input");

    // Set exit key to some key that doesn't exist
    SetExitKey(2000000000);

    init();

    Player player = Player("Xyno");
    activeEntity = &player;

    Texture2D background = LoadTexture("resources/background.png");
    Texture2D foreground = LoadTexture("resources/foreground.png");

    player.camera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};
    player.zoom = SCALEFACTOR;
    player.width = PLAYERWIDTH;
    player.height = PLAYERHEIGHT;

    Vector2 windowPosition = GetWindowPosition();

    int maxMonitorWidth, maxMonitorHeight;

    for (int i = 0; i < GetMonitorCount(); i++)
    {
        if (GetMonitorWidth(i) > maxMonitorWidth)
            maxMonitorWidth = GetMonitorWidth(i);
        if (GetMonitorHeight(i) > maxMonitorHeight)
            maxMonitorHeight = GetMonitorHeight(i);
    }

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        checkIfPlayerCanMove();
        // Event handling
        if (IsKeyPressed(KEY_F))
        {
            if (!IsWindowFullscreen())
            {
                MaximizeWindow();
                ToggleFullscreen();
            }
            else
            {
                ToggleFullscreen();
                SetWindowSize(windowWidth, windowHeight);
                SetWindowPosition(windowPosition.x, windowPosition.y);
            }
        }

        player.updateMovement();
        

        if (IsKeyPressed(KEY_Q))
        {
            FocusableEntity old_entity = *activeEntity;
            activeEntity = &testEntity;
            activeEntity->camera.target = old_entity.camera.target;
            targetZoom = activeEntity->zoom;
            activeEntity->camera.zoom = old_entity.camera.zoom;
        }
        if (IsKeyPressed(KEY_W))
        {
            FocusableEntity old_entity = *activeEntity;
            activeEntity = &player;
            activeEntity->camera.target = old_entity.camera.target;
            targetZoom = activeEntity->zoom;
            activeEntity->camera.zoom = old_entity.camera.zoom;
        }
        if (IsKeyPressed(KEY_E))
        {
            testButton.outputText();
        }
        if (IsKeyPressed(KEY_R))
        {
            activeEntity->startShake();
        }
        if (IsKeyPressed(KEY_T))
        {
            activeInteraction = &testInteraction;
            isInteracting = true;
        }
        if (IsKeyPressed(KEY_ESCAPE))
        {
            settingsActive = !settingsActive;
        }

        if (IsWindowResized())
        {
            windowWidth = GetScreenWidth();
            windowHeight = GetScreenHeight();
            if (windowWidth > maxMonitorWidth)
                windowWidth = maxMonitorWidth;
            if (windowHeight > maxMonitorHeight)
                windowHeight = maxMonitorHeight;
            SetWindowSize(windowWidth, windowHeight);
        }

        // Update camera position to player position
        activeEntity->updateCamera();
        
        // Rendering
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        BeginMode2D(activeEntity->camera);
        DrawTexture(background, 0, 0, WHITE);
        DrawTexture(foreground, 0, 0, BLUE);
        player.draw();
        DrawRectangle(50, 200, PLAYERWIDTH, PLAYERHEIGHT, YELLOW);
        EndMode2D();

        if(settingsActive) {
            BeginMode2D(fixedCamera);
            DrawText("hej", 60, 20, 10, RED);
            DrawRectangle(windowWidth / 2 / SCALEFACTOR, windowHeight / 2 / SCALEFACTOR, PLAYERWIDTH, PLAYERHEIGHT, ORANGE);
            testButton.draw();
            EndMode2D();
        }
        
        if(isInteracting) {
            if(!activeInteraction->iterate()) {
                isInteracting = false;
            }
        }

        std::string text_string3 = std::to_string(player.v.x) + " " + std::to_string(player.v.y);

        // Debugging information
        DrawFPS(10, 10);
        DrawText(text_string3.c_str(), 10, 50, 20, RED);
        std::string text_string = std::to_string(GetMouseGamePosition().x) + " " + std::to_string(GetMouseGamePosition().y);
        DrawText(text_string.c_str(), 10, 70, 20, RED);
        std::string text_string2 = std::to_string(GetMouseFixedPosition().x) + " " + std::to_string(GetMouseFixedPosition().y);
        DrawText(text_string2.c_str(), 10, 90, 20, RED);

        EndDrawing();
    }

    UnloadTexture(background);
    UnloadTexture(foreground);
    CloseWindow();
    return 0;
}