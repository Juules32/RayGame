
#include <limits>
#include "types.hpp"
#include "state.hpp"
#include "utils.hpp"

int main(void)
{
    // Setup
    Interaction testInteraction = Interaction();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "raylib [core] example - keyboard input");
    SetTargetFPS(60);

    // Set exit key to some key that doesn't exist
    SetExitKey(std::numeric_limits<int>::max());

    initGlobals();

    TextButton testButton(10,10,50,50,"Hello!");
    FocusableEntity testEntity;
    testEntity.zoom = 1;
    Player player = Player("xyno");
    active::player = &player;
    active::entity = active::player;
    active::area = loadArea("plains");
    player.zoom = SCALEFACTOR;
    player.alignCamera();
    

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
            active::changeEntity(&testEntity);
        }
        if (IsKeyPressed(KEY_W))
        {
            active::changeEntity(active::player);
        }
        if (IsKeyPressed(KEY_E))
        {
            testButton.outputText();
        }
        if (IsKeyPressed(KEY_R))
        {
            active::entity->startShake();
        }
        if (IsKeyPressed(KEY_T))
        {
            active::interaction = &testInteraction;
            isInteracting = true;
        }
        if (IsKeyPressed(KEY_Y))
        {
            active::changeArea("city", (Vector2){50, 50});
        }
        if (IsKeyPressed(KEY_U))
        {
            active::changeArea("plains", (Vector2){150, 50});
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
        active::player->updateMovement();

        // Update camera position to player position
        active::entity->updateCamera();
        
        // Rendering
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        BeginMode2D(active::entity->camera);
        active::area.draw();

        Rectangle rect = active::player->getCollisionBox();
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, GREEN);
        active::player->draw();
        EndMode2D();

        
        // Current interaction
        if(isInteracting) {
            if(!active::interaction->iterate()) {
                isInteracting = false;
            }
        }


        // Settings menu
        if(settingsActive) {
            BeginMode2D(fixedCamera);
            DrawText("hej", 60, 20, 10, RED);
            DrawRectangle(windowWidth / 2 / SCALEFACTOR, windowHeight / 2 / SCALEFACTOR, player.width, player.height, ORANGE);
            testButton.draw();
            EndMode2D();
        }


        // Debugging information
        DrawFPS(10, 10);
        DrawText((std::to_string(player.v.x) + " " + std::to_string(player.v.y)).c_str(), 10, 50, 20, RED);
        DrawText((std::to_string(GetMousePosition(active::entity->camera).x) + " " + std::to_string(GetMousePosition(active::entity->camera).y)).c_str(), 10, 70, 20, RED);
        DrawText((std::to_string(GetMouseFixedPosition().x) + " " + std::to_string(GetMouseFixedPosition().y)).c_str(), 10, 90, 20, RED);
        DrawText(active::area.name.c_str(), 100, 10, 20, ORANGE);
        if (GetMousePosition(active::entity->camera).x >= 0 && GetMousePosition(active::entity->camera).x < active::area.background.width &&
                GetMousePosition(active::entity->camera).y >= 0 && GetMousePosition(active::entity->camera).y < active::area.background.height)
            {
                int index = ((int)GetMousePosition(active::entity->camera).y * active::area.background.width) + (int)GetMousePosition(active::entity->camera).x;
                Color pixelColor = active::area.collision[index];
                DrawText((("Pixel Color: R=" + std::to_string(pixelColor.r) + ", G=" + std::to_string(pixelColor.g) + ", B=" + std::to_string(pixelColor.b) + ", A=" + std::to_string(pixelColor.a))).c_str(), 180, 20, 20, BLUE);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}