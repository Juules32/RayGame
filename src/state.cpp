#include "state.hpp"
#include <fstream>
#include "../libs/json.hpp"
#include <iostream>
#include "types.hpp"

Object json::loadObject(std::string objectName)
{
    Object newObject;

    using json = nlohmann::json;
    json objectData;

    // Open the JSON file using std::ifstream
    std::ifstream inputFile("resources/objects/" + objectName + "/objectData.json");

    // Check if the file is open
    if (!inputFile.is_open())
    {
        std::cerr << "Failed to open JSON file." << std::endl;
    }

    // Parse the JSON data
    try
    {
        inputFile >> objectData;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }

    // Close the file
    inputFile.close();

    newObject.texture = LoadTexture(("resources/objects/" + objectName + "/" + objectName + ".png").c_str());
    
    newObject.x = objectData["default"]["xywh"][0];
    newObject.y = objectData["default"]["xywh"][1];
    newObject.width = objectData["default"]["xywh"][2];
    newObject.height = objectData["default"]["xywh"][3];

    if (objectData["default"].contains("dialogue") && objectData["default"]["dialogue"].is_array())
    {
        for (const auto &dialogueElement : objectData["default"]["dialogue"])
        {
            std::string dialogueText = dialogueElement["text"];
            std::optional<OptionContainer> optionContainer = std::nullopt;
            std::optional<Texture2D> reaction = std::nullopt;

            if (dialogueElement.contains("options") && dialogueElement["options"].is_array())
            {
                std::vector<Option> options;
                for (const auto &option : dialogueElement["options"])
                {
                    if (option.size() == 2)
                    {
                        options.push_back(Option(option[0], option[1]));
                    }
                    else
                    {
                        options.push_back(Option(option[0]));
                    }
                }
                optionContainer = OptionContainer(options);
            }

            if (dialogueElement.contains("reaction"))
            {
                std::string reactionType = dialogueElement["reaction"];
                reaction = LoadTexture(("resources/reactions/" + reactionType + ".png").c_str());
            }

            newObject.interaction.IEs.push_back(Dialogue(dialogueText, optionContainer, reaction));
        }
    }

    return newObject;
}

Area json::loadArea(std::string areaName, Vector2 pos)
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

    if (areaData.contains("objects") && areaData["objects"].is_array())
    {
        for (const auto &objectName : areaData["objects"])
        {
            newArea.objects.push_back(loadObject(objectName));

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

int active::gamePhase = 1;

void active::changeEntity(FocusableEntity *newEntity)
{
    FocusableEntity* old_entity = active::entity;
    active::entity = newEntity;
    active::entity->camera.target = old_entity->camera.target;
    active::entity->camera.zoom = old_entity->camera.zoom;
}

void active::changeArea(std::string areaName, Vector2 pos)
{

    // Unload pre-existing textures and audio
    unloadArea(active::area);

    // Move player to given position and align camera accordingly
    active::player->x = pos.x;
    active::player->y = pos.y;
    active::player->alignCamera();

    active::area = json::loadArea(areaName, pos);
}
