#include "globals.hpp"
#include "raylib.h"
#include <iostream>
#include <vector>

// Global Constants
const int SCALEFACTOR = 3;
const int PLAYERWIDTH = 48;
const int PLAYERHEIGHT = 48;
const float ACCELERATION = 0.7;
const float MAX_SPEED = 2.5;
const float FRICTION = 0.6;
const float MOVEMENT_CUTOFF = 0.1;
const int END_CONVERSATION = 100000;
int gamePhase = 1;
const Color NIGHT = {0, 20, 40, 150};
const Color DAY = {155, 155, 0, 30};

// Global Variables
int windowWidth = 1400;
int windowHeight = 800;
float targetZoom = SCALEFACTOR;
Camera2D fixedCamera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};
bool settingsActive = false;
bool isInteracting = false;
bool playerCanMove = true;
std::vector<bool *> flags = {&settingsActive, &isInteracting};

// Global helper functions
void checkIfPlayerCanMove()
{
    for (bool *menu : flags)
    {
        if (*menu)
        {
            playerCanMove = false;
            return;
        }
    }
    playerCanMove = true;
}

int GetScaledScreenWidth()
{
    return GetScreenWidth() / SCALEFACTOR;
}

int GetScaledScreenHeight()
{
    return GetScreenHeight() / SCALEFACTOR;
}

Vector2 GetMousePosition(Camera2D camera)
{
    Vector2 mouseCoords = GetMousePosition();
    mouseCoords.x += camera.target.x * camera.zoom;
    mouseCoords.y += camera.target.y * camera.zoom;
    mouseCoords.x -= camera.offset.x;
    mouseCoords.y -= camera.offset.y;
    mouseCoords.x /= camera.zoom;
    mouseCoords.y /= camera.zoom;

    return mouseCoords;
}

Vector2 GetMouseFixedPosition()
{
    Vector2 mouseCoords = GetMousePosition();
    mouseCoords.x /= fixedCamera.zoom;
    mouseCoords.y /= fixedCamera.zoom;

    return mouseCoords;
}

Texture2D Xyno::reaction::happy;

void Xyno::init()
{
    Xyno::reaction::happy = LoadTexture("resources/happy.png");
}

void initGlobals()
{
    Xyno::init();
}

Area loadArea(std::string areaName, Vector2 pos)
{
    Area newArea;

    using json = nlohmann::json;
    json areaData;

    // Open the JSON file using std::ifstream
    std::ifstream inputFile("resources/areas/" + areaName + "/areaData.json");

    // Check if the file is open
    if (!inputFile.is_open())
    {
        std::cerr << "Failed to open JSON file." << std::endl;
    }

    // Parse the JSON data
    try
    {
        inputFile >> areaData;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }

    // Close the file
    inputFile.close();

    newArea.name = areaName;
    newArea.background = LoadTexture(("resources/areas/" + areaName + "/background.png").c_str());
    newArea.foreground = LoadTexture(("resources/areas/" + areaName + "/foreground.png").c_str());
    newArea.collisionImage = LoadImage(("resources/areas/" + areaName + "/collision.png").c_str());
    newArea.collision = LoadImageColors(newArea.collisionImage);

    return newArea;
}

Player *active::player;
Interaction *active::interaction;
Area active::area;
FocusableEntity *active::entity;

void active::changeEntity(FocusableEntity *newEntity)
{
    FocusableEntity old_entity = *active::entity;
    active::entity = newEntity;
    active::entity->camera.target = old_entity.camera.target;
    targetZoom = active::entity->zoom;
    active::entity->camera.zoom = old_entity.camera.zoom;
}

void active::changeArea(std::string areaName, Vector2 pos)
{

    // Unload pre-existing textures and audio
    UnloadTexture(active::area.background);
    UnloadTexture(active::area.foreground);
    UnloadImage(active::area.collisionImage);
    UnloadImageColors(active::area.collision);
    for (size_t i = 0; i < active::area.objects.size(); i++)
    {
        UnloadTexture(active::area.objects[i].texture);
    }

    // Move player to given position and align camera accordingly
    active::player->pos = pos;
    active::player->alignCamera();

    active::area = loadArea(areaName, pos);
}