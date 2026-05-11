#include "Stage.h"
#include <DxLib.h>

Stage::Stage()
    : groundY_(760.0f)
    , stepStartX_(1800.0f)
    , stepEndX_(2500.0f)
    , stepTopY_(640.0f)
    , goalX_(4700.0f)
    , stageWidth_(5000.0f)
{
}

void Stage::Update()
{
}

void Stage::Draw(float cameraX, int screenWidth, int screenHeight) const
{
    // è∞ÇÃï`âÊ
    const int groundY = static_cast<int>(groundY_);
    DrawBox(0, groundY, screenWidth, screenHeight, GetColor(60, 170, 60), TRUE);

    // íiç∑ÇÃï`âÊ
    const int stepLeft = static_cast<int>(stepStartX_ - cameraX);
    const int stepRight = static_cast<int>(stepEndX_ - cameraX);
    const int stepTop = static_cast<int>(stepTopY_);
    DrawBox(stepLeft, stepTop, stepRight, groundY, GetColor(110, 110, 110), TRUE);

    // ÉSÅ[ÉãÇÃï`âÊ
    const int goalDrawX = static_cast<int>(goalX_ - cameraX);
    DrawBox(goalDrawX - 8, groundY - 180, goalDrawX + 8, groundY, GetColor(255, 255, 255), TRUE);
    DrawTriangle(goalDrawX + 8, groundY - 180, goalDrawX + 72, groundY - 150, goalDrawX + 8, groundY - 120, GetColor(255, 80, 80), TRUE);
}

float Stage::GetGroundY() const { return groundY_; }
float Stage::GetStepStartX() const { return stepStartX_; }
float Stage::GetStepEndX() const { return stepEndX_; }
float Stage::GetStepTopY() const { return stepTopY_; }

float Stage::GetGroundYAtX(float x) const
{
    if (x >= stepStartX_ && x <= stepEndX_)
    {
        return stepTopY_;
    }
    return groundY_;
}

float Stage::GetGoalX() const { return goalX_; }
float Stage::GetStageWidth() const { return stageWidth_; }
