#include "../Gimmick/GimmickWaterDrop.h" 
#include "../Player/Player.h"
#include <DxLib.h>

GimmickWater::GimmickWater(float x, float startY, float endY, float interval, std::shared_ptr<ImageFile> tex)
    : posX_(x), startY_(startY), endY_(endY), posY_(startY), interval_(interval), tex_(tex)
{
    timer_ = interval_;
    isVisible_ = false; // 最初は待機中
    wasTouching = false;
    hitBoxSizeX = 30;
    hitBoxSizeY = 30;
}

void GimmickWater::Update(float deltaTime)
{
    if (!isVisible_)
    {
        // 1. 待機中の処理
        timer_ -= deltaTime;
        if (timer_ <= 0)
        {
            isVisible_ = true;  // 出現！
            posY_ = startY_;    // 開始位置に戻す
            wasTouching = false; // 水滴判定リセット
        }
    }
    else
    {
        // 2. 落下中の処理
        posY_ += 800.0f * deltaTime; // 下に移動

        // 地面まで行ったら消える
        if (posY_ > endY_)
        {
            isVisible_ = false;
            timer_ = interval_;
            wasTouching = false; // 地面についた時も念のためリセット
        }
    }
}

void GimmickWater::Draw(int scrollX, int scrollY) const
{
    if (!isVisible_) return; // 見えていない時は描かない

    int drawX = static_cast<int>(posX_) - scrollX;
    int drawY = static_cast<int>(posY_) - scrollY;

    if (tex_) {
        DrawGraph(drawX - 16, drawY - 16, tex_->GetHandle(), TRUE);
    }
    else {
        DrawCircle(drawX, drawY, 10, GetColor(0, 100, 255), TRUE);
    }
}

Rect GimmickWater::GetHitBox() const
{
    Rect r;
    //見えていない時は当たり判定を消す
    if (!isVisible_) {
        r.x = 0; r.y = 0; r.w = 0; r.h = 0;
        return r;
    }

    r.x = posX_ - (static_cast<float>(hitBoxSizeX) / 2.0f);
    r.y = posY_ - (static_cast<float>(hitBoxSizeY) / 2.0f);
    r.w = static_cast<float>(hitBoxSizeX);
    r.h = static_cast<float>(hitBoxSizeY);
    return r;
}

void GimmickWater::OnTouch(Player& player, float deltaTime)
{
    // isFalling_ ではなく isVisible_ を使う
    if (!isVisible_) return;

    // まだこの水滴に当たっていないなら
    if (!wasTouching) {
        player.AddDirt(); // プレイヤーを汚す

        // 当たった瞬間に「消す」
        isVisible_ = false;
        timer_ = interval_;

        // 接触済みフラグを立てる
        wasTouching = true;
    }
}