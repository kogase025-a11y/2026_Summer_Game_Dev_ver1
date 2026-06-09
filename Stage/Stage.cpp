#include "Stage.h"
#include <DxLib.h>

Stage::Stage()
    : groundY_(760.0f)
    , stepStartX_(1400.0f)
    , stepEndX_(2200.0f)
    , stepTopY_(640.0f)
    , step2StartX_(2700.0f)
    , step2EndX_(3000.0f)
    , step2TopY_(640.0f)
    , step3StartX_(3600.0f)
    , step3EndX_(3800.0f)
    , step3TopY_(600.0f)
    , goalX_(4700.0f)
    , stageWidth_(5000.0f)
    , puddleStartX_(800.0f)
    , puddleEndX_(1200.0f)
	, diatyStartX_(3000.0f)
	, diatyEndX_(3600.0f)
	, slopeStartX_(4000.0f)
	, slopeEndX_(4500.0f)
{
}


void Stage::Init(int stageNum)
{
    if (stageNum == 1)
    {
        // --- ステージ1の地形設定 ---
        groundY_ = 760.0f;
        stepStartX_ = 1400.0f;
        stepEndX_ = 2200.0f;
        stepTopY_ = 640.0f;

        puddleStartX_ = 800.0f;
        puddleEndX_ = 1200.0f;

        goalX_ = 4700.0f; // ステージ1は長め
        stageWidth_ = 5000.0f;
    }
    else if (stageNum == 2)
    {
        // --- ステージ2の地形設定（例：段差の位置やゴールを少し手前に変えてみる） ---
        groundY_ = 760.0f;
        stepStartX_ = 800.0f;   // 最初からいきなり段差がある
        stepEndX_ = 1500.0f;
        stepTopY_ = 600.0f;   // ちょっと高めの段差

        puddleStartX_ = 2000.0f;  // 水たまりの位置を後ろにする
        puddleEndX_ = 2400.0f;

        goalX_ = 3200.0f;  // ステージ2は少し短め
        stageWidth_ = 3500.0f;
    }
    else if (stageNum == 3)
    {
        // --- ステージ3の地形設定（例：さらに違う配置にする） ---
        groundY_ = 760.0f;
        stepStartX_ = 2000.0f;
        stepEndX_ = 2500.0f;
        stepTopY_ = 550.0f;

        puddleStartX_ = 500.0f;
        puddleEndX_ = 900.0f;

        goalX_ = 4000.0f;
        stageWidth_ = 4300.0f;
    }
}

void Stage::Update()
{
}

void Stage::Draw(float cameraX, int screenWidth, int screenHeight) const
{
    // 床の描画
    const int groundY = static_cast<int>(groundY_);
    DrawBox(0, groundY, screenWidth, screenHeight, GetColor(60, 170, 60), TRUE);

    // 水たまりの描画(床と重なるように青色で描画)
    const int puddleLeft = static_cast<int>(puddleStartX_  - cameraX);
    const int puddleRight = static_cast<int>(puddleEndX_   - cameraX);
    DrawBox(puddleLeft, groundY, puddleRight, groundY + 20, GetColor(50, 50, 200), TRUE);
   
    const int diatyLeft = static_cast<int>(diatyStartX_ - cameraX);
    const int diatyRight = static_cast<int>(diatyEndX_ - cameraX);
    DrawBox(diatyLeft, groundY, diatyRight, groundY + 20, GetColor(50, 50, 200), TRUE);

    
    // 段差の描画
    const int stepLeft = static_cast<int>(stepStartX_ - cameraX);
    const int stepRight = static_cast<int>(stepEndX_ - cameraX);
    const int stepTop = static_cast<int>(stepTopY_);
    DrawBox(stepLeft, stepTop, stepRight, groundY, GetColor(110, 110, 110), TRUE);

    // 段差の描画
    const int step2Left = static_cast<int>(step2StartX_ - cameraX);
    const int step2Right = static_cast<int>(step2EndX_ - cameraX);
    const int step2Top = static_cast<int>(step2TopY_);
    DrawBox(step2Left, step2Top, step2Right, groundY, GetColor(110, 110, 110), TRUE);

    const int step3Left = static_cast<int>(step3StartX_ - cameraX);
    const int step3Right = static_cast<int>(step3EndX_ - cameraX);
    const int step3Top = static_cast<int>(step3TopY_);
    DrawBox(step3Left, step3Top, step3Right, groundY, GetColor(110, 110, 110), TRUE);
    //坂の描写
	const int slopeLeft = static_cast<int>(slopeStartX_ - cameraX);
	const int slopeRight = static_cast<int>(slopeEndX_ - cameraX);
	DrawTriangle(slopeLeft, groundY, slopeRight, groundY, slopeRight, groundY - 200, GetColor(110, 110, 110), TRUE);
    // ゴールの描画

    const int goalDrawX = static_cast<int>(goalX_ - cameraX);
    DrawBox(goalDrawX - 8, groundY - 180, goalDrawX + 8, groundY, GetColor(255, 255, 255), TRUE);
    DrawTriangle(goalDrawX + 8, groundY - 180, goalDrawX + 72, groundY - 150, goalDrawX + 8, groundY - 120, GetColor(255, 80, 80), TRUE);
}


float Stage::GetGroundYAtX(float x) const
{
    if (x >= stepStartX_ && x <= stepEndX_)
    {
        return stepTopY_;
    }
    if (x >= step2StartX_ && x <= step2EndX_)
    {
        return step2TopY_;
    }
    if (x >= step3StartX_ && x <= step3EndX_)
    {
        return step3TopY_;
    }
    
    if (x >= slopeStartX_ && x <= slopeEndX_)
    {
        const float slopeHeight = 200.0f; // 傾斜の高さ
        const float slopeWidth = slopeEndX_ - slopeStartX_;
        const float slopeProgress = (x - slopeStartX_) / slopeWidth;
        return groundY_ - slopeHeight * slopeProgress;
	}
    
    return groundY_;
    



}

float Stage::GetGroundY() const { return groundY_; }
float Stage::GetStepStartX() const { return stepStartX_; }
float Stage::GetStepEndX() const { return stepEndX_; }
float Stage::GetStepTopY() const { return stepTopY_; }

float Stage::GetStep2StartX() const { return step2StartX_; }
float Stage::GetStep2EndX() const { return step2EndX_; }
float Stage::GetStep2TopY() const { return step2TopY_; }

float Stage::GetStep3StartX() const { return step3StartX_; }
float Stage::GetStep3EndX() const { return step3EndX_; }
float Stage::GetStep3TopY() const { return step3TopY_; }


float Stage::GetSlopeStartX() const { return slopeStartX_; }
float Stage::GetSlopeEndX() const { return slopeEndX_; }
float Stage::GetSlopeStartY() const { return groundY_; }

   
float Stage::GetGoalX() const { return goalX_; }
float Stage::GetStageWidth() const { return stageWidth_; }

float Stage::GetPuddleStartX() const { return puddleStartX_; }
float Stage::GetPuddleEndX() const { return puddleEndX_; }

float Stage::GetDiatyStartX() const { return diatyStartX_; }
float Stage::GetDiatyEndX() const { return diatyEndX_; }

