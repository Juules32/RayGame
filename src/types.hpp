#include "raylib.h"
#include "string"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include "globals.hpp"

int GetScaledScreenWidth() {
    return GetScreenWidth() / SCALEFACTOR;
}

int GetScaledScreenHeight() {
    return GetScreenHeight() / SCALEFACTOR;
}


struct Entity
{
    Vector2 pos = {0, 0};
};

struct FocusableEntity : Entity
{

private:
    float shakeTime = 0;

public:
    Camera2D camera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, 1};
    int zoom = 1;


    void updateCamera()
    {
        camera.offset.x = windowWidth / 2;
        camera.offset.y = windowHeight / 2;
        camera.target.y -= (camera.target.y - (pos.y + PLAYERHEIGHT / 2)) * 0.15;
        camera.target.x -= (camera.target.x - (pos.x + PLAYERWIDTH / 2)) * 0.15;
        camera.zoom += (targetZoom - camera.zoom) * 0.15;

        if (shakeTime)
        {
            camera.offset.x += sin(shakeTime) * shakeTime;
            camera.offset.y += sin(shakeTime * 0.15) * shakeTime * 0.3;
            --shakeTime;
        }
    }

    void startShake()
    {
        shakeTime = 100;
    }
};

struct Player : FocusableEntity
{
    Vector2 v = {0, 0};
};

Player player;

FocusableEntity *activeEntity = &player;

Vector2 GetMouseGamePosition()
{
    Vector2 mouseCoords = GetMousePosition();
    mouseCoords.x += activeEntity->camera.target.x * activeEntity->camera.zoom;
    mouseCoords.y += activeEntity->camera.target.y * activeEntity->camera.zoom;
    mouseCoords.x -= activeEntity->camera.offset.x;
    mouseCoords.y -= activeEntity->camera.offset.y;
    mouseCoords.x /= activeEntity->camera.zoom;
    mouseCoords.y /= activeEntity->camera.zoom;

    return mouseCoords;
}

Vector2 GetMouseFixedPosition()
{
    Vector2 mouseCoords = GetMousePosition();
    mouseCoords.x /= fixedCamera.zoom;
    mouseCoords.y /= fixedCamera.zoom;

    return mouseCoords;
}

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
        // Check if the left mouse button is pressed and if the mouse pointer is within the button's boundaries
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePos = GetMouseFixedPosition();
            if ((mousePos.x >= x && mousePos.x <= (x + width)) &&
                (mousePos.y >= y && mousePos.y <= (y + height)))
            {
                return true; // Button is clicked
            }
        }
        return false; // Button is not clicked
    }
    bool isHovered()
    {
        Vector2 mousePos = GetMouseFixedPosition();
        if ((mousePos.x >= x && mousePos.x <= (x + width)) &&
            (mousePos.y >= y && mousePos.y <= (y + height)))
        {
            return true; // Button is clicked
        }
        return false; // Button is not clicked
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

struct InteractionElement
{
    virtual int start() = 0;
    int toIndex = 1;
    bool relativeIndex = true;
};

struct Dialogue : InteractionElement
{
    std::string text;

    Dialogue(std::string text) : text(text){};

    int start() override
    {
        BeginMode2D(fixedCamera);
        DrawText(text.c_str(), 150, 150, 10, PURPLE);
        EndMode2D();

        if (IsKeyPressed(KEY_ENTER))
        {
            return 1;
        }
        return 0;
    }
};

struct Option : Button
{
    std::string text;
    int position;
    int toIndex;

    Option(std::string text, int toIndex = 1) : text(text), toIndex(toIndex){};
};

struct SelectOption : InteractionElement
{
    std::vector<Option> options;
    int selectedOptionIndex = 0;

    SelectOption(std::vector<Option> o) : options(o)
    {
        for (int i = 0; i < options.size(); i++)
        {
            options[options.size()-1-i].x = GetScaledScreenWidth() - 100;
            options[options.size()-1-i].y = GetScaledScreenHeight() - (200 + i * 30);
            options[options.size()-1-i].width = 30;
            options[options.size()-1-i].height = 30;
            options[i].position = i;
        }
    };

    int start() override
    {

        if (IsKeyPressed(KEY_UP))
        {
            --selectedOptionIndex;
        }
        if (IsKeyPressed(KEY_DOWN))
        {
            ++selectedOptionIndex;
        }
        if (selectedOptionIndex < 0)
            selectedOptionIndex = 0;
        if (selectedOptionIndex >= options.size())
            selectedOptionIndex = options.size() - 1;

        if (IsKeyPressed(KEY_ENTER))
        {
            toIndex = options[selectedOptionIndex].toIndex;
            return 1;
        }
        for (int i = 0; i < options.size(); i++)
        {
            if (options[i].isClicked())
            {
                toIndex = options[i].toIndex;
                return 1;
            }
            
        }
        for (int i = 0; i < options.size(); i++)
        {
            if (options[i].isHovered())
            {
                selectedOptionIndex = options[i].position;
                break;
            }
        }

        BeginMode2D(fixedCamera);
        for (int i = options.size() - 1; i >= 0; i--)
        {
            Option option = options[i];
            if (i == selectedOptionIndex)
                DrawRectangle(option.x, option.y, option.width, option.height, GREEN);
            else 
                DrawRectangle(option.x, option.y, option.width, option.height, GRAY);
            DrawText(options[i].text.c_str(), option.x, option.y, 10, PURPLE);
        }

        EndMode2D();
        return 0;
    }
};

struct Interaction
{

    int index = 0;
    std::vector<std::unique_ptr<InteractionElement>> IEs = {};

    Interaction()
    {
        std::vector<Option> options = {Option("Yes"), Option("No", END_CONVERSATION), Option("Nope", END_CONVERSATION)};
        IEs.push_back(std::make_unique<Dialogue>("Do you want to talk more?"));
        IEs.push_back(std::make_unique<SelectOption>(options));
        IEs.push_back(std::make_unique<Dialogue>("Cool, me too"));
    }

    int iterate(unsigned int i)
    {
        if (i >= IEs.size())
            return 0;

        if (IEs[i]->start())
        {
            if (IEs[i])
                if (IEs[i]->relativeIndex)
                    index += IEs[i]->toIndex;
        }
        return 1;
    }
};
