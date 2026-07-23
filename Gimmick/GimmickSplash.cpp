#include "GimmickSplash.h"
#include "../Player/Player.h"
#include <DxLib.h>

GimmickSplash::GimmickSplash(float x, float y, float width, float height)
    : x_(x), y_(y), width_(width), height_(height),
      isSplashing(false), splashTimer(0.0f),
      leftSplashX(0), leftSplashY(0),
      rightSplashX(0), rightSplashY(0),
      imageHandle_(-1)
{
    hitBoxSizeX = static_cast<int>(width);
    hitBoxSizeY = static_cast<int>(height);

    // フォルダ名が「Resource」ではなく「Image」なので修正
    imageHandle_ = LoadGraph("Image/Haneterumizu.png");
}

GimmickSplash::~GimmickSplash() {
    if (imageHandle_ != -1) {
        DeleteGraph(imageHandle_);
    }
}

void GimmickSplash::Update(float deltaTime) {
    if (isSplashing) {
        splashTimer += deltaTime;

        // 左右へ放物線を描いて飛ぶ（必要に応じて速度や重力を調整してください）
        leftSplashX -= 120.0f * deltaTime;
        leftSplashY += (-200.0f + 400.0f * splashTimer) * deltaTime;

        rightSplashX += 120.0f * deltaTime;
        rightSplashY += (-200.0f + 400.0f * splashTimer) * deltaTime;

        // 1秒経過で水しぶきを終了
        if (splashTimer > 1.0f) {
            isSplashing = false;
        }
    }
}

void GimmickSplash::Draw(int scrollX, int scrollY) const {
    if (isSplashing && imageHandle_ != -1) {
        // 後半にかけてフェードアウト処理
        int alpha = 255;
        if (splashTimer > 0.5f) {
            alpha = static_cast<int>(255.0f * (1.0f - (splashTimer - 0.5f) * 2.0f));
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;
        }

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        // 左に飛ぶ水しぶきを描画（画像を左右反転）
        DrawRotaGraph(static_cast<int>(leftSplashX) - scrollX, static_cast<int>(leftSplashY) - scrollY,
                      1.0, 0.0, imageHandle_, TRUE, TRUE);

        // 右に飛ぶ水しぶきを描画
        DrawRotaGraph(static_cast<int>(rightSplashX) - scrollX, static_cast<int>(rightSplashY) - scrollY,
                      1.0, 0.0, imageHandle_, TRUE, FALSE);

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // --- デバッグ用の描画（当たり判定の可視化） ---
    // ※不要になったらこの下の行をコメントアウトするか削除してください
    DrawBox(
        static_cast<int>(x_) - scrollX, 
        static_cast<int>(y_) - scrollY, 
        static_cast<int>(x_ + width_) - scrollX, 
        static_cast<int>(y_ + height_) - scrollY, 
        GetColor(255, 0, 0), FALSE // 赤色の枠線で当たり判定を描画
    );
}

void GimmickSplash::OnTouch(Player& player, float deltaTime) {
    if (!wasTouching) {
        // プレイヤーが新しく触れた瞬間に水しぶきを発生させる
        isSplashing = true;
        splashTimer = 0.0f;

        // ギミックの中心付近から発生させる
        leftSplashX = x_ + width_ / 2.0f;
        leftSplashY = y_ + height_ / 2.0f;

        rightSplashX = x_ + width_ / 2.0f;
        rightSplashY = y_ + height_ / 2.0f;

		// プレイヤーを汚す
		player.AddDirt(); 
    }
    

    wasTouching = true;
}

Rect GimmickSplash::GetHitBox() const {
    Rect r;
    r.x = x_;
    r.y = y_;
    r.w = width_;
    r.h = height_;
    return r;
}
