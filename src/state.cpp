#include "state.hpp"
#include <fstream>
#include "../libs/json.hpp"
#include <iostream>
#include "types.hpp"

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

    if (areaData.contains("exits") && areaData["exits"].is_array())
    {
        for (const auto &exit : areaData["exits"])
        {
            newArea.exits.push_back(Exit(exit["xywh"][0], exit["xywh"][1], exit["xywh"][2], exit["xywh"][3],
                                         (Vector2){exit["toPos"][0], exit["toPos"][1]}, exit["toAreaName"]));
        }
    }

    return newArea;
}

void unloadArea(Area area)
{
    UnloadTexture(active::area.background);
    UnloadTexture(active::area.foreground);
    UnloadImage(active::area.collisionImage);
    UnloadImageColors(active::area.collision);
    for (size_t i = 0; i < active::area.objects.size(); i++)
    {
        UnloadTexture(active::area.objects[i].texture);
    }
}

Player *active::player;
Interaction *active::interaction;
Area active::area;
FocusableEntity *active::entity;
float active::targetZoom = SCALEFACTOR;

int active::gamePhase = 1;

void active::changeEntity(FocusableEntity *newEntity)
{
    FocusableEntity old_entity = *active::entity;
    active::entity = newEntity;
    active::entity->camera.target = old_entity.camera.target;
    active::targetZoom = active::entity->zoom;
    active::entity->camera.zoom = old_entity.camera.zoom;
}

void active::changeArea(std::string areaName, Vector2 pos)
{

    // Unload pre-existing textures and audio
    unloadArea(active::area);

    // Move player to given position and align camera accordingly
    active::player->x = pos.x;
    active::player->y = pos.y;
    active::player->alignCamera();

    active::area = loadArea(areaName, pos);
}
