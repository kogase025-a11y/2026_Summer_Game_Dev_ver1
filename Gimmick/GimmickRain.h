#pragma once
#include "GimmickSuper.h"
#include <memory>

class RainManager;
class SceneGame;

class GimmickRain : public GimmickSuper {
public:
    GimmickRain(float x, float y, int w, int h, SceneGame* scene);
    virtual ~GimmickRain();

    void Update(float deltaTime) override;
    void Draw(int scrollX, int scrollY) const override;
    void OnTouch(Player& player, float deltaTime) override;

    Rect GetHitBox() const override;
    int GetWorldX() const override { return static_cast<int>(worldX_); }
    int GetWorldY() const override { return static_cast<int>(worldY_); }

private:
    float worldX_, worldY_;
    int width_, height_;
    SceneGame* scene_;
    std::unique_ptr<RainManager> rainManager_;
};