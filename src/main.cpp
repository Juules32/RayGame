
#include "raylib.h"
#include <iostream>
#include "shorthands.hpp"


struct Player {
    Vector2 pos = {0,0};
    Vector2 v = {0,0};
};


int main(void)
{

    // Setup
    int windowWidth = 800;
    int windowHeight = 450;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "raylib [core] example - keyboard input");
    
    Texture2D test = LoadTexture("resources/test.png");

    Camera2D playerCamera {(Vector2){0,0},(Vector2){0,0}, 0, 1};
    Player player;
    Vector2 windowPosition = GetWindowPosition();
    
    int maxMonitorWidth, maxMonitorHeight;
    
    for (int i = 0; i < GetMonitorCount(); i++)
    {
        if (GetMonitorWidth(i) > maxMonitorWidth) maxMonitorWidth = GetMonitorWidth(i);
        if (GetMonitorHeight(i) > maxMonitorHeight) maxMonitorHeight = GetMonitorHeight(i);
    }

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // Event handling
        if(IsKeyPressed(KEY_F)) {
            if(!IsWindowFullscreen()) {
                MaximizeWindow();
                ToggleFullscreen();

            }
            else {
                ToggleFullscreen();
                SetWindowSize(windowWidth, windowHeight);
                SetWindowPosition(windowPosition.x, windowPosition.y);
            }
        }
        if(IsKeyDown(KEY_RIGHT)) {
            player.v.x = 5;
        }
        if(IsKeyDown(KEY_LEFT)) {
            player.v.x = -5;
        }
        if(IsKeyDown(KEY_UP)) {
            player.v.y = -5;
        }
        if(IsKeyDown(KEY_DOWN)) {
            player.v.y = 5;
        }

        if(IsWindowResized()) {
            std::cout << "wa" << std::endl;
            windowWidth = GetScreenWidth();
            windowHeight = GetScreenHeight();
            if(windowWidth > maxMonitorWidth) windowWidth = maxMonitorWidth;
            if(windowHeight > maxMonitorHeight) windowHeight = maxMonitorHeight;
            SetWindowSize(windowWidth, windowHeight);
        }

        // Physics
        player.pos.x += player.v.x;
        player.pos.y += player.v.y;
        player.v.x = player.v.x * 0.8;
        player.v.y = player.v.y * 0.8;

        // Update camera position to player position
        playerCamera.offset.x = windowWidth / 2;
        playerCamera.offset.y = windowHeight / 2;
        playerCamera.target = player.pos;
        playerCamera.zoom += 0.01;
        playerCamera.rotation += 1;
        
        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(playerCamera);
        DrawTexture(test,0,0, WHITE);
        DrawRectangle(player.pos.x - PLAYERWIDTH,player.pos.y - PLAYERHEIGHT,PLAYERWIDTH*2,PLAYERHEIGHT*2,BLUE);
        EndMode2D();

        DrawRectangle(0, 0, maxMonitorWidth, maxMonitorHeight, NIGHT);


        // Debugging information
        DrawFPS(10,10);

        EndDrawing();
    }

    UnloadTexture(test);
    CloseWindow();
    return 0;
}