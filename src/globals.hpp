#pragma once
#include "raylib.h"
#include <iostream>
#include <vector>

using std::cout;
using std::cin;
using std::endl;

// Global Constants
extern const int SCALEFACTOR;

extern const Color NIGHT;
extern const Color DAY;

extern int windowWidth;
extern int windowHeight;
extern Camera2D fixedCamera;



// Global textures
namespace other {
    extern Texture2D interactibleIndicator;
}


void initGlobals();

extern bool settingsActive;
extern bool isInteracting;
extern bool playerCanMove;
extern std::vector<bool*> flags;

// Global helper functions
void checkIfPlayerCanMove();