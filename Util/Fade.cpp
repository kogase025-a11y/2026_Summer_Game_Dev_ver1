#include "Fade.h"
#include <DxLib.h>

Fade::Fade() : state_(State::None), timer_(0.0f), duration_(1.0f) {}

Fade::~Fade() = default;

void Fade::Start(State state, float duration)
{
	state_ = state;
	duration_ = duration;
	timer_ = 0.0f;
}

void Fade::Update()
{
	if (state_ != State::None)
	{
		timer_ += 1.0f;
		if (timer_ > duration_)
		{
			timer_ = duration_;
		}
	}
}

void Fade::Draw()
{
	if (state_ == State::None) return;

	float t = GetProgress();
	int alpha = 0;

	if (state_ == State::Out) // フェードアウト（明→暗）
	{
		alpha = static_cast<int>(t * 255.0f);
	}
	else if (state_ == State::In) // フェードイン（暗→明）
	{
		alpha = static_cast<int>((1.0f - t) * 255.0f);
	}

	if (alpha > 0)
	{
		int w, h;
		GetDrawScreenSize(&w, &h);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawBox(0, 0, w, h, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void Fade::DrawBlack()
{
	int w, h;
	GetDrawScreenSize(&w, &h);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawBox(0, 0, w, h, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

float Fade::GetProgress() const
{
	if (duration_ <= 0.0f) return 1.0f;
	float v = timer_ / duration_;
	if (v < 0.0f) return 0.0f;
	if (v > 1.0f) return 1.0f;
	return v;
}
