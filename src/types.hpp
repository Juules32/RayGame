#include "raylib.h"
#include "string"
#include <iostream>
#include <cmath>
#include "globals.hpp"

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
        void updateCamera() {
            camera.offset.x = windowWidth / 2;
            camera.offset.y = windowHeight / 2;
            camera.target.y -= (camera.target.y - (pos.y + PLAYERHEIGHT / 2)) * 0.15;
            camera.target.x -= (camera.target.x - (pos.x + PLAYERWIDTH / 2)) * 0.15;
            camera.zoom += (targetZoom - camera.zoom) * 0.15;

            if(shakeTime) {
                camera.offset.x += sin(shakeTime) * shakeTime;
                camera.offset.y += sin(shakeTime * 0.15) * shakeTime * 0.3;
                --shakeTime;
            }
        }

        void startShake() {
            shakeTime = 100;
        }
};

struct Player : FocusableEntity
{
    Vector2 v = {0, 0};
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
};

struct TextButton : Button
{
    std::string text;
    void outputText()
    {
        std::cout << text << std::endl;
    }

    TextButton(int x, int y, int width, int height, const std::string &text)
        : Button{x, y, width, height}, text(text) {}
};