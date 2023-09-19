
#include "raylib.h"
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include "types.hpp"






int main(void)
{
    // Setup
    Interaction testInteraction = Interaction();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "raylib [core] example - keyboard input");
    SetTargetFPS(60);

    // Set exit key to some key that doesn't exist
    SetExitKey(2000000000);

    init();

    area::initializeAreaContents();


    TextButton testButton(10,10,50,50,"Hello!");
    FocusableEntity testEntity;
    Interaction* activeInteraction;
    Player player = Player("Xyno");
    activePlayer = &player;
    activeEntity = activePlayer;

    area::change("plains", (Vector2){150, 150});
    Texture2D foreground = LoadTexture("resources/foreground.png");
    Image back = LoadImage("resources/plains.png");
    Color *colors = LoadImageColors(back);

    Image foreground2 = LoadImage("resources/foreground.png");
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

        
        // Testing features
        if (IsKeyPressed(KEY_Q))
        {
            changeActiveEntity(&testEntity);
        }
        if (IsKeyPressed(KEY_W))
        {
            changeActiveEntity(activePlayer);
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
        if (IsKeyPressed(KEY_Y))
        {
            area::change("city", (Vector2){50, 50});
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

        // Update movement of active player
        activePlayer->updateMovement();

        // Update camera position to player position
        activeEntity->updateCamera();
        
        // Rendering
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        BeginMode2D(activeEntity->camera);
        area::draw();
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
        std::string text_string = std::to_string(GetMousePosition(activeEntity->camera).x) + " " + std::to_string(GetMousePosition(activeEntity->camera).y);
        DrawText(text_string.c_str(), 10, 70, 20, RED);
        std::string text_string2 = std::to_string(GetMouseFixedPosition().x) + " " + std::to_string(GetMouseFixedPosition().y);
        DrawText(text_string2.c_str(), 10, 90, 20, RED);
        DrawText(area::name.c_str(), 100, 10, 20, ORANGE);
        if (GetMousePosition(activeEntity->camera).x >= 0 && GetMousePosition(activeEntity->camera).x < area::background.width &&
                GetMousePosition(activeEntity->camera).y >= 0 && GetMousePosition(activeEntity->camera).y < area::background.height)
            {
                int index = ((int)GetMousePosition(activeEntity->camera).y * area::background.width) + (int)GetMousePosition(activeEntity->camera).x;
                Color pixelColor = colors[index];
                std::string vak = ("Pixel Color: R=" + std::to_string(pixelColor.r) + ", G=" + std::to_string(pixelColor.g) + ", B=" + std::to_string(pixelColor.b) + ", A=" + std::to_string(pixelColor.a));
                DrawText(vak.c_str(), 180, 20, 20, BLUE);
            }
        EndDrawing();
    }

    UnloadTexture(foreground);
    CloseWindow();
    return 0;
}