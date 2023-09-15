#include "raylib.h"

using std::cout;
using std::cin;
using std::endl;

const int SCALEFACTOR = 3;
int windowWidth = 1400;
int windowHeight = 800;
const int PLAYERWIDTH = 50;
const int PLAYERHEIGHT = 50;
const float MOVE_SPEED = 1.0;
const float FRICTION = 0.8;
const int END_CONVERSATION = 100000;
float targetZoom = SCALEFACTOR;

Camera2D fixedCamera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, SCALEFACTOR};

bool settingsActive = false;

bool playerCanMove = true;

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