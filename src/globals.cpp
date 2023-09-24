#include "globals.hpp"

// Global Constants
const int SCALEFACTOR = 3;

const Color NIGHT = {0, 20, 40, 150};
const Color DAY = {155, 155, 0, 30};

// Global Variables
int windowWidth = 1400;
int windowHeight = 800;
Camera2D fixedCamera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};



Texture2D Xyno::reaction::happy;

void Xyno::init()
{
    Xyno::reaction::happy = LoadTexture("resources/happy.png");
}

void initGlobals()
{
    Xyno::init();
}

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