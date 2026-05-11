#pragma once
struct Rect
{
    float x;
    float y;
    float w;
    float h;

    bool IsHit(const Rect& other) const
    {
        return (x < other.x + other.w &&
            x + w > other.x &&
            y < other.y + other.h &&
            y + h > other.y);
    }

    bool Contains(float px, float py) const
    {
        return (px >= x && px <= x + w &&
            py >= y && py <= y + h);
    }
};
