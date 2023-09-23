#pragma once
#include "globals.hpp"
#include <string>
#include <vector>
#include <memory>

extern const int END_CONVERSATION;

struct FocusableEntity
{

private:
    float shakeTime = 0;

public:
    Vector2 pos = {0, 0};
    Camera2D camera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, (float) SCALEFACTOR};
    int zoom = 1;
    int width = 0;
    int height = 0;

    void updateCamera();

    void alignCamera();

    void startShake();
};

struct Button
{
    int x;
    int y;
    int width;
    int height;
    void draw();
    bool isClicked();
    bool isHovered();
};

struct TextButton : Button
{
    std::string text;
    TextButton(int x, int y, int width, int height, const std::string &text);
    void outputText();
};

struct Option : Button
{
    std::string text;
    int position;
    int toIndex;

    Option(std::string text, int toIndex = 1);
};

struct SelectOption
{
    int toIndex = 1;
    const int TEXTSIZE = 10;
    const int DISTFROMBOTTOM = 100;
    const int DISTFROMRIGHT = 10;
    const int OPTIONHEIGHT = 30;
    int OPTIONWIDTH = 50;
    std::vector<Option> options;
    unsigned int selectedOptionIndex = 0;

    SelectOption(std::vector<Option> o);

    int start();
};

struct Dialogue
{
    bool relativeIndex = true;
    std::string text;
    SelectOption *selectOption; // Optional field as a pointer
    Texture2D *reaction;        // Optional field as a pointer

    // Constructor with an optional SelectOption
    Dialogue(std::string text);

    Dialogue(std::string text, Texture2D *reaction, SelectOption *selectOption = nullptr);

    Dialogue(std::string text, SelectOption *selectOption, Texture2D *reaction = nullptr);

    // Returned value is the index change for the current conversation
    // 0 means staying in the same dialoguewq
    int start();

    void reset();
};

struct Interaction
{
    unsigned int currentDialogueIndex = 0;

    // Is necessary in case dialogue is shown twice in same interaction
    bool shouldReset = true;

    std::vector<std::unique_ptr<Dialogue>> IEs = {};
    std::vector<Option> options = {Option("Yes"), Option("No", END_CONVERSATION), Option("Let's start over", -1)};
    std::unique_ptr<SelectOption> selectOption = std::make_unique<SelectOption>(options);

    Interaction();

    int iterate();
};

struct Object : FocusableEntity
{
    Texture2D texture;

    Object(std::string texturePath, int x, int y, int w, int h);

    void draw();
};

struct Exit : Rectangle
{
    Exit(float x, float y, float w, float h);
};

struct Player : FocusableEntity
{
    std::string name;
    Vector2 v = {0, 0};
    int frameIncrementer = 0;
    int frameTime = 5; // In frames
    int frameCount = 0;
    int frameWidth = 48;

    Texture2D idle, runRight, runLeft, runUp, runDown;

    Player(std::string name);

    void updateMovement();

    Rectangle getCollisionBox();

    bool overlapsWithCollision();

    void draw();
};

struct Area
{
    std::string name;
    Texture2D background, foreground;
    Image collisionImage;
    Color *collision;
    std::string musicID, ambienceID, oldMusicID, oldAmbienceID;
    float volume; // Used to incrementally increase old/new music and ambience volumes
    std::vector<Object> objects;
    std::vector<Exit> exits;

    void draw();
};

