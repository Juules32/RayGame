#include "utils.hpp"


int GetScaledScreenWidth()
{
    return GetScreenWidth() / SCALEFACTOR;
}

int GetScaledScreenHeight()
{
    return GetScreenHeight() / SCALEFACTOR;
}

Vector2 GetMousePosition(Camera2D camera)
{
    Vector2 mouseCoords = GetMousePosition();
    mouseCoords.x += camera.target.x * camera.zoom;
    mouseCoords.y += camera.target.y * camera.zoom;
    mouseCoords.x -= camera.offset.x;
    mouseCoords.y -= camera.offset.y;
    mouseCoords.x /= camera.zoom;
    mouseCoords.y /= camera.zoom;

    return mouseCoords;
}

Vector2 GetMouseFixedPosition()
{
    Vector2 mouseCoords = GetMousePosition();
    mouseCoords.x /= fixedCamera.zoom;
    mouseCoords.y /= fixedCamera.zoom;

    return mouseCoords;
}