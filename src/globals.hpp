#include "raylib.h"
#include <iostream>
#include <vector>

using std::cout;
using std::cin;
using std::endl;

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
const Color NIGHT = {0,20,40,150};
const Color DAY = {155,155,0,30};

// Global Variables
int windowWidth = 1400;
int windowHeight = 800;
float targetZoom = SCALEFACTOR;
Camera2D fixedCamera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};
bool settingsActive = false;
bool isInteracting = false;
bool playerCanMove = true;
std::vector<bool*> flags = {&settingsActive, &isInteracting};

// Global helper functions
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


// Global textures
namespace Xyno {
    namespace reaction {
        Texture2D happy;
    }

    void init() {
        reaction::happy = LoadTexture("resources/happy.png");
    }
}

void init() {
    Xyno::init();
}