#include "raylib.h"
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

// Global Constants
const int SCALEFACTOR = 3;
const int PLAYERWIDTH = 50;
const int PLAYERHEIGHT = 50;
const float ACCELERATION = 0.7;
const float MAX_SPEED = 3.0;
const float FRICTION = 0.6;
const float MOVEMENT_CUTOFF = 0.1;
const int END_CONVERSATION = 100000;
const Color NIGHT = {0,20,40,150};
const Color DAY = {155,155,0,30};

// Global Variables
int windowWidth = 1400;
int windowHeight = 800;
float targetZoom = SCALEFACTOR;
Camera2D fixedCamera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};
bool settingsActive = false;
bool playerCanMove = true;

// Global helper functions
int GetScaledScreenWidth()
{
    return GetScreenWidth() / SCALEFACTOR;
}

int GetScaledScreenHeight()
{
    return GetScreenHeight() / SCALEFACTOR;
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