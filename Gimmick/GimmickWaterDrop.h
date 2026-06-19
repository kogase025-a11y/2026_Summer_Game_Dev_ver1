#pragma once

#include "GimmickSuper.h"

class GimmickWater : public GimmickSuper
{
public:
    // x: 落ちる場所, startY: 天井の高さ, endY: 地面の高さ, interval: 落ちる間隔(秒)
    GimmickWater(float x, float startY, float endY, float interval, std::shared_ptr<ImageFile> tex);

    // 親クラスの純粋仮想関数を実装（これで継承エラーが消えます）
    void Draw(int scrollX, int scrollY) const override;
    int GetWorldX() const override { return static_cast<int>(posX_); }
    int GetWorldY() const override { return static_cast<int>(posY_); }

    // 動きの更新
    void Update(float deltaTime) override;

    // 当たり判定の範囲を返す
    Rect GetHitBox() const override;

    // プレイヤーに当たった時の処理
    void OnTouch(Player& player, float deltaTime) override;

private:
    float posX_;        // X座標
    float posY_;        // 現在のY座標
    float startY_;      // 出現位置
    float endY_;        // 消える位置
    float timer_;       // 復活までのタイマー
    float interval_;    // 復活する間隔
    bool isVisible_;    // 今落ちている最中か
    std::shared_ptr<ImageFile> tex_; // 水滴の画像
};