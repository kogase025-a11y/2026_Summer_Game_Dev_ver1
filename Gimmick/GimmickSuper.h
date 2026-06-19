#pragma once
#include "../Util/Rect.h"
#include "../Resource/ImageFile.h"

#include <DxLib.h>
#include <memory>


class Player;
class Vector2;
class GimmickSuper
{
public:
    virtual ~GimmickSuper() = default;

    // 毎フレーム呼ばれる（必要な場合のみオーバーライド）
    virtual void Update(float deltaTime) {}

    // 描画（必須・スクロール対応）
    virtual void Draw(int scrollX, int scrollY) const = 0;

    virtual Rect GetHitBox() const { return Rect{}; };

    // Playerが触れている間、毎フレーム呼ばれる
    // Gimmick同士は別関数で
    // ルール: ここでPlayerの変数を直接変更してはいけない
    // RequestAddLight, RequestResetLight を使う
    virtual void OnTouch(Player& player, float deltaTime) {};

    virtual void OnLeave() { wasTouching = false; }

    // ワールド座標を取得
    virtual int GetWorldX() const = 0;
    virtual int GetWorldY() const = 0;

protected:
    bool wasTouching = false;
    int hitBoxSizeX = 40;
    int hitBoxSizeY = 40;
    float releaseRate = 200.0f;
};