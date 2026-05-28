#include "Player.h"
#include "../Stage/Stage.h"
#include "../Manager/FileManager.h"

#include <DxLib.h>
#include <algorithm>

Player::Player(Stage* stage, FileManager& fileMng) : stage_(stage), fileMng_(fileMng), sceneGame_(nullptr) {
	
	 particleTex = fileMng_.LoadImageFM("Image/ToiletPaper.PNG");
	 wetTexs[0] = fileMng_.LoadImageFM("Image/ToiletPaperYogore.PNG");
	 wetTexs[1] = fileMng_.LoadImageFM("Image/ToiletPaperYogore2.PNG");
	 wetTexs[2] = fileMng_.LoadImageFM("Image/ToiletPaperYogore3.PNG");

}

Player::~Player(void)
{

}

bool Player::SystemInit(void)
{
	// システム初期化時にプレイヤー状態を初期化
	positionX_ = 200.0f;
	positionY_ = stage_->GetGroundY();
	velocityX_ = 0.0f;
	velocityY_ = 0.0f;
	onGround_ = true;
	jumpTimer_ = 0;
	dirtLevel_ = 0;
	wasInDiaty_ = false;
	wasInPuddle_ = false;
	stateName_ = "Idle";
	return true;
}

void Player::GameInit(void)
{
	// ゲーム開始時のリセット
	positionX_ = 300.0f;
	positionY_ = stage_->GetGroundY();
	velocityX_ = 0.0f;
	velocityY_ = 0.0f;
	onGround_ = true;
	jumpTimer_ = 0;
	dirtLevel_ = 0;
	wasInPuddle_ = false;
	wasInDiaty_ = false;
	stateName_ = "Idle";
}

void Player::Update(void)
{
	// 旧インターフェース用（入力がない時は更新しない）
}

void Player::Draw(void)
{
	// 旧インターフェース用（カメラ0・画像なしで描画）
	Draw(0.0f, -1);
}

bool Player::Release(void)
{
	return true;
}

void Player::Update(const InputManager& input)
{
	ProcessMove(input);

	if (!onGround_)
	{
		AddGravity();
	}

	ProcessJump(input);

	Move();

	// 水たまり当たり判定
	isInPuddle_ = false;
	const float puddleStart = stage_->GetPuddleStartX();
	const float puddleEnd = stage_->GetPuddleEndX();

	isInDiaty_ = false;
	const float diatyStart = stage_->GetDiatyStartX();
	const float diatyEnd = stage_->GetDiatyEndX();
	// 通常地面に触れており、X座標が水たまりの範囲内なら水たまりと判定
	if (onGround_ && (positionY_ == stage_->GetGroundY()) && 
		(positionX_ >= puddleStart) && (positionX_ <= puddleEnd))
	{
		isInPuddle_ = true;
	}
	if (onGround_ && (positionY_ == stage_->GetGroundY()) &&
		(positionX_ >= diatyStart) && (positionX_ <= diatyEnd))
	{
		isInDiaty_ = true;
	}
	// 水たまりに新しく入った瞬間なら汚れ段階をアップ
	if (isInPuddle_ && !wasInPuddle_)
	{
		if (dirtLevel_ < 3)
		{
			dirtLevel_++;
		}
	}
	wasInPuddle_ = isInPuddle_;

	if (isInDiaty_ && !wasInDiaty_)
	{
		if (dirtLevel_ < 3)
		{
			dirtLevel_++;
		}
	}
	wasInDiaty_ = isInDiaty_;
	// 状態名更新（アニメーション等で利用）
	if (input.IsTrgDown(KEY_INPUT_X))
	{
		stateName_ = "Damage";
	}
	else if (input.IsTrgDown(KEY_INPUT_Z))
	{
		stateName_ = "Attack";
	}
	else if (!onGround_)
	{
		stateName_ = (velocityY_ < 0.0f) ? "Jump" : "Fall";
	}
	else if (std::abs(velocityX_) > 0.1f)
	{
		stateName_ = "Run";
	}
	else
	{
		stateName_ = "Idle";
	}
}

void Player::Draw(float cameraX, int playerGraphHandle) const
{
	const int drawX = static_cast<int>(positionX_ - cameraX);
	const int drawY = static_cast<int>(positionY_);

	// 一度でも水たまりで汚れたら、その状態を維持して描画
	if (dirtLevel_ > 0)
	{
		// 汚れ段階(1?3)に合わせて画像インデックス(0?2)にする
		int targetIndex = dirtLevel_ - 1;
		int animIndex = targetIndex;
		
		// もし指定した画像が読み込めていなければ、前の状態の画像を代わりに表示する
		while (animIndex >= 0 && !wetTexs[animIndex])
		{
			animIndex--;
		}

		if (animIndex >= 0 && wetTexs[animIndex])
		{
			DrawRotaGraph(drawX, drawY - 24, 1.0, 0.0, wetTexs[animIndex]->GetHandle(), TRUE);

#ifdef _DEBUG
			// 読み込み失敗が分かるように上部に原因状態を描画
			if (targetIndex != animIndex) {
				DrawString(0, 100, "Error: missing Yogore Image!", GetColor(255, 0, 0), TRUE);
			}
			DrawFormatString(0, 120, GetColor(255, 255, 255), "DirtLevel: %d  TargetIndex: %d", dirtLevel_, targetIndex);
#endif
			return;
		}
	}

	if (particleTex)
	{
		DrawRotaGraph(drawX, drawY - 24, 1.0, 0.0, particleTex->GetHandle(), TRUE);
		return;
	}
	
	if (playerGraphHandle >= 0)
	{
		DrawRotaGraph(drawX, drawY - (SIZE_Y / 2), 1.0, 0.0, playerGraphHandle, TRUE);
		return;
	}

	const int left = drawX - (SIZE_X / 2);
	const int right = drawX + (SIZE_X / 2);
	const int top = drawY - SIZE_Y;
	const int bottom = drawY;
	DrawBox(left, top, right, bottom, GetColor(120, 220, 255), TRUE);

	// 当たり判定デバッグ描画（当たり判定の幅で赤い枠線を描画）
#ifdef _DEBUG
	const float playerHalfWidth = static_cast<float>(SIZE_X) / 2.0f;
	const int debugLeft = static_cast<int>(positionX_ - playerHalfWidth - cameraX);
	const int debugRight = static_cast<int>(positionX_ + playerHalfWidth - cameraX);
	const int debugTop = static_cast<int>(positionY_ - SIZE_Y); // 描画の下端(bottom)は positionY_ と同じ
	const int debugBottom = static_cast<int>(positionY_);

	DrawBox(debugLeft, debugTop, debugRight, debugBottom, GetColor(255, 0, 0), FALSE); // FALSEで枠線のみ
#endif
}

const char* Player::GetStateName() const
{
	return stateName_;
}

float Player::GetX() const
{
	return positionX_;
}

float Player::GetY() const
{
	return positionY_;
}

// プレイヤーの移動操作
void Player::ProcessMove(const InputManager& input)
{
	// DxLib標準の汎用パッド入力を取得(左スティック・十字キー対応)
	const int padState = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	const float moveInput =
		(input.IsNew(KEY_INPUT_RIGHT) || (padState & PAD_INPUT_RIGHT) ? 1.0f : 0.0f) -
		(input.IsNew(KEY_INPUT_LEFT)  || (padState & PAD_INPUT_LEFT)  ? 1.0f : 0.0f);

	if (moveInput > 0.0f)
	{
		Accele(MOVE_ACC);
	}
	else if (moveInput < 0.0f)
	{
		Accele(-MOVE_ACC);
	}
	else
	{
		Decelerate(MOVE_DEC);
	}
}

// 加速(スピードを加える)
void Player::Accele(float speed)
{
	velocityX_ += speed;
	if (velocityX_ > MAX_MOVE_SPEED) velocityX_ = MAX_MOVE_SPEED;
	if (velocityX_ < -MAX_MOVE_SPEED) velocityX_ = -MAX_MOVE_SPEED;
}

// 減速(ディセラレイト)
void Player::Decelerate(float speed)
{
	if (velocityX_ > 0.0f)
	{
		velocityX_ -= speed;
		if (velocityX_ < 0.0f) velocityX_ = 0.0f;
	}
	else if (velocityX_ < 0.0f)
	{
		velocityX_ += speed;
		if (velocityX_ > 0.0f) velocityX_ = 0.0f;
	}
}

// 移動(実際の座標移動)
void Player::Move(void)
{
	const float playerHalfWidth = static_cast<float>(SIZE_X) / 2.0f;
	const float prevX = positionX_;
	
	positionX_ += velocityX_;

	// 画面（ステージ）外に出ないようにクランプ
	const float clampedStageWidth = (std::max)(0.0f, stage_->GetStageWidth());
	const float minX = playerHalfWidth;
	const float maxX = (std::max)(minX, clampedStageWidth - playerHalfWidth);
	positionX_ = (std::max)(minX, (std::min)(positionX_, maxX));

	// 段差より下にいる時だけ側面衝突を有効にする
	const float stepTopY = stage_->GetStepTopY();
	const float stepStartX = stage_->GetStepStartX();
	const float stepEndX = stage_->GetStepEndX();
	const float step2TopY = stage_->GetStep2TopY();
	const float step2StartX = stage_->GetStep2StartX();
	const float step2EndX = stage_->GetStep2EndX();
	const bool isBelowStepTop = (positionY_ > stepTopY, positionY_ > step2TopY + 0.5f);
	const bool isBelowStep2Top = (positionY_ > step2TopY + 0.5f);
	if (isBelowStepTop)
	{
		// 新しい座標が壁の中に入っているか
		if (positionX_ + playerHalfWidth > stepStartX && positionX_ , playerHalfWidth > stepStartX && positionX_ - playerHalfWidth < stepEndX)
		{
			// 古い座標を用いて左からぶつかったか、右からぶつかったか判定
			if (prevX + playerHalfWidth <= stepStartX)
			{
				positionX_ = stepStartX - playerHalfWidth;
				velocityX_ = 0.0f;
			}
			else if (prevX - playerHalfWidth >= stepEndX)
			{
				positionX_ = stepEndX + playerHalfWidth;
				velocityX_ = 0.0f;
			}
			else
			{
				// 上から落ちてきた場合などの押し出し（念のため中央より近い方へ押し出す）
				float stepMidX = stepStartX + (stepEndX - stepStartX) * 0.5f;
				if (positionX_ < stepMidX)
				{
					positionX_ = stepStartX - playerHalfWidth;
				}
				else
				{
					positionX_ = stepEndX + playerHalfWidth;
				}
				velocityX_ = 0.0f;
			}
			// 古い座標を用いて左からぶつかったか、右からぶつかったか判定
			if (prevX + playerHalfWidth <= step2StartX)
			{
				positionX_ = step2StartX - playerHalfWidth;
				velocityX_ = 0.0f;
			}
			else if (prevX - playerHalfWidth >= step2EndX)
			{
				positionX_ = stepEndX + playerHalfWidth;
				velocityX_ = 0.0f;
			}
			else
			{
				// 上から落ちてきた場合などの押し出し（念のため中央より近い方へ押し出す）
				float stepMidX = step2StartX + (step2EndX - step2StartX) * 0.5f;
				if (positionX_ < stepMidX)
				{
					positionX_ = stepStartX - playerHalfWidth;
				}
				else
				{
					positionX_ = step2EndX + playerHalfWidth;
				}
				velocityX_ = 0.0f;
				if (prevX + playerHalfWidth <= step2StartX)
				{
					positionX_ = step2StartX - playerHalfWidth;
					velocityX_ = 0.0f;
				}
				else if (prevX - playerHalfWidth >= step2EndX)
				{
					positionX_ = stepEndX + playerHalfWidth;
					velocityX_ = 0.0f;
				}
				else
				{
					// 上から落ちてきた場合などの押し出し（念のため中央より近い方へ押し出す）
					float stepMidX = stepStartX + (stepEndX - stepStartX	) * 0.5f;
					if (positionX_ < stepMidX)
					{
						positionX_ = stepStartX - playerHalfWidth;
					}
					else
					{
						positionX_ = stepEndX + playerHalfWidth;
					}
					velocityX_ = 0.0f;
				}
			}
		}
	}

	// 足元の床高さを取得（プレイヤーの幅を考慮して一番高い床に乗る）
	const float supportYCenter = stage_->GetGroundYAtX(positionX_);
	const float supportYLeft = stage_->GetGroundYAtX(positionX_ - playerHalfWidth + 1.0f);
	const float supportYRight = stage_->GetGroundYAtX(positionX_ + playerHalfWidth - 1.0f);
	const float supportGroundY = (std::min)(supportYCenter, (std::min)(supportYLeft, supportYRight));

	if (onGround_ && (positionY_ < supportGroundY - 0.5f))
	{
		onGround_ = false;
	}

	positionY_ += velocityY_;
	if (positionY_ >= supportGroundY)
	{
		positionY_ = supportGroundY;
		velocityY_ = 0.0f;
		onGround_ = true;
	}
}

// 重力をかける
void Player::AddGravity(void)
{
	velocityY_ += GRAVITY;
}

// プレイヤーのジャンプ操作
void Player::ProcessJump(const InputManager& input)
{
	// SPACEキー または コントローラのAボタン(JOYPAD_BTN::DOWN が Aボタンに該当)
	const bool jumpTrg = input.IsTrgDown(KEY_INPUT_SPACE) || input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
	const bool jumpHold = input.IsNew(KEY_INPUT_SPACE)    || input.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	// マリオ風フレームジャンプ：押した瞬間にジャンプ開始、押し続けで一定フレーム上昇
	if (onGround_ && jumpTrg)
	{
		onGround_ = false;
		jumpTimer_ = INPUT_JUMP_FRAME;
		Jump(); // 初速を与える
	}
	else if (!onGround_)
	{
		// 空中かつボタンを押し続けているなら上昇力を維持（大ジャンプ）
		if (jumpHold && jumpTimer_ > 0)
		{
			jumpTimer_--;
			Jump();
		}
		// ボタンを離した、かつまだ上昇中なら速度をカット（小ジャンプ）
		else if (!jumpHold && velocityY_ < 0.0f)
		{
			jumpTimer_ = 0;
			velocityY_ *= 0.5f; // 上昇力を半減させて急落下させる
		}
		else
		{
			jumpTimer_ = 0;
		}
	}
}

// ジャンプ
void Player::Jump(void)
{
	SetJumpPow(MAX_JUMP_POW);
}

// ジャンプ力の設定
void Player::SetJumpPow(float pow)
{
	velocityY_ = -pow;
}