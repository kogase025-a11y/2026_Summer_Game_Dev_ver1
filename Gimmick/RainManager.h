#pragma once
#include <vector>

class SceneGame;

struct Drop {
    float x, y, speed;
    bool isActive;
    Drop() : x(0.0f), y(0.0f), speed(0.0f), isActive(false) {}
};

class RainManager {
public:
    RainManager(int maxDrops);
    void Update(float deltaTime, float areaX, float areaY, int areaW, int areaH, SceneGame* scene);
    void Draw(int scrollX, int scrollY) const;

private:
    std::vector<Drop> drops_;
    float timer_ = 0.0f;
};