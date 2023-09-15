#include "raylib.h"
#include "string"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include "globals.hpp"

int GetScaledScreenWidth()
{
    return GetScreenWidth() / SCALEFACTOR;
}

int GetScaledScreenHeight()
{
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
    SelectOption* selectOption = nullptr; // Optional field as a pointer
    Texture2D* reaction = nullptr; // Optional field as a pointer

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
        if(reaction != nullptr) {
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
};

struct Interaction
{

    int currentDialogueIndex = 0;
    std::vector<std::unique_ptr<Dialogue>> IEs = {};
    std::vector<Option> options = {Option("Yes"), Option("No", END_CONVERSATION), Option("Let's start over", -1)};
    std::unique_ptr<SelectOption> selectOption = std::make_unique<SelectOption>(options);

    Interaction()
    {

        IEs.push_back(std::make_unique<Dialogue>("Hello", &Xyno::reaction::happy));
        IEs.push_back(std::make_unique<Dialogue>("Do you want to talk more?", selectOption.get()));
        IEs.push_back(std::make_unique<Dialogue>("Cool, me too"));
    }

    int iterate(unsigned int i)
    {
        if (i >= IEs.size())
            return 0;

        int targetDialogue = IEs[i]->start();
        if (targetDialogue)
        {
            if (IEs[i])
            {   
                if (IEs[i]->relativeIndex)
                    currentDialogueIndex += targetDialogue;
                else
                    currentDialogueIndex = targetDialogue;
            }
        }
        return 1;
    }
};
