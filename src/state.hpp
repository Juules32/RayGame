#pragma once
#include "globals.hpp"
#include <string>
#include <vector>

struct Player;
struct Interaction;
struct Area;
struct FocusableEntity;

Area loadArea(std::string areaName, Vector2 pos = {0, 0});
void unloadArea(Area area);

namespace active
{
    extern Player *player;
    extern Interaction *interaction;
    extern Area area;
    extern FocusableEntity *entity;
    extern float targetZoom;

    extern int gamePhase;

    void changeEntity(FocusableEntity *newEntity);

    void changeArea(std::string areaName, Vector2 pos);
}

