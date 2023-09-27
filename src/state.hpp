#pragma once
#include "globals.hpp"
#include <string>
#include <vector>

struct Player;
struct Interaction;
struct Area;
struct FocusableEntity;
struct Object;

namespace json
{
    Area loadArea(std::string areaName, Vector2 pos = {0, 0});
    Object loadObject(std::string objectName);
}

namespace active
{
    extern Player *player;
    extern Interaction *interaction;
    extern Area area;
    extern FocusableEntity *entity;
    extern Object* closestInteractible;
    extern int gamePhase;

    void changeEntity(FocusableEntity *newEntity);

    void changeArea(std::string areaName, Vector2 pos);
}

void unloadArea(Area area);

extern int indicatorFrameIncrementer;
extern int indicatorFrameCount;
extern int indicatorFrameTime;
static const int INDICATORWIDTH = 16;
static const int INDICATORHEIGHT = 16;

float distanceBetweenRectangles(const Rectangle& rect1, const Rectangle& rect2);

void findClosestInteractible();

void drawClosestInteractible();

void tryInteracting();