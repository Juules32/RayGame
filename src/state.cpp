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

    newObject.width = (objectData.contains("width")) ? (float)objectData["width"] : newObject.texture.width;
    newObject.height = (objectData.contains("height")) ? (float)objectData["height"] : newObject.texture.height;

    newObject.x = objectData["default"]["x"];
    newObject.y = objectData["default"]["y"];

    if (objectData.contains("collisionBox"))
    {
        newObject.collisionBox.x = (float) (objectData["collisionBox"]["x"]) + newObject.x;
        newObject.collisionBox.y = (float) (objectData["collisionBox"]["y"]) + newObject.y;
        newObject.collisionBox.width = objectData["collisionBox"]["width"];
        newObject.collisionBox.height = objectData["collisionBox"]["height"];
    }
    else
    {

        newObject.collisionBox.x = newObject.x;
        newObject.collisionBox.y = newObject.y;
        newObject.collisionBox.width = newObject.width;
        newObject.collisionBox.height = newObject.height;
    }
    cout << newObject.collisionBox.x << endl;
    cout << newObject.collisionBox.y << endl;
    cout << newObject.collisionBox.width << endl;
    cout << newObject.collisionBox.height << endl;

    if (objectData["default"].contains("dialogue") && objectData["default"]["dialogue"].is_array())
    {
        newObject.interaction = Interaction();
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

            newObject.interaction->IEs.push_back(Dialogue(dialogueText, optionContainer, reaction));
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
        for (size_t j = 0; j < active::area.objects[i].interaction->IEs.size(); j++)
        {
            if (active::area.objects[i].interaction->IEs[j].reaction.has_value())
            {
                UnloadTexture(active::area.objects[i].interaction->IEs[j].reaction.value());
            }
        }
    }
}

Player *active::player;
Interaction *active::interaction;
Area active::area;
FocusableEntity *active::entity;
Object *active::closestInteractible = nullptr;

int active::gamePhase = 1;

void active::changeEntity(FocusableEntity *newEntity)
{
    FocusableEntity *old_entity = active::entity;
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

int indicatorFrameIncrementer = 0;
int indicatorFrameCount = 0;
int indicatorFrameTime = 5;
bool indicatorFirstPartDone = false;

void findClosestInteractible()
{
    float smallestDist = 30; // Acts as minimum distance
    Object *closestPtr = nullptr;
    for (auto &object : active::area.objects)
    {
        if (!object.interaction.has_value())
        {
            continue;
        }
        float distBetweenRects = sqrt(pow((active::player->getCollisionBox().x + active::player->getCollisionBox().width / 2) - (object.collisionBox.x + object.collisionBox.width / 2), 2) +
                                      pow((active::player->getCollisionBox().y + active::player->getCollisionBox().height / 2) - (object.collisionBox.y + object.collisionBox.height / 2), 2));

        if (distBetweenRects < smallestDist)
        {
            smallestDist = distBetweenRects;
            closestPtr = &object;
        }
    }

    active::closestInteractible = closestPtr;
}

void drawClosestInteractible()
{
    findClosestInteractible();

    if (active::closestInteractible != nullptr)
    {
        ++indicatorFrameIncrementer;
        if (indicatorFrameCount > 2)
        {
            if(indicatorFirstPartDone == false) {
                indicatorFirstPartDone = true;
                indicatorFrameIncrementer = 20;
            }
            indicatorFrameCount = indicatorFrameIncrementer / (indicatorFrameTime * 4);
            if (indicatorFrameCount % 2)
            {
                indicatorFrameCount = 3;
            }
            else
            {
                indicatorFrameCount = 4;
            }
        }

        else
        {
            indicatorFrameCount = indicatorFrameIncrementer / indicatorFrameTime;
        }

        DrawTextureRec(other::interactibleIndicator, (Rectangle){(float)(INDICATORWIDTH * indicatorFrameCount), 0, INDICATORWIDTH, INDICATORHEIGHT}, {active::closestInteractible->x, active::closestInteractible->y - INDICATORHEIGHT}, WHITE);
    }
    else
    {
        indicatorFrameIncrementer = 0;
        indicatorFrameCount = 0;
        indicatorFirstPartDone = false;
    }
}

void tryInteracting()
{
    if (IsKeyPressed(KEY_E))
    {
        if (active::closestInteractible != nullptr)
        {

            active::changeEntity(active::closestInteractible);
            active::interaction = &active::closestInteractible->interaction.value();
            isInteracting = true;
        }
    }
}