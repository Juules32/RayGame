
#include "raylib.h"
#include <iostream>
#include "shorthands.hpp"
#include <memory>

const int SCALEFACTOR = 4;
const float MOVE_SPEED = 1.0;
const float FRICTION = 0.8;

struct Entity
{
    Vector2 pos = {0, 0};
};

struct FocusableEntity : Entity
{
    Camera2D camera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};
};

struct Player : FocusableEntity
{
    Vector2 v = {0, 0};
};

Player player;

FocusableEntity *activeEntity = &player;

FocusableEntity testEntity;

Vector2 GetMouseGamePosition()
{
    Vector2 mouseCoords = GetMousePosition();
    mouseCoords.x += activeEntity->camera.target.x * SCALEFACTOR;
    mouseCoords.y += activeEntity->camera.target.y * SCALEFACTOR;
    mouseCoords.x -= activeEntity->camera.offset.x;
    mouseCoords.y -= activeEntity->camera.offset.y;
    mouseCoords.x /= SCALEFACTOR;
    mouseCoords.y /= SCALEFACTOR;

    return mouseCoords;
}

int main(void)
{
    // Setup
    int windowWidth = 1400;
    int windowHeight = 800;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "raylib [core] example - keyboard input");

    Texture2D test = LoadTexture("resources/test.png");
    Texture2D Xyno = LoadTexture("resources/Xyno.png");

    player.camera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};
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
        std::cout << player.v.x << std::endl;

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

        
        // When left right up is pressed, controls don't work properly!
        if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT))
        {
            player.v.x += 1;
        }
        else if (!IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_LEFT))
        {
            player.v.x -= 1;
        }
        else
        {
            player.v.x *= 0.8;
        }
        if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
        {
            player.v.y -= MOVE_SPEED;
        }
        else if (!IsKeyDown(KEY_UP) && IsKeyDown(KEY_DOWN))
        {
            player.v.y += MOVE_SPEED;
        }
        else
        {
            player.v.y *= FRICTION;
        }

        if (IsKeyDown(KEY_Q))
        {
            FocusableEntity old_entity = *activeEntity;
            activeEntity = &testEntity;
            activeEntity->camera.target = old_entity.camera.target;
        }
        if (IsKeyDown(KEY_W))
        {
            FocusableEntity old_entity = *activeEntity;
            activeEntity = &player;
            activeEntity->camera.target = old_entity.camera.target;
        }

        if (IsWindowResized())
        {
            std::cout << "wa" << std::endl;
            windowWidth = GetScreenWidth();
            windowHeight = GetScreenHeight();
            if (windowWidth > maxMonitorWidth)
                windowWidth = maxMonitorWidth;
            if (windowHeight > maxMonitorHeight)
                windowHeight = maxMonitorHeight;
            SetWindowSize(windowWidth, windowHeight);
        }

        if (player.v.x > 5)
            player.v.x = 5;
        if (player.v.x < -5)
            player.v.x = -5;
        if (player.v.y > 5)
            player.v.y = 5;
        if (player.v.y < -5)
            player.v.y = -5;

        // Physics
        player.pos.x += player.v.x;
        player.pos.y += player.v.y;
        if (abs(player.v.x) < 0.5)
            player.v.x = 0;
        if (abs(player.v.y) < 0.5)
            player.v.y = 0;

        // Update camera position to player position
        activeEntity->camera.offset.x = windowWidth / 2;
        activeEntity->camera.offset.y = windowHeight / 2;
        activeEntity->camera.target.x -= (activeEntity->camera.target.x - (activeEntity->pos.x + PLAYERWIDTH / 2)) * 0.15;
        activeEntity->camera.target.y -= (activeEntity->camera.target.y - (activeEntity->pos.y + PLAYERHEIGHT / 2)) * 0.15;

        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(activeEntity->camera);
        DrawTexture(test, 0, 0, WHITE);
        DrawTexture(Xyno, 50, 150, BLUE);
        DrawRectangle(player.pos.x, player.pos.y, 50, 50, BLUE);
        DrawRectangle(50, -30, PLAYERWIDTH, PLAYERHEIGHT, YELLOW);

        std::string text_string = std::to_string(GetMouseGamePosition().x) + " " + std::to_string(GetMouseGamePosition().y);
        DrawText(text_string.c_str(), GetMouseGamePosition().x, GetMouseGamePosition().y, 5 / SCALEFACTOR, RED);

        EndMode2D();

        text_string = std::to_string(player.v.x) + " " + std::to_string(player.v.y);

        // Debugging information
        DrawFPS(10, 10);
        DrawText(text_string.c_str(), 10, 50, 20, RED);

        EndDrawing();
    }

    UnloadTexture(test);
    UnloadTexture(Xyno);
    CloseWindow();
    return 0;
}