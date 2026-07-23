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
    particleTex = fileMng_.LoadImageFM("Image/ToiletPaper.png");
    wetTexs[0] = fileMng_.LoadImageFM("Image/ToiletPaper2.png");
    wetTexs[1] = fileMng_.LoadImageFM("Image/ToiletPaper3.png");
    wetTexs[2] = fileMng_.LoadImageFM("Image/ToiletPaper4.png");
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

    // 現在位置の地面の高さを取得してそこに立つ
    positionY_ = stage_->GetGroundYAtX(positionX_);

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

    // 5. 特殊地形判定（水たまりなどのチェック）
    CheckTerrainCollision();

    // 8. 状態名の更新
    UpdateStateName(input);
}

/**
 * 水たまりや汚れ地面との接触判定
 * （GimmickPuddleのOnTouchシステムと完全に連動するリファクタリング版）
 */
void Player::CheckTerrainCollision() {
    // 【入った瞬間】の判定：前フレームは外で、今フレームが水たまり中のときにSEを鳴らす
    if (isInPuddle_ && !wasInPuddle_) {
        if (puddleSe_ && puddleSe_->GetHandle() != -1) {
            PlaySoundMem(puddleSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
        }
    }

    // 次のフレームの比較用に現在の状態を保存
    wasInPuddle_ = isInPuddle_;

    // 次のフレームの処理に向けて一旦リセット
    // プレイヤーが水たまりの中に残っていれば、次のフレームのギミック更新ループ（GimmickPuddle::OnTouch）内で
    // 再び player.SetInPuddle(true) が呼び出されて true に維持されます。
    isInPuddle_ = false;
    isInDirty_ = false; // 汚れ地面用のフラグも念のためリセット
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
 * 描画処理
 */
void Player::Draw(float cameraX, int playerGraphHandle) const {
    if (isInvincible_ && (GetNowCount() / 100) % 2 == 0) return;

    const int drawX = static_cast<int>(positionX_ - cameraX);
    const int drawY = static_cast<int>(positionY_);

    const int imageCenterY = drawY - static_cast<int>(HIT_H / 2.0f) + static_cast<int>(OFFSET_Y);

    if (dirtLevel_ > 0) {
        DrawRotaGraph(drawX, imageCenterY, 1.0, angle_, wetTexs[dirtLevel_ - 1]->GetHandle(), TRUE);
    }
    else {
        DrawRotaGraph(drawX, imageCenterY, 1.0, angle_, particleTex->GetHandle(), TRUE);
    }

    // デバッグ表示：赤い枠を表示
    Rect hb = GetHitBox();
    DrawBox(
        static_cast<int>(hb.x - cameraX),
        static_cast<int>(hb.y),
        static_cast<int>(hb.x + hb.w - cameraX),
        static_cast<int>(hb.y + hb.h),
        GetColor(255, 0, 0), FALSE
    );
}

/**
 * 物理移動と詳細な衝突判定（マップチップ対応完全版）
 */
void Player::Move(void) {
    const float playerHalfWidth = HIT_W / 2.0f;
    const float prevX = positionX_;

    // -------------------------------------------------------------------------
    // 1. 横移動と壁判定（マップチップ配列から直接押し戻す汎用ロジック）
    // -------------------------------------------------------------------------
    positionX_ += velocityX_;

    // ステージ端の制限
    const float stageW = (std::max)(0.0f, stage_->GetStageWidth());
    if (positionX_ < playerHalfWidth) { positionX_ = playerHalfWidth; velocityX_ = 0.0f; }
    if (positionX_ > stageW - playerHalfWidth) { positionX_ = stageW - playerHalfWidth; velocityX_ = 0.0f; }

    // マップチップ配列（IsWall）を使用した横方向の壁押し戻し処理
    // プレイヤーの「頭・中央・足元」の3つの高さで左右の壁判定を行う
    float checkYTop = positionY_ - HIT_H + 5.0f;
    float checkYMid = positionY_ - (HIT_H / 2.0f);
    float checkYBottom = positionY_ - 5.0f;

    if (velocityX_ > 0.0f) { // 右移動時：右側が壁にめり込んでいるかチェック
        float rightX = positionX_ + playerHalfWidth;
        if (stage_->IsWall(rightX, checkYTop) || stage_->IsWall(rightX, checkYMid) || stage_->IsWall(rightX, checkYBottom)) {
            // めり込む前の元のマスの境界（64の倍数）に押し戻す
            positionX_ = static_cast<float>((static_cast<int>(rightX) / 64) * 64) - playerHalfWidth - 0.1f;
            velocityX_ = 0.0f;
        }
    }
    else if (velocityX_ < 0.0f) { // 左移動時：左側が壁にめり込んでいるかチェック
        float leftX = positionX_ - playerHalfWidth;
        if (stage_->IsWall(leftX, checkYTop) || stage_->IsWall(leftX, checkYMid) || stage_->IsWall(leftX, checkYBottom)) {
            // めり込んだ壁の右端（次のマスの開始位置）に押し戻す
            positionX_ = static_cast<float>((static_cast<int>(leftX) / 64) * 64 + 64) + playerHalfWidth + 0.1f;
            velocityX_ = 0.0f;
        }
    }

    // 【レガシーコード退避】旧段差判定（アクセッサ消去のためビルドエラー回避のためコメントアウト）
    /*
    CheckStepCollision(stage_->GetStepStartX(), stage_->GetStepEndX(), stage_->GetStepTopY(), playerHalfWidth, prevX);
    CheckStepCollision(stage_->GetStep2StartX(), stage_->GetStep2EndX(), stage_->GetStep2TopY(), playerHalfWidth, prevX);
    CheckStepCollision(stage_->GetStep3StartX(), stage_->GetStep3EndX(), stage_->GetStep3TopY(), playerHalfWidth, prevX);
    */

    // -------------------------------------------------------------------------
    // 2. 縦移動と足元の地面判定
    // -------------------------------------------------------------------------
    // 3点チェックで足元の床（一番高い位置）を特定
    const float sYMid = stage_->GetGroundYAtX(positionX_);
    const float sYL = stage_->GetGroundYAtX(positionX_ - playerHalfWidth + 1.0f);
    const float sYR = stage_->GetGroundYAtX(positionX_ + playerHalfWidth - 1.0f);
    const float supportGroundY = (std::min)({ sYMid, sYL, sYR });

    // 崖から落ちる処理：足元に地面がなくなったら落下状態へ
    if (onGround_ && (positionY_ < supportGroundY - 0.5f)) {
        onGround_ = false;
    }

    positionY_ += velocityY_;

    // 着地判定
    if (positionY_ >= supportGroundY) {
        positionY_ = supportGroundY;
        velocityY_ = 0.0f;
        onGround_ = true;
    }
}

/**
 * 段差衝突の共通ロジック
 * ※マップチップ化により汎用壁判定に一本化したため、現在は未使用ですが一応残しています。
 */
void Player::CheckStepCollision(float sStart, float sEnd, float sTop, float pHalf, float prevX) {
    if (positionY_ > sTop + 0.5f) {
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
 * ジャンプ入力
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
        if (hold && jumpTimer_ > 0) {
            jumpTimer_--;
            Jump();
        }
        else if (!hold && velocityY_ < 0.0f) {
            jumpTimer_ = 0;
            velocityY_ *= 0.5f;
        }
        else {
            jumpTimer_ = 0;
        }
    }
}

void Player::Jump() { SetJumpPow(MAX_JUMP_POW); }
void Player::SetJumpPow(float pow) { velocityY_ = -pow; }

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