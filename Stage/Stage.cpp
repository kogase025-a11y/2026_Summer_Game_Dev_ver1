#include "Stage.h"
#include <DxLib.h>

Stage::Stage()
    : groundY_(760.0f)
    , stepStartX_(1800.0f)
    , stepEndX_(2500.0f)
    , stepTopY_(640.0f)
    , goalX_(4700.0f)
    , stageWidth_(5000.0f)
    , puddleStartX_(800.0f)
    , puddleEndX_(1200.0f)
{
}

void Stage::Update()
{
}

void Stage::Draw(float cameraX, int screenWidth, int screenHeight) const
{
    // °‚Ì•`‰æ
    const int groundY = static_cast<int>(groundY_);
    DrawBox(0, groundY, screenWidth, screenHeight, GetColor(60, 170, 60), TRUE);

    // …‚½‚Ü‚è‚Ì•`‰æ(°‚Æd‚È‚é‚æ‚¤‚ÉÂF‚Å•`‰æ)
    const int puddleLeft = static_cast<int>(puddleStartX_ - cameraX);
    const int puddleRight = static_cast<int>(puddleEndX_ - cameraX);
    DrawBox(puddleLeft, groundY, puddleRight, groundY + 20, GetColor(50, 50, 200), TRUE);

    // ’i·‚Ì•`‰æ
    const int stepLeft = static_cast<int>(stepStartX_ - cameraX);
    const int stepRight = static_cast<int>(stepEndX_ - cameraX);
    const int stepTop = static_cast<int>(stepTopY_);
    DrawBox(stepLeft, stepTop, stepRight, groundY, GetColor(110, 110, 110), TRUE);

    // ƒS[ƒ‹‚Ì•`‰æ
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

float Stage::GetPuddleStartX() const { return puddleStartX_; }
float Stage::GetPuddleEndX() const { return puddleEndX_; }
