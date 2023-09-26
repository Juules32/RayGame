#include "globals.hpp"

// Global Constants
const int SCALEFACTOR = 3;

const Color NIGHT = {0, 20, 40, 150};
const Color DAY = {155, 155, 0, 30};

// Global Variables
int windowWidth = 1400;
int windowHeight = 800;
Camera2D fixedCamera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};




Texture2D other::interactibleIndicator;


void initGlobals()
{
    other::interactibleIndicator = LoadTexture("resources/other/interactible-indicator-animation.png");

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