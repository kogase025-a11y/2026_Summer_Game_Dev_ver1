#include "Stage.h"
#include "../Manager/FileManager.h"
#include <DxLib.h>
#include <algorithm>

Stage::Stage()
    : groundY_(760.0f)
    , stepStartX_(1400.0f), stepEndX_(2200.0f), stepTopY_(640.0f)
    , step2StartX_(2700.0f), step2EndX_(3000.0f), step2TopY_(640.0f)
    , step3StartX_(3600.0f), step3EndX_(3800.0f), step3TopY_(600.0f)
    , goalX_(4700.0f)
    , stageWidth_(5000.0f)
    , puddleStartX_(800.0f), puddleEndX_(1200.0f)
    , dirtyStartX_(3000.0f), dirtyEndX_(3600.0f)
	, baketuStartX_(1000.0f), baketuEndX_(1400.0f)
    
{
}

void Stage::Init(int stageNum, FileManager& fileMng)
{
    goalTex_ = fileMng.LoadImageFM("Image/GoalMan.png");

    // --- マップデータの初期化 ---
    for (int y = 0; y < kMapHeight; y++) {
        for (int x = 0; x < kMapWidth; x++) {
            mapData_[y][x] = 0;
        }
    }

    // 一番下の行を地面にする
    for (int x = 0; x < kMapWidth; x++) {
        mapData_[kMapHeight - 1][x] = 1;
    }

    // 特定の場所にブロックを配置
    mapData_[15][10] = 1;
    mapData_[15][11] = 1;

    // --- ステージ別設定 ---
    if (stageNum == 1) {
        groundY_ = 760.0f;
        stepStartX_ = 1400.0f;
        stepEndX_ = 2200.0f;
        stepTopY_ = 640.0f;
        puddleStartX_ = 800.0f;
        puddleEndX_ = 1200.0f;
        goalX_ = 4700.0f;
        stageWidth_ = 5000.0f;
    }
    else if (stageNum == 2) {
        groundY_ = 760.0f;
        stepStartX_ = 800.0f;
        stepEndX_ = 1500.0f;
        stepTopY_ = 600.0f;
        puddleStartX_ = 2000.0f;
        puddleEndX_ = 2400.0f;
        goalX_ = 3200.0f;
        stageWidth_ = 3500.0f;
    }
    else if (stageNum == 3) {
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
    const int groundY = static_cast<int>(groundY_);

    // 1. 基本の床（緑）
    DrawBox(0, groundY, screenWidth, screenHeight, GetColor(60, 170, 60), TRUE);

    // 2. 水たまり・汚れ地面
    const int pLeft = static_cast<int>(puddleStartX_ - cameraX);
    const int pRight = static_cast<int>(puddleEndX_ - cameraX);
    DrawBox(pLeft, groundY, pRight, groundY + 20, GetColor(50, 50, 200), TRUE);

    const int dLeft = static_cast<int>(dirtyStartX_ - cameraX);
    const int dRight = static_cast<int>(dirtyEndX_ - cameraX);
    DrawBox(dLeft, groundY, dRight, groundY + 20, GetColor(50, 50, 200), TRUE);

    // 3. 段差の描画 (1, 2, 3)
    auto DrawStep = [&](float start, float end, float top, unsigned int color) {
        DrawBox(static_cast<int>(start - cameraX), static_cast<int>(top),
            static_cast<int>(end - cameraX), groundY, color, TRUE);
        };
    unsigned int gray = GetColor(110, 110, 110);
    DrawStep(stepStartX_, stepEndX_, stepTopY_, gray);
    DrawStep(step2StartX_, step2EndX_, step2TopY_, gray);
    DrawStep(step3StartX_, step3EndX_, step3TopY_, gray);

   
    

    // 5. ゴールの描画
    if (goalTex_) {
        const int goalDrawX = static_cast<int>(goalX_ - cameraX);
        int handle = goalTex_->GetHandle();
        int w, h;
        GetGraphSize(handle, &w, &h);

        float scale = 0.3f;   // ★大きさ調整
        double angle = 0.0;
        float drawY = groundY - (h * scale / 2.0f);

        DrawRotaGraph(goalDrawX, static_cast<int>(drawY), static_cast<double>(scale), angle, handle, TRUE);
    }

    // 6. mapData_ に基づくタイルの描画
    for (int y = 0; y < kMapHeight; y++) {
        for (int x = 0; x < kMapWidth; x++) {
            if (mapData_[y][x] == 1) {
                int drawX = x * kTileSize - static_cast<int>(cameraX);
                int drawY = y * kTileSize;
                DrawBox(drawX, drawY, drawX + kTileSize, drawY + kTileSize, GetColor(150, 100, 50), TRUE);
                DrawBox(drawX, drawY, drawX + kTileSize, drawY + kTileSize, GetColor(0, 0, 0), FALSE); // 枠
            }
        }
    }
}

/*
// --- 最適化されたタイル描画ロジック（コメント保持） ---
void Stage::DrawOptimized(float cameraX, int screenWidth, int screenHeight) const
{
    int startX = static_cast<int>(cameraX) / kTileSize;
    int endX = (static_cast<int>(cameraX) + screenWidth) / kTileSize + 1;
    for (int y = 0; y < kMapHeight; y++) {
        for (int x = startX; x < endX; x++) {
            if (x < 0 || x >= kMapWidth) continue;
            if (mapData_[y][x] == 1) { ... }
        }
    }
}
*/

float Stage::GetGroundYAtX(float x) const
{
    // 1. 段差チェック
    if (x >= stepStartX_ && x <= stepEndX_)   return stepTopY_;
    if (x >= step2StartX_ && x <= step2EndX_) return step2TopY_;
    if (x >= step3StartX_ && x <= step3EndX_) return step3TopY_;

    

    return groundY_;
}

bool Stage::IsWall(float x, float y) const
{
    int tx = static_cast<int>(x) / kTileSize;
    int ty = static_cast<int>(y) / kTileSize;
    if (tx < 0 || tx >= kMapWidth || ty < 0 || ty >= kMapHeight) return false;
    return (mapData_[ty][tx] == 1);
}

bool Stage::IsBlock(float x, float y) const
{
    int tx = static_cast<int>(x) / kTileSize;
    int ty = static_cast<int>(y) / kTileSize;
    if (tx < 0 || tx >= kMapWidth || ty < 0 || ty >= kMapHeight) return false;
    return (mapData_[ty][tx] == 1);
}

// --- シンプルなアクセッサ群 ---
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

float Stage::GetGoalX() const { return goalX_; }
float Stage::GetStageWidth() const { return stageWidth_; }
float Stage::GetPuddleStartX() const { return puddleStartX_; }
float Stage::GetPuddleEndX() const { return puddleEndX_; }
float Stage::GetDirtyStartX() const { return dirtyStartX_; }
float Stage::GetDirtyEndX() const { return dirtyEndX_; }