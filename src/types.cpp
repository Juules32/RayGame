#include "types.hpp"
#include <iostream>
#include <map>
#include <cmath>
#include "utils.hpp"

const float ACCELERATION = 0.7;
const float MAX_SPEED = 2.5;
const float FRICTION = 0.6;
const float MOVEMENT_CUTOFF = 0.1;
const int END_CONVERSATION = 100000;

void FocusableEntity::updateCamera()
{
    camera.offset.x = windowWidth / 2;
    camera.offset.y = windowHeight / 2;
    camera.target.x -= (camera.target.x - (x + width / 2)) * 0.15;
    camera.target.y -= (camera.target.y - (y + height / 2)) * 0.15;
    camera.zoom += (targetZoom - camera.zoom) * 0.15;

    if (shakeTime)
    {
        camera.offset.x += sin(shakeTime) * shakeTime;
        camera.offset.y += sin(shakeTime * 0.15) * shakeTime * 0.3;
        --shakeTime;
    }
}

void FocusableEntity::alignCamera()
{
    camera.offset.x = windowWidth / 2;
    camera.offset.y = windowHeight / 2;
    camera.target.x = x + width / 2;
    camera.target.y = y + height / 2;
}

void FocusableEntity::startShake()
{
    shakeTime = 100;
}

void Button::draw()
{
    DrawRectangle(x, y, width, height, PURPLE);
}
bool Button::isClicked()
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
bool Button::isHovered()
{
    Vector2 mouseCoords = GetMouseFixedPosition();
    if ((mouseCoords.x >= x && mouseCoords.x <= (x + width)) &&
        (mouseCoords.y >= y && mouseCoords.y <= (y + height)))
        return true; // Button is clicked
    return false;    // Button is not clicked
}

TextButton::TextButton(int x, int y, int width, int height, const std::string &text)
    : Button{x, y, width, height}, text(text) {}
void TextButton::outputText()
{
    cout << text << endl;
}

Option::Option(std::string text, int toIndex) : text(text), toIndex(toIndex){};

OptionContainer::OptionContainer(std::vector<Option> o) : options(o)
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
    }
};

int OptionContainer::start()
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
            selectedOptionIndex = i;
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

// Constructor with an optional OptionContainer
Dialogue::Dialogue(std::string text)
    : text(text)
{
    reaction = std::nullopt;
    optionContainer = std::nullopt;
}

Dialogue::Dialogue(std::string text, std::optional<Texture2D> reaction, std::optional<OptionContainer> optionContainer)
    : text(text), optionContainer(optionContainer), reaction(reaction) {}

Dialogue::Dialogue(std::string text, std::optional<OptionContainer> optionContainer, std::optional<Texture2D> reaction)
    : text(text), optionContainer(optionContainer), reaction(reaction) {}

// Returned value is the index change for the current conversation
// 0 means staying in the same dialoguewq
int Dialogue::start()
{
    BeginMode2D(fixedCamera);
    if (reaction != std::nullopt)
    {
        DrawTexture(*reaction, 100, 50, WHITE);
    }
    DrawText(text.c_str(), 150, 50, 10, PURPLE);
    EndMode2D();

    if (optionContainer != std::nullopt)
    {
        return optionContainer->start();
    }
    else if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        return 1;
    }
    return 0;
}

void Dialogue::reset()
{
    if (optionContainer != std::nullopt)
    {
        optionContainer->selectedOptionIndex = 0;
    }
}

int Interaction::iterate()
{
    if (currentDialogueIndex >= IEs.size())
    {
        currentDialogueIndex = 0;
        return 0;
    }

    if (shouldReset)
    {
        IEs[currentDialogueIndex].reset();
        shouldReset = false;
    }
    int targetDialogue = IEs[currentDialogueIndex].start();
    if (targetDialogue)
    {
        if (IEs[currentDialogueIndex].relativeIndex)
            currentDialogueIndex += targetDialogue;
        else
            currentDialogueIndex = targetDialogue;
        shouldReset = true;
    }

    return 1;
}



void Object::draw()
{
    DrawTexture(texture, x, y, WHITE);
}

Exit::Exit(float x, float y, float w, float h, Vector2 toPos, std::string toAreaName) : Rectangle{x, y, w, h}, toPos(toPos), toAreaName(toAreaName) {};

void Exit::draw() {
    DrawRectangle(x, y, width, height, PURPLE);
}

Player::Player(std::string name) : name(name)
{
    idle = LoadTexture(("resources/players/" + name + "/" + name + ".png").c_str());
    runRight = LoadTexture(("resources/players/" + name + "/Run/Right.png").c_str());
    runLeft = LoadTexture(("resources/players/" + name + "/Run/Left.png").c_str());
    runUp = LoadTexture(("resources/players/" + name + "/Run/Up.png").c_str());
    runDown = LoadTexture(("resources/players/" + name + "/Run/Down.png").c_str());
    width = idle.width;
    height = idle.height;
}

bool Player::overlapsWithCollision(Area* activeArea)
{
    Rectangle collisionBox = Player::getCollisionBox();

    if (collisionBox.x <= 0 || collisionBox.y <= 0 ||
        collisionBox.x + collisionBox.width > activeArea->background.width ||
        collisionBox.y + collisionBox.height > activeArea->background.height)
    {
        return true;
    }

    for (int yVal = collisionBox.y; yVal < collisionBox.y + collisionBox.height; yVal++)
    {
        for (int xVal = collisionBox.x; xVal < collisionBox.x + collisionBox.width; xVal++)
        {
            int index = yVal * activeArea->background.width + xVal;
            if (activeArea->collision[index].a > 100)
                return true;
        }
    }
    return false;

}

Exit* Player::overlapsWithExit(Area* activeArea) {
    Rectangle collisionBox = Player::getCollisionBox();

    for(auto& exit : activeArea->exits) {
        Rectangle collisionRect = GetCollisionRec(collisionBox, exit);
        if(collisionRect.width && collisionRect.height) {
            return &exit;
        }
    }

    return nullptr;
}

void Player::draw()
{

    float absX = std::fabs(v.x);
    float absY = std::fabs(v.y);

    if (absX > 2 || absY > 2)
    {
        ++frameIncrementer;
    }
    if (absX == 0 && absY == 0)
    {
        DrawTexture(idle, x, y, WHITE);
        frameIncrementer = 0;
        return;
    }

    absX += 0.1;

    if (absX >= absY)
    {
        if (v.x >= 0)
        {
            DrawTextureRec(runRight, (Rectangle){(float) (frameWidth * frameCount), 48, 48, 48}, {x, y}, WHITE);
        }
        else
        {
            DrawTextureRec(runLeft, (Rectangle){(float) (frameWidth * frameCount), 48, 48, 48}, {x, y}, WHITE);
        }
    }
    else
    {
        if (v.y >= 0)
        {
            DrawTextureRec(runDown, (Rectangle){(float) (frameWidth * frameCount), 48, 48, 48}, {x, y}, WHITE);
        }
        else
        {
            DrawTextureRec(runUp, (Rectangle){(float) (frameWidth * frameCount), 48, 48, 48}, {x, y}, WHITE);
        }
    }

    frameCount = (frameIncrementer / frameTime) % 8;
}

Rectangle Player::getCollisionBox()
{
    return (Rectangle){x + 16, y + 32, 16, 16};
}

void Player::updateMovement(Area* activeArea)
{
    checkIfPlayerCanMove();
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

    float xCopy = x;
    float yCopy = y;

    x += v.x;
    if (overlapsWithCollision(activeArea))
    {
        x = xCopy;
    }

    y += v.y;
    if (overlapsWithCollision(activeArea))
    {
        y = yCopy;
    }

    if (std::fabs(v.x) < MOVEMENT_CUTOFF)
        v.x = 0;
    if (std::fabs(v.y) < MOVEMENT_CUTOFF)
        v.y = 0;
}

void Area::draw()
{
    DrawTexture(background, 0, 0, WHITE);
    DrawTexture(foreground, 0, 0, WHITE);
    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i].draw();
    }
    for (size_t i = 0; i < exits.size(); i++)
    {
        exits[i].draw();
    }
}
