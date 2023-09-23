#pragma once
#include "types.hpp"

using std::cout;
using std::cin;
using std::endl;

// Global Constants
extern const int SCALEFACTOR;
extern const int PLAYERWIDTH;
extern const int PLAYERHEIGHT;
extern const float ACCELERATION;
extern const float MAX_SPEED;
extern const float FRICTION;
extern const float MOVEMENT_CUTOFF;
extern const int END_CONVERSATION;
extern int gamePhase;
extern const Color NIGHT;
extern const Color DAY;

// Global Variables
extern int windowWidth;
extern int windowHeight;
extern float targetZoom;
extern Camera2D fixedCamera;
extern bool settingsActive;
extern bool isInteracting;
extern bool playerCanMove;
extern std::vector<bool*> flags;

// Global helper functions
void checkIfPlayerCanMove();

int GetScaledScreenWidth();

int GetScaledScreenHeight();


Vector2 GetMousePosition(Camera2D camera);

Vector2 GetMouseFixedPosition();


// Global textures
namespace Xyno {
    namespace reaction {
        extern Texture2D happy;
    }

    void init();
}

void initGlobals();

Area loadArea(std::string areaName, Vector2 pos = {0, 0});

namespace active
{
    extern Player *player;
    extern Interaction *interaction;
    extern Area area;
    extern FocusableEntity *entity;

    void changeEntity(FocusableEntity *newEntity);

    void changeArea(std::string areaName, Vector2 pos);
}
