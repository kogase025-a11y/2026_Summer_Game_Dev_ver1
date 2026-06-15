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
	 jumpSe_ = fileMng_.LoadSoundFM("Image/Sound/jump03.mp3");
	 goalSe_ = fileMng_.LoadSoundFM("Image/Sound/Level up! Ragnarok.mp3");
	 puddleSe_ = fileMng_.LoadSoundFM("Image/Sound/powerdown07.mp3");
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
	invincibleTimer_ = 0.0f;
	isInvincible_ = false;
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

	// --- 無敵タイマーの更新 ---
	if (invincibleTimer_ > 0) {
		invincibleTimer_ -= (1.0f / 60.0f); // 60FPSと仮定。本来はdeltaTimeを渡すのが理想
		if (invincibleTimer_ <= 0) {
			invincibleTimer_ = 0;
			isInvincible_ = false;
		}
	}



	// ★追加：速度に合わせて回転させる（0.05fは回転スピードの調整値）
	angle_ += velocityX_ * 0.05f;

	bool wasOnGround = onGround_;
	Move();
	bool landedThisFrame = (!wasOnGround && onGround_);

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
	// --- 水たまり当たり判定の修正 ---
	if (isInPuddle_ && !wasInPuddle_)
	{
		// 無敵中でなければ汚れを増やす
		if (!isInvincible_) {
			if (dirtLevel_ < 3) dirtLevel_++;
			if (puddleSe_ && puddleSe_->GetHandle() != -1)
				PlaySoundMem(puddleSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
		}
	}
	// (isInDiaty_ の方も同様に !isInvincible_ で囲む)
	
	// (isInDiaty_ の方も同様に !isInvincible_ で囲む)
	/*else if (landedThisFrame)
	{
		if (fallSe_ && fallSe_->GetHandle() != -1)
		{
			PlaySoundMem(fallSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
		}
	}*/
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
	
	// --- 無敵中は点滅させる演出 ---
	if (isInvincible_) {
		// 5フレームに1回描画しない（点滅）
		if ((GetNowCount() / 100) % 2 == 0) return;
	}


	// 画面上の足元位置
	const int drawX = static_cast<int>(positionX_ - cameraX);
	const int drawY = static_cast<int>(positionY_);

	// ─── 【完全固定】画像の「中心の穴」を描画するY座標 ───
	// 足元から「32ピクセル上」を画像の中心点（穴）にします。
	const int imageCenterY = drawY - 32;

	// 1. 水たまりで汚れた時の画像を描画
	if (dirtLevel_ > 0)
	{
		int targetIndex = dirtLevel_ - 1;
		int animIndex = targetIndex;

		while (animIndex >= 0 && !wetTexs[animIndex])
		{
			animIndex--;
		}

		if (animIndex >= 0 && wetTexs[animIndex])
		{
			DrawRotaGraph(drawX, imageCenterY, 1.0, 0.0, wetTexs[animIndex]->GetHandle(), TRUE);
		}
	}
	// 2. 通常時の画像（particleTex）を描画
	else if (particleTex)
	{
		DrawRotaGraph(drawX, imageCenterY, 1.0, 0.0, particleTex->GetHandle(), TRUE);
	}
	// 3. 外部からグラフィックハンドルが指定された場合
	else if (playerGraphHandle >= 0)
	{
		DrawRotaGraph(drawX, imageCenterY, 1.0, 0.0, playerGraphHandle, TRUE);
	}
	// 4. 画像が何もないときの仮の青い四角
	else
	{
		DrawBox(drawX - 32, drawY - 64, drawX + 32, drawY, GetColor(120, 220, 255), TRUE);
	}

	// 【当たり判定の確認】赤い枠が見たい場合はこれを残す
	 DrawBox(drawX - 20, drawY - 64, drawX + 20, drawY, GetColor(255, 0, 0), FALSE);
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
	//const float halfW = 20.0f; // 当たり判定の横幅（見た目より少し小さくするとスムーズ）
	//const float prevX_ = positionX_; 

	//// 1. 横に移動してみる
	//positionX_ += velocityX_;

	//// 2. 移動した先がブロックだったら、元の場所（prevX）に戻して速度を0にする
	// //※足元(positionY_ - 5)と頭の上(positionY_ - 60)の2箇所でチェック
	//if (stage_->IsBlock(positionX_ + halfW, positionY_ - 5.0f) ||
	//	stage_->IsBlock(positionX_ + halfW, positionY_ - 60.0f) ||
	//	stage_->IsBlock(positionX_ - halfW, positionY_ - 5.0f) ||
	//	stage_->IsBlock(positionX_ - halfW, positionY_ - 60.0f))
	//{
	//	positionX_ = prevX_; // ぶつかったので戻す
	//	velocityX_ = 0.0f;  // 速度も止める
	//}

	//// 3. 縦の移動（重力など）と着地判定（ここは今のロジックでOK）
	//positionY_ += velocityY_;
	//float supportY = stage_->GetGroundYAtX(positionX_);
	//if (positionY_ >= supportY) {
	//	positionY_ = supportY;
	//	velocityY_ = 0.0f;
	//	onGround_ = true;
	//}
	//else {
	//	onGround_ = false;
	//}



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

	const float step3TopY = stage_->GetStep3TopY();
	const float step3StartX = stage_->GetStep3StartX();
	const float step3EndX = stage_->GetStep3EndX();

	const float slopeStartX = stage_->GetSlopeStartX();
	const float slopeEndX = stage_->GetSlopeEndX();
	const float slopeTopY = stage_->GetSlopeStartY() - 200.0f;

	const bool isBelowStepTop = (positionY_ > stepTopY + 0.5f);
	const bool isBelowStep2Top = (positionY_ > step2TopY + 0.5f);
	const bool isBelowStep3Top = (positionY_ > step3TopY + 0.5f);
	const bool isBelowSlopeTop = (positionY_ > slopeTopY + 0.5f);

	if (isBelowStepTop)
	{
		// 新しい座標が壁の中に入っているか
		if (positionX_ + playerHalfWidth > stepStartX && positionX_ - playerHalfWidth < stepEndX)
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
		}
	}

	if (isBelowStep2Top)
	{
		// 新しい座標が壁の中に入っているか
		if (positionX_ + playerHalfWidth > step2StartX && positionX_ - playerHalfWidth < step2EndX)
		{
			// 古い座標を用いて左からぶつかったか、右からぶつかったか判定
			if (prevX + playerHalfWidth <= step2StartX)
			{
				positionX_ = step2StartX - playerHalfWidth;
				velocityX_ = 0.0f;
			}
			else if (prevX - playerHalfWidth >= step2EndX)
			{
				positionX_ = step2EndX + playerHalfWidth;
				velocityX_ = 0.0f;
			}
			else
			{
				// 上から落ちてきた場合などの押し出し（念のため中央より近い方へ押し出す）
				float stepMidX = step2StartX + (step2EndX - step2StartX) * 0.5f;
				if (positionX_ < stepMidX)
				{
					positionX_ = step2StartX - playerHalfWidth;
				}
				else
				{
					positionX_ = step2EndX + playerHalfWidth;
				}
				velocityX_ = 0.0f;
			}
		}
	}

	if (isBelowStep3Top)
	{
		// 新しい座標が壁の中に入っているか
		if (positionX_ + playerHalfWidth > step3StartX && positionX_ - playerHalfWidth < step3EndX)
		{
			// 古い座標を用いて左からぶつかったか、右からぶつかったか判定
			if (prevX + playerHalfWidth <= step3StartX)
			{
				positionX_ = step3StartX - playerHalfWidth;
				velocityX_ = 0.0f;
			}
			else if (prevX - playerHalfWidth >= step3EndX)
			{
				positionX_ = step3EndX + playerHalfWidth;
				velocityX_ = 0.0f;
			}
			else
			{
				// 上から落ちてきた場合などの押し出し（念のため中央より近い方へ押し出す）
				float stepMidX = step3StartX + (step3EndX - step3StartX) * 0.5f;
				if (positionX_ < stepMidX)
				{
					positionX_ = step3StartX - playerHalfWidth;
				}
				else
				{
					positionX_ = step3EndX + playerHalfWidth;
				}
				velocityX_ = 0.0f;
			}
		}
	}

	if (isBelowSlopeTop)
	{
		// 坂道（△）は左から登れるため、右側の絶壁のみ横当たり判定を行う
		if (positionX_ - playerHalfWidth < slopeEndX && prevX - playerHalfWidth >= slopeEndX)
		{
			positionX_ = slopeEndX + playerHalfWidth;
			velocityX_ = 0.0f;
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

		if (jumpSe_ && jumpSe_->GetHandle() != -1)
		{
			PlaySoundMem(jumpSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
		}
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
void Player::PlayGoalSound()
{
	if (goalSe_ && goalSe_->GetHandle() != -1)
	{
		PlaySoundMem(goalSe_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
	}
}
void Player::StartInvincible(float seconds) 
{
	invincibleTimer_ = seconds;
	isInvincible_ = true;
}