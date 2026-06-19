#include "Player.h"
#include "../Stage/Stage.h"
#include "../Manager/FileManager.h"
#include "../Scene/SceneGame.h"
#include <DxLib.h>
#include <algorithm>
#include <cmath>

// コンストラクタ：リソースの読み込み
Player::Player(Stage* stage, FileManager& fileMng)
    : stage_(stage), fileMng_(fileMng), sceneGame_(nullptr)
{
    particleTex = fileMng_.LoadImageFM("Image/ToiletPaper.PNG");
    wetTexs[0] = fileMng_.LoadImageFM("Image/ToiletPaperYogore.PNG");
    wetTexs[1] = fileMng_.LoadImageFM("Image/ToiletPaperYogore2.PNG");
    wetTexs[2] = fileMng_.LoadImageFM("Image/ToiletPaperYogore3.PNG");
    jumpSe_ = fileMng_.LoadSoundFM("Image/Sound/jump03.mp3");
    goalSe_ = fileMng_.LoadSoundFM("Image/Sound/Level up! Ragnarok.mp3");
    puddleSe_ = fileMng_.LoadSoundFM("Image/Sound/powerdown07.mp3");
}

Player::~Player() {}

// システム初期化：基本情報の初期化
bool Player::SystemInit() {
    positionX_ = 200.0f;
    positionY_ = stage_->GetGroundY();
    velocityX_ = 0.0f;
    velocityY_ = 0.0f;
    onGround_ = true;
    jumpTimer_ = 0;
    dirtLevel_ = 0;
    wasInDirty_ = false;
    wasInPuddle_ = false;
    stateName_ = "Idle";
    return true;
}

// ゲーム開始時リセット
void Player::GameInit() {
    positionX_ = 300.0f;
    positionY_ = stage_->GetGroundY();
    velocityX_ = 0.0f;
    velocityY_ = 0.0f;
    onGround_ = true;
    jumpTimer_ = 0;
    dirtLevel_ = 0;
    wasInPuddle_ = false;
    wasInDirty_ = false;
    stateName_ = "Idle";
    invincibleTimer_ = 0.0f;
    isInvincible_ = false;
}

bool Player::Release() { return true; }

void Player::Update() { /* 旧インターフェース(未使用) */ }

/**
 * 毎フレームの更新メイン処理
 */
void Player::Update(const InputManager& input) {


   

    // 1. 入力による移動・ジャンプ
    ProcessMove(input);
    if (!onGround_) AddGravity();
    ProcessJump(input);

    // 2. 特殊状態タイマー更新
    UpdateInvincibleTimer();

    // 3. 回転演出（速度に比例して転がる角度を更新）
    angle_ += velocityX_ * 0.03f;

    // 4. 移動実行
    bool wasOnGround = onGround_;
    Move();
    bool landedThisFrame = (!wasOnGround && onGround_); // このフレームで着地したか

    // 5. 特殊地形判定
    CheckTerrainCollision();

    //// 6. 水たまりヒット時の処理
    //if (isInPuddle_ && !wasInPuddle_) {
    //    if (!isInvincible_) {
    //        if (dirtLevel_ < 3) dirtLevel_++;
    //        if (puddleSe_ && puddleSe_->GetHandle() != -1)
    //            PlaySoundMem(puddleSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
    //    }
    //}

    // (isInDiaty_ の方も同様に !isInvincible_ で囲む)
    /*
    // --- 着地音の検討メモ ---
    else if (landedThisFrame)
    {
        if (fallSe_ && fallSe_->GetHandle() != -1)
        {
            PlaySoundMem(fallSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
        }
    }
    */
   /* wasInPuddle_ = isInPuddle_;*/

    // 7. 汚れ地面ヒット時の処理
    if (isInDirty_ && !wasInDirty_) {
        if (dirtLevel_ < 3) dirtLevel_++;
    }
    wasInDirty_ = isInDirty_;

    // 8. 状態名の更新
    UpdateStateName(input);
}

/**
 * 水たまりや汚れ地面との接触判定
 */
void Player::CheckTerrainCollision() {
    isInPuddle_ = false;
    const float puddleStart = stage_->GetPuddleStartX();
    const float puddleEnd = stage_->GetPuddleEndX();

    isInDirty_ = false;
    const float dirtyStart = stage_->GetDirtyStartX();
    const float dirtyEnd = stage_->GetDirtyEndX();

    // 地面にいて、X座標が範囲内なら「中」とみなす
    if (onGround_ && (positionY_ == stage_->GetGroundY())) {
        if (positionX_ >= puddleStart && positionX_ <= puddleEnd) isInPuddle_ = true;
        if (positionX_ >= dirtyStart && positionX_ <= dirtyEnd) isInDirty_   = true;
    }
}

/**
 * 無敵タイマーの管理
 */
void Player::UpdateInvincibleTimer() {
    if (invincibleTimer_ > 0) {
        invincibleTimer_ -= (1.0f / 60.0f); // 60FPSベース
        if (invincibleTimer_ <= 0) {
            invincibleTimer_ = 0;
            isInvincible_ = false;
        }
    }
}

/**
 * アニメーション制御用の状態文字列を決定
 */
void Player::UpdateStateName(const InputManager& input) {
    if (input.IsTrgDown(KEY_INPUT_X))      stateName_ = "Damage";
    else if (input.IsTrgDown(KEY_INPUT_Z)) stateName_ = "Attack";
    else if (!onGround_)                   stateName_ = (velocityY_ < 0.0f) ? "Jump" : "Fall";
    else if (std::abs(velocityX_) > 0.1f)  stateName_ = "Run";
    else                                   stateName_ = "Idle";
}

void Player::Draw() { Draw(0.0f, -1); }

/**
 * 描画処理：汚れレベルに応じた画像選択と、中心点調整
 */
void Player::Draw(float cameraX, int playerGraphHandle) const {
    // 無敵中の点滅演出
    if (isInvincible_) {
        if ((GetNowCount() / 100) % 2 == 0) return;
    }

    const int drawX = static_cast<int>(positionX_ - cameraX);
    const int drawY = static_cast<int>(positionY_);

    // 画像の「穴」が足元から32px上にくるように中心点を設定
    const int imageCenterY = drawY - 32;

    // 汚れ、通常画像、外部指定、または仮の四角形の順で描画
    if (dirtLevel_ > 0) {
        int animIndex = dirtLevel_ - 1;
        while (animIndex >= 0 && !wetTexs[animIndex]) animIndex--;
        if (animIndex >= 0 && wetTexs[animIndex]) {
            DrawRotaGraph(drawX, imageCenterY, 1.0, angle_, wetTexs[animIndex]->GetHandle(), TRUE);
        }
    }
    else if (particleTex) {
        DrawRotaGraph(drawX, imageCenterY, 1.0, angle_, particleTex->GetHandle(), TRUE);
    }
    else if (playerGraphHandle >= 0) {
        DrawRotaGraph(drawX, imageCenterY, 1.0, angle_, playerGraphHandle, TRUE);
    }
    else {
        DrawBox(drawX - 32, drawY - 64, drawX + 32, drawY, GetColor(120, 220, 255), TRUE);
    }

    // デバッグ用当たり判定枠
    DrawBox(drawX - 20, drawY - 64, drawX + 20, drawY, GetColor(255, 0, 0), FALSE);
}

/**
 * 物理移動と詳細な衝突判定
 */
void Player::Move(void) {
    /*
    // --- 旧ロジック（ブロック単位の判定検討時のもの） ---
    const float halfW = 20.0f;
    const float prevX_ = positionX_;
    positionX_ += velocityX_;
    if (stage_->IsBlock(positionX_ + halfW, positionY_ - 5.0f) ||
        stage_->IsBlock(positionX_ + halfW, positionY_ - 60.0f) ||
        stage_->IsBlock(positionX_ - halfW, positionY_ - 5.0f) ||
        stage_->IsBlock(positionX_ - halfW, positionY_ - 60.0f))
    {
        positionX_ = prevX_;
        velocityX_ = 0.0f;
    }
    ...
    */

    const float playerHalfWidth = static_cast<float>(SIZE_X) / 2.0f;
    const float prevX = positionX_;

    // 横移動
    positionX_ += velocityX_;

    // ステージ幅でクランプ（(std::min)のように括弧をつけるのはDxLibマクロ衝突回避）
    const float stageW = (std::max)(0.0f, stage_->GetStageWidth());
    const float minX = playerHalfWidth;
    const float maxX = (std::max)(minX, stageW - playerHalfWidth);
    if (positionX_ < minX) positionX_ = minX;
    if (positionX_ > maxX) positionX_ = maxX;

   

    // 段差の衝突判定（3つ分）
    CheckStepCollision(stage_->GetStepStartX(), stage_->GetStepEndX(), stage_->GetStepTopY(), playerHalfWidth, prevX);
    CheckStepCollision(stage_->GetStep2StartX(), stage_->GetStep2EndX(), stage_->GetStep2TopY(), playerHalfWidth, prevX);
    CheckStepCollision(stage_->GetStep3StartX(), stage_->GetStep3EndX(), stage_->GetStep3TopY(), playerHalfWidth, prevX);

   

    // 足元の高さ計算（プレイヤーの幅の範囲で最も高い場所を探す）
    const float sYMid = stage_->GetGroundYAtX(positionX_);
    const float sYL = stage_->GetGroundYAtX(positionX_ - playerHalfWidth + 1.0f);
    const float sYR = stage_->GetGroundYAtX(positionX_ + playerHalfWidth - 1.0f);
    const float supportGroundY = (std::min)(sYMid, (std::min)(sYL, sYR));

    // 地面より浮いた判定
    if (onGround_ && (positionY_ < supportGroundY - 0.5f)) onGround_ = false;

    // 縦移動
    positionY_ += velocityY_;
    if (positionY_ >= supportGroundY) {
        positionY_ = supportGroundY;
        velocityY_ = 0.0f;
        onGround_ = true;
    }
}

/**
 * 段差衝突の共通ロジック：横からの押し戻し
 */
void Player::CheckStepCollision(float sStart, float sEnd, float sTop, float pHalf, float prevX) {
    if (positionY_ > sTop + 0.5f) { // 段差より下にいる場合のみ衝突
        if (positionX_ + pHalf > sStart && positionX_ - pHalf < sEnd) {
            if (prevX + pHalf <= sStart)      positionX_ = sStart - pHalf;
            else if (prevX - pHalf >= sEnd)   positionX_ = sEnd + pHalf;
            else {
                float mid = sStart + (sEnd - sStart) * 0.5f;
                positionX_ = (positionX_ < mid) ? (sStart - pHalf) : (sEnd + pHalf);
            }
            velocityX_ = 0.0f;
        }
    }
}

/**
 * 入力デバイスからの左右移動値取得
 */
void Player::ProcessMove(const InputManager& input) {
    const int pad = GetJoypadInputState(DX_INPUT_KEY_PAD1);
    const float moveInput =
        (input.IsNew(KEY_INPUT_RIGHT) || (pad & PAD_INPUT_RIGHT) ? 1.0f : 0.0f) -
        (input.IsNew(KEY_INPUT_LEFT) || (pad & PAD_INPUT_LEFT) ? 1.0f : 0.0f);

    if (moveInput > 0.0f)      Accele(MOVE_ACC);
    else if (moveInput < 0.0f) Accele(-MOVE_ACC);
    else                       Decelerate(MOVE_DEC);
}

void Player::Accele(float speed) {
    velocityX_ += speed;
    if (velocityX_ > MAX_MOVE_SPEED)  velocityX_ = MAX_MOVE_SPEED;
    if (velocityX_ < -MAX_MOVE_SPEED) velocityX_ = -MAX_MOVE_SPEED;
}

void Player::Decelerate(float speed) {
    if (velocityX_ > 0.0f)      velocityX_ = (std::max)(0.0f, velocityX_ - speed);
    else if (velocityX_ < 0.0f) velocityX_ = (std::min)(0.0f, velocityX_ + speed);
}

void Player::AddGravity() { velocityY_ += GRAVITY; }

/**
 * ジャンプ入力：押し続けると高く飛ぶマリオ風ジャンプ
 */
void Player::ProcessJump(const InputManager& input) {
    const bool trg = input.IsTrgDown(KEY_INPUT_SPACE) || input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
    const bool hold = input.IsNew(KEY_INPUT_SPACE) || input.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

    if (onGround_ && trg) {
        onGround_ = false;
        jumpTimer_ = INPUT_JUMP_FRAME;
        Jump();
        if (jumpSe_ && jumpSe_->GetHandle() != -1) PlaySoundMem(jumpSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
    }
    else if (!onGround_) {
        if (hold && jumpTimer_ > 0) { // 押し続けて上昇維持
            jumpTimer_--;
            Jump();
        }
        else if (!hold && velocityY_ < 0.0f) { // 離したら上昇力カット
            jumpTimer_ = 0;
            velocityY_ *= 0.5f;
        }
        else {
            jumpTimer_ = 0;
        }
    }
}


void Player::Jump() { SetJumpPow(MAX_JUMP_POW); }
void Player::SetJumpPow(float pow) { velocityY_ = -pow; 
}
void Player::PlayGoalSound() {
    if (goalSe_ && goalSe_->GetHandle() != -1) PlaySoundMem(goalSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
}
void Player::StartInvincible(float sec) 
{
    invincibleTimer_ = sec;
    isInvincible_ = true;
}

const char* Player::GetStateName() const { return stateName_; }
float Player::GetX() const { return positionX_; }
float Player::GetY() const { return positionY_; }
   