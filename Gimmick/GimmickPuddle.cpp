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
    // --- 1. 透明度の設定 ---
    // 前は150でしたが、210に上げました（よりハッキリ見えます）
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 210);

    // --- 2. 水たまり本体を描画 ---
    // 前は (50, 150, 255) でしたが、(0, 80, 200) にして「深い青」にしました
    unsigned int puddleColor = GetColor(0, 80, 200);

    DrawBox(
        static_cast<int>(x1_) - scrollX,
        static_cast<int>(y_) - 2 - scrollY,
        static_cast<int>(x2_) - scrollX,
        static_cast<int>(y_) + 10 - scrollY,
        puddleColor, TRUE
    );

    // --- 3. 【追加】フチ取りを描画する（これでさらに「濃く」見えます） ---
    // 少しだけ暗い色で、塗りつぶさない四角を描きます
    DrawBox(
        static_cast<int>(x1_) - scrollX,
        static_cast<int>(y_) - 2 - scrollY,
        static_cast<int>(x2_) - scrollX,
        static_cast<int>(y_) + 10 - scrollY,
        GetColor(0, 40, 150), FALSE // FALSEは枠線だけ
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