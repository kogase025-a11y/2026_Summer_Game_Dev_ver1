#include "../Gimmick/GimmickWaterDrop.h"
#include "../Player/Player.h" // Playerの関数を呼ぶため

GimmickWater::GimmickWater(float x, float startY, float endY, float interval, std::shared_ptr<ImageFile> tex)
    : posX_(x), startY_(startY), endY_(endY), posY_(startY), interval_(interval), tex_(tex)
{
    timer_ = interval_;
    isVisible_ = false;
    hitBoxSizeX = 20; // 当たり判定の幅
    hitBoxSizeY = 20; // 当たり判定の高さ
}

void GimmickWater::Update(float deltaTime)
{
    if (!isVisible_)
    {
        // 待機中：タイマーを減らす
        timer_ -= deltaTime;
        if (timer_ <= 0)
        {
            isVisible_ = true;
            posY_ = startY_;
        }
    }
    else
    {
        // 落下中：下に移動
        posY_ += 400.0f * deltaTime; // 400.0fは落下の速さ（調整可能）

        // 地面まで行ったら消える
        if (posY_ > endY_)
        {
            isVisible_ = false;
            timer_ = interval_; // 再びタイマーセット
            wasTouching = false; // 当たり判定フラグをリセット
        }
    }
}

void GimmickWater::Draw(int scrollX, int scrollY) const
{
    if (!isVisible_) return;

    int drawX = static_cast<int>(posX_) - scrollX;
    int drawY = static_cast<int>(posY_) - scrollY;

    if (tex_)
    {
        DrawGraph(drawX - 16, drawY - 16, tex_->GetHandle(), TRUE);
    }
    else
    {
        // 画像がない場合の仮描画（青い丸）
        DrawCircle(drawX, drawY, 10, GetColor(0, 100, 255), TRUE);
    }
}

Rect GimmickWater::GetHitBox() const
{
    Rect r; // まず変数を作る
    if (!isVisible_)
    {
        // 非表示のときはサイズ0のRectを返す
        r.x = 0.0f;
        r.y = 0.0f;
        r.w = 0.0f;
        r.h = 0.0f;
        return r;
    }

    // posX_, posY_ は水滴の中心なので、
    // そこからサイズ（hitBoxSize）の半分を引いて「左上」の座標を計算します
    r.x = posX_ - (static_cast<float>(hitBoxSizeX) / 2.0f);
    r.y = posY_ - (static_cast<float>(hitBoxSizeY) / 2.0f);

    // 幅と高さはそのまま代入します
    r.w = static_cast<float>(hitBoxSizeX);
    r.h = static_cast<float>(hitBoxSizeY);

    return r;
}
void GimmickWater::OnTouch(Player& player, float deltaTime)
{
    // すでに落下中でない（消えている）なら何もしない
    if (!isFalling_) return;

    // wasTouching を使って、その水滴との最初の接触だけ判定する
    if (!wasTouching) {
        // プレイヤーを汚す
        player.AddDirt();

        // 【重要】当たったら水滴を消す（待機状態に戻す）
        isFalling_ = false;
        timer_ = interval_; // 次に落ちるまでの待ち時間をリセット

        // 接触フラグを立てる
        wasTouching = true;

        // もし水滴が当たった時の専用SEがあればここで鳴らす
        // PlaySoundMem(waterHitSe, DX_PLAYTYPE_BACK);
    }
}