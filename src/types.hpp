#include "raylib.h"
#include "string"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "globals.hpp"

struct FocusableEntity
{

private:
    float shakeTime = 0;

public:
    Vector2 pos = {0, 0};
    Camera2D camera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, 1};
    int zoom = 1;
    int width = 0;
    int height = 0;

    void updateCamera()
    {
        camera.offset.x = windowWidth / 2;
        camera.offset.y = windowHeight / 2;
        camera.target.x -= (camera.target.x - (pos.x + width / 2)) * 0.15;
        camera.target.y -= (camera.target.y - (pos.y + height / 2)) * 0.15;
        camera.zoom += (targetZoom - camera.zoom) * 0.15;

        if (shakeTime)
        {
            camera.offset.x += sin(shakeTime) * shakeTime;
            camera.offset.y += sin(shakeTime * 0.15) * shakeTime * 0.3;
            --shakeTime;
        }
    }

    void alignCamera()
    {
        camera.offset.x = windowWidth / 2;
        camera.offset.y = windowHeight / 2;
        camera.target.x = pos.x + width / 2;
        camera.target.y = pos.y + height / 2;
    }

    void startShake()
    {
        shakeTime = 100;
    }
};

struct Button
{
    int x;
    int y;
    int width;
    int height;
    void draw()
    {
        DrawRectangle(x, y, width, height, PURPLE);
    }
    bool isClicked()
    {
        // Check if the left mouse button is pressed
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            // and if the mouse pointer is within the button's boundaries
            Vector2 mouseCoords = GetMouseFixedPosition();
            if ((mouseCoords.x >= x && mouseCoords.x <= (x + width)) &&
                (mouseCoords.y >= y && mouseCoords.y <= (y + height)))
                return true; // Button is clicked
        }
        return false; // Button is not clicked
    }
    bool isHovered()
    {
        Vector2 mouseCoords = GetMouseFixedPosition();
        if ((mouseCoords.x >= x && mouseCoords.x <= (x + width)) &&
            (mouseCoords.y >= y && mouseCoords.y <= (y + height)))
            return true; // Button is clicked
        return false;    // Button is not clicked
    }
};

struct TextButton : Button
{
    std::string text;
    void outputText()
    {
        cout << text << endl;
    }

    TextButton(int x, int y, int width, int height, const std::string &text)
        : Button{x, y, width, height}, text(text) {}
};

struct Option : Button
{
    std::string text;
    int position;
    int toIndex;

    Option(std::string text, int toIndex = 1) : text(text), toIndex(toIndex){};
};

struct SelectOption
{
    int toIndex = 1;
    static const int TEXTSIZE = 10;
    static const int DISTFROMBOTTOM = 100;
    static const int DISTFROMRIGHT = 10;
    static const int OPTIONHEIGHT = 30;
    int OPTIONWIDTH = 50;
    std::vector<Option> options;
    unsigned int selectedOptionIndex = 0;

    SelectOption(std::vector<Option> o) : options(o)
    {
        // Set optionwidth according to longest text of dialogue option
        for (unsigned int i = 0; i < options.size(); i++)
        {
            int suggestedWidth = options[i].text.length() * TEXTSIZE;
            if (suggestedWidth > OPTIONWIDTH)
                OPTIONWIDTH = suggestedWidth;
        }

        // Sets size and index of each option
        for (unsigned int i = 0; i < options.size(); i++)
        {
            options[options.size() - 1 - i].width = OPTIONWIDTH;
            options[options.size() - 1 - i].height = OPTIONHEIGHT;
            options[i].position = i;
        }
    };

    int start()
    {
        // Updates option position, in case window is resized
        for (unsigned int i = 0; i < options.size(); i++)
        {
            options[options.size() - 1 - i].x = GetScaledScreenWidth() - DISTFROMRIGHT - OPTIONWIDTH;

            options[options.size() - 1 - i].y = GetScaledScreenHeight() - (DISTFROMBOTTOM + i * OPTIONHEIGHT);
        }

        // Changes selected option if arrow up or down is pressed
        if (IsKeyPressed(KEY_UP) && (selectedOptionIndex > 0))
            --selectedOptionIndex;
        if (IsKeyPressed(KEY_DOWN) && (selectedOptionIndex < options.size() - 1))
            ++selectedOptionIndex;

        // Continues interaction if enter is pressed
        if (IsKeyPressed(KEY_ENTER))
        {
            toIndex = options[selectedOptionIndex].toIndex;
            return toIndex;
        }

        // Changes selected option if mouse is hovered over
        for (unsigned int i = 0; i < options.size(); i++)
        {
            if (options[i].isHovered())
            {
                selectedOptionIndex = options[i].position;
                break;
            }
        }

        // If option is clicked, continues interaction to toIndex
        if (options[selectedOptionIndex].isClicked())
        {
            toIndex = options[selectedOptionIndex].toIndex;
            return toIndex;
        }

        // Draws options, with different color for selected option
        BeginMode2D(fixedCamera);
        for (unsigned int i = 0; i < options.size(); i++)
        {
            Option option = options[i];
            if (i == selectedOptionIndex)
                DrawRectangle(option.x, option.y, option.width, option.height, GREEN);
            else
                DrawRectangle(option.x, option.y, option.width, option.height, GRAY);
            DrawText(options[i].text.c_str(), option.x, option.y, TEXTSIZE, PURPLE);
        }
        EndMode2D();

        return 0;
    }
};

struct Dialogue
{
    bool relativeIndex = true;
    std::string text;
    SelectOption *selectOption = nullptr; // Optional field as a pointer
    Texture2D *reaction = nullptr;        // Optional field as a pointer

    // Constructor with an optional SelectOption
    Dialogue(std::string text, SelectOption *selectOption = nullptr)
        : text(text), selectOption(selectOption) {}

    Dialogue(std::string text, Texture2D *reaction, SelectOption *selectOption = nullptr)
        : text(text), reaction(reaction), selectOption(selectOption) {}

    // Returned value is the index change for the current conversation
    // 0 means staying in the same dialoguewq
    int start()
    {
        BeginMode2D(fixedCamera);
        if (reaction != nullptr)
        {
            DrawTexture(*reaction, 100, 50, WHITE);
        }
        DrawText(text.c_str(), 150, 50, 10, PURPLE);
        EndMode2D();

        if (selectOption != nullptr)
        {
            return selectOption->start();
        }
        else if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            return 1;
        }
        return 0;
    }

    void reset()
    {
        if (selectOption != nullptr)
        {
            selectOption->selectedOptionIndex = 0;
        }
    }
};

struct Interaction
{
    unsigned int currentDialogueIndex = 0;

    // Is necessary in case dialogue is shown twice in same interaction
    bool shouldReset = true;

    std::vector<std::unique_ptr<Dialogue>> IEs = {};
    std::vector<Option> options = {Option("Yes"), Option("No", END_CONVERSATION), Option("Let's start over", -1)};
    std::unique_ptr<SelectOption> selectOption = std::make_unique<SelectOption>(options);

    Interaction()
    {

        IEs.push_back(std::make_unique<Dialogue>("Hello", &Xyno::reaction::happy));
        IEs.push_back(std::make_unique<Dialogue>("Do you want to talk more?", selectOption.get()));
        IEs.push_back(std::make_unique<Dialogue>("Cool, me too"));
    }

    int iterate()
    {
        if (currentDialogueIndex >= IEs.size())
        {
            currentDialogueIndex = 0;
            return 0;
        }

        if (shouldReset)
        {
            IEs[currentDialogueIndex]->reset();
            shouldReset = false;
        }
        int targetDialogue = IEs[currentDialogueIndex]->start();
        if (targetDialogue)
        {
            if (IEs[currentDialogueIndex]->relativeIndex)
                currentDialogueIndex += targetDialogue;
            else
                currentDialogueIndex = targetDialogue;
            shouldReset = true;
        }

        return 1;
    }
};

struct Object : FocusableEntity
{
    Texture2D texture;

    Object(std::string texturePath, int x, int y, int w, int h)
    {
        texture = LoadTexture(texturePath.c_str());
        pos.x = x;
        pos.y = y;
        width = w;
        height = h;
    }

    void draw()
    {
        DrawTexture(texture, pos.x, pos.y, WHITE);
    }
};

struct Exit : Rectangle
{
    Exit(float x, float y, float w, float h) : Rectangle{x, y, w, h} {};
};

namespace area
{
    std::string name;
    Texture2D background;
    std::string musicID, ambienceID, oldMusicID, oldAmbienceID;
    float volume; // Used to incrementally increase old/new music and ambience volumes
    std::vector<Object> objects;
    std::vector<Exit> exits;

    std::map<std::string, std::function<void()>> selector;

    void initializeAreaContents()
    {

        selector["plains"] = []()
        {
            switch (gamePhase)
            {
            case 1:
                objects.push_back(Object("resources/ninja.png", 20, 20, 20, 20));
                break;
            }
        };
        selector["city"] = []() {
        };
    }

    void change(std::string areaName, Vector2 pos)
    {

        // Unload pre-existing textures and audio
        if (background.id)
            UnloadTexture(background);
        for (size_t i = 0; i < objects.size(); i++)
        {
            UnloadTexture(objects[i].texture);
        }
        objects.clear();

        // Move player to given position and align camera accordingly
        // activePlayer->pos = pos;
        // activePlayer->alignCamera();

        // Updates to given area name and loads content through selector
        name = areaName;
        background = LoadTexture(("resources/" + name + ".png").c_str());
        selector[areaName]();
    }

    // Make function checkInteractions that checks for exits, interactions etc.

    void draw()
    {
        DrawTexture(background, 0, 0, WHITE);
        for (size_t i = 0; i < objects.size(); i++)
        {
            objects[i].draw();
        }
    }
}

FocusableEntity *activeEntity;

void changeActiveEntity(FocusableEntity *newEntity)
{
    FocusableEntity old_entity = *activeEntity;
    activeEntity = newEntity;
    activeEntity->camera.target = old_entity.camera.target;
    targetZoom = activeEntity->zoom;
    activeEntity->camera.zoom = old_entity.camera.zoom;
}

Color *colors;

struct Player : FocusableEntity
{
    std::string name;
    Vector2 v = {0, 0};
    int frameIncrementer = 0;
    int frameTime = 5; // In frames
    int frameCount = 0;
    int frameWidth = 48;

    Texture2D idle, runRight, runLeft, runUp, runDown;

    Player(std::string name) : name(name)
    {
        idle = LoadTexture(("resources/" + name + "/" + name + ".png").c_str());
        runRight = LoadTexture(("resources/" + name + "/Run/Right.png").c_str());
        runLeft = LoadTexture(("resources/" + name + "/Run/Left.png").c_str());
        runUp = LoadTexture(("resources/" + name + "/Run/Up.png").c_str());
        runDown = LoadTexture(("resources/" + name + "/Run/Down.png").c_str());
    }

    void updateMovement()
    {
        if (playerCanMove)
        {
            // When left right up is pressed, controls don't work properly!
            if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT))
            {
                v.x += ACCELERATION;
            }
            else if (!IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_LEFT))
            {
                v.x -= ACCELERATION;
            }
            else
            {
                v.x *= FRICTION;
            }
            if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
            {
                v.y -= ACCELERATION;
            }
            else if (!IsKeyDown(KEY_UP) && IsKeyDown(KEY_DOWN))
            {
                v.y += ACCELERATION;
            }
            else
            {
                v.y *= FRICTION;
            }
        }
        else
        {
            v.x = 0;
            v.y = 0;
        }

        if (v.x > MAX_SPEED)
            v.x = MAX_SPEED;
        if (v.x < -MAX_SPEED)
            v.x = -MAX_SPEED;
        if (v.y > MAX_SPEED)
            v.y = MAX_SPEED;
        if (v.y < -MAX_SPEED)
            v.y = -MAX_SPEED;

        Vector2 posCopy = pos;

        pos.x += v.x;
        if (overlapsWithCollision())
        {
            pos.x = posCopy.x;
        }

        pos.y += v.y;
        if (overlapsWithCollision())
        {
            pos.y = posCopy.y;
        }

        if (std::fabs(v.x) < MOVEMENT_CUTOFF)
            v.x = 0;
        if (std::fabs(v.y) < MOVEMENT_CUTOFF)
            v.y = 0;
    }

    Rectangle getCollisionBox()
    {
        return (Rectangle) {pos.x + 16, pos.y + 32, 16, 16};
    }

    bool overlapsWithCollision()
    {
        Rectangle collisionBox = getCollisionBox();
        for (int yVal = collisionBox.y; yVal < collisionBox.y + collisionBox.height; yVal++)
        {
            for (int xVal = collisionBox.width; xVal < collisionBox.x + collisionBox.width; xVal++)
            {
                int index = yVal * area::background.width + xVal;
                if (colors[index].a > 100)
                    return true;
            }
        }
        return false;
    }

    void draw()
    {

        float absX = std::fabs(v.x);
        float absY = std::fabs(v.y);

        if (absX > 2 || absY > 2)
        {
            ++frameIncrementer;
        }
        if (absX == 0 && absY == 0)
        {
            DrawTexture(idle, pos.x, pos.y, WHITE);
            frameIncrementer = 0;
            return;
        }

        absX += 0.1;

        if (absX >= absY)
        {
            if (v.x >= 0)
            {
                DrawTextureRec(runRight, (Rectangle){frameWidth * frameCount, 48, 48, 48}, pos, WHITE);
            }
            else
            {
                DrawTextureRec(runLeft, (Rectangle){frameWidth * frameCount, 48, 48, 48}, pos, WHITE);
            }
        }
        else
        {
            if (v.y >= 0)
            {
                DrawTextureRec(runDown, (Rectangle){frameWidth * frameCount, 48, 48, 48}, pos, WHITE);
            }
            else
            {
                DrawTextureRec(runUp, (Rectangle){frameWidth * frameCount, 48, 48, 48}, pos, WHITE);
            }
        }

        frameCount = (frameIncrementer / frameTime) % 8;
    }
};

Player *activePlayer;
