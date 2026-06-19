#include "GimmickPuddle.h"
#include "../Player/Player.h"

GimmickPuddle::GimmickPuddle(float x1, float x2, float y)
    : x1_(x1), x2_(x2), y_(y) {
    // 当たり判定のサイズを計算しておく
    hitBoxSizeX = static_cast<int>(x2_ - x1_);
    hitBoxSizeY = 20;
}

void GimmickPuddle::Update(float deltaTime) {
    // 水たまりは動かないので何もしない（あるいはキラキラさせる演出など）
}

void GimmickPuddle::Draw(int scrollX, int scrollY) const {
    // 描画：透明感のある青色にする
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); // 半透明に
    DrawBox(
        static_cast<int>(x1_) - scrollX,
        static_cast<int>(y_) - 2 - scrollY, // 地面より少しだけ上にする
        static_cast<int>(x2_) - scrollX,
        static_cast<int>(y_) + 10 - scrollY,
        GetColor(50, 150, 255), TRUE
    );
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // 戻す
}

Rect GimmickPuddle::GetHitBox() const {
    Rect r;
    r.x = x1_;
    r.y = y_ - 5; // 足元がちょっと触れたら当たり
    r.w = x2_ - x1_;
    r.h = 10.0f;
    return r;
}

void GimmickPuddle::OnTouch(Player& player, float deltaTime) {
    // 【解説】wasTouchingを使うことで、入った瞬間だけ汚れる
    if (!wasTouching) {
        player.AddDirt(); // 汚れを増やす
        wasTouching = true;
    }
}