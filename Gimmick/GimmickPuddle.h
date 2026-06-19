#pragma once
#include "GimmickSuper.h"

class GimmickPuddle : public GimmickSuper {
public:
    // x1: 左端, x2: 右端, y: 地面の高さ
    GimmickPuddle(float x1, float x2, float y);

    void Draw(int scrollX, int scrollY) const override;
    void Update(float deltaTime) override;
    void OnTouch(Player& player, float deltaTime) override;

    // 純粋仮想関数の実装
    int GetWorldX() const override { return static_cast<int>(x1_); }
    int GetWorldY() const override { return static_cast<int>(y_); }
    Rect GetHitBox() const override;

private:
    float x1_, x2_, y_;
};
