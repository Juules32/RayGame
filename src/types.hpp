#pragma once
#include "globals.hpp"
#include <string>
#include <vector>
#include <memory>
#include <optional>

extern const int END_CONVERSATION;

struct FocusableEntity
{

private:
    float shakeTime = 0;

public:
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;


    Camera2D camera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, (float) SCALEFACTOR};
    int targetZoom = SCALEFACTOR;

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
    int toIndex;

    Option(std::string text, int toIndex = 1);
};

struct OptionContainer
{
    static const int TEXTSIZE = 10;
    static const int DISTFROMBOTTOM = 100;
    static const int DISTFROMRIGHT = 10;
    static const int OPTIONHEIGHT = 30;
    int toIndex = 1;
    int OPTIONWIDTH = 50;
    std::vector<Option> options;
    unsigned int selectedOptionIndex = 0;

    OptionContainer(std::vector<Option> o);

    int start();
};

struct Dialogue
{
    bool relativeIndex = true;
    std::string text;
    std::optional<OptionContainer> optionContainer; // Optional field as a pointer
    std::optional<Texture2D> reaction;        // Optional field as a pointer

    // Constructor with an optional OptionContainer
    Dialogue(std::string text);

    Dialogue(std::string text, std::optional<Texture2D> reaction, std::optional<OptionContainer> optionContainer = std::nullopt);

    Dialogue(std::string text, std::optional<OptionContainer> optionContainer, std::optional<Texture2D> reaction = std::nullopt);

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

    std::vector<Dialogue> IEs;
    std::vector<Option> options;
    OptionContainer* optionContainer;
    

    int iterate();
};

struct Object : FocusableEntity
{
    Texture2D texture;
    Interaction interaction;
    Camera2D camera = {(Vector2){0, 0}, (Vector2){0, 0}, 0, (float) SCALEFACTOR};

    void draw();
};

struct Exit : Rectangle
{
    Vector2 toPos;
    std::string toAreaName;

    Exit(float x, float y, float w, float h, Vector2 toPos, std::string toAreaName);

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

    void updateMovement(Area* activeArea);

    Rectangle getCollisionBox();

    bool overlapsWithCollision(Area* activeArea);
    Exit* overlapsWithExit(Area* activeArea);

    void draw();
};


