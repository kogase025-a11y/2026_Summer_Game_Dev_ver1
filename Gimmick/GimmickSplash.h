#pragma once
#include "GimmickSuper.h"

#include <string>

class GimmickSplash : public GimmickSuper {
public:
    GimmickSplash(float x, float y, float width, float height);
    ~GimmickSplash() override;

    void Update(float deltaTime) override;
    void Draw(int scrollX, int scrollY) const override;
    void OnTouch(Player& player, float deltaTime) override;

    int GetWorldX() const override { return static_cast<int>(x_); }
    int GetWorldY() const override { return static_cast<int>(y_); }
    Rect GetHitBox() const override;

private:
    float x_, y_;
    float width_, height_;

    bool isSplashing;
    float splashTimer;

    float leftSplashX, leftSplashY;
    float rightSplashX, rightSplashY;

    int imageHandle_;
};
