#include "Stage.h"
#include "../Manager/FileManager.h"
#include <DxLib.h>
#include <string> 
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

Stage::Stage()
    : groundY_(2000.0f) // 奈落（穴）の設定
    , goalX_(4700.0f)
    , stageWidth_(5000.0f)
{
    // 画像ハンドル配列を初期化
    for (int i = 0; i < kMaxChips; i++) {
        chipHandles_[i] = -1;
    }
}

void Stage::Init(int stageNum, FileManager& fileMng)
{
    goalTex_ = fileMng.LoadImageFM("Image/GoalMan.png");

    // --- 1. マップチップ画像の分割読み込み (1800x1800画像専用設定) ---
    
    // 横1800px / 20px = 90枚。横に90枚並んでいる設定です。
    // 1800枚分読み込み, 横90枚, 縦20枚, チップ幅20px, チップ高さ20px
    int result = LoadDivGraph("Image/Map/TileSet.png", 1800, 90, 20, 20, 20, chipHandles_);

    if (result == -1) {
        // デバッグ用メッセージ（ビルド後に左上に出ます）
        printfDx("【エラー】Image/Map/TileSet.png が読み込めません！サイズかパスを確認してください。\n");
    }

    // --- 2. マップデータの全クリア ---
    for (int y = 0; y < kMapHeight; y++) {
        for (int x = 0; x < kMapWidth; x++) mapData_[y][x] = 0;
    }
    // --- 2. ステージ番号によって読み込むファイルを変える ---
   // stageNumが 1 なら "Data/Stage1.csv"
   // stageNumが 2 なら "Data/Stage2.csv" を読み込むようになります
    std::string csvPath = "Data/Stage" + std::to_string(stageNum) + ".csv";

    // --- 3. CSVファイルの読み込み ---
    LoadMapCSV(csvPath);

    // --- 3. ステージごとの個別の設定（幅やゴールの位置） ---
    if (stageNum == 1) {
        stageWidth_ = 6400.0f;
        goalX_ = 6000.0f;
    }
    else if (stageNum == 2) {
        // ステージ2は少し短めにする、などの設定が可能です
        stageWidth_ = 4000.0f;
        goalX_ = 3700.0f;
    }

    // ステージの横幅をCSVに合わせて自動計算
    stageWidth_ = kMapWidth * kTileSize;
}

void Stage::Update()
{
}

/**
 * 描画：20pxのチップを64pxに拡大して描画
 */
void Stage::Draw(float cameraX, int screenWidth, int screenHeight) const
{
    int startX = static_cast<int>(cameraX) / kTileSize;
    int endX = (static_cast<int>(cameraX) + screenWidth) / kTileSize + 1;
    if (endX > kMapWidth) endX = kMapWidth;

    for (int y = 0; y < kMapHeight; y++) {
        for (int x = startX; x < endX; x++) {
            int tileType = mapData_[y][x];
            if (tileType <= 0) continue;

            int drawX = x * kTileSize - static_cast<int>(cameraX);
            int drawY = y * kTileSize;

            // 1. チップ画像の描画（今の処理）
            if (tileType < kMaxChips && chipHandles_[tileType] != -1) {
                DrawExtendGraph(drawX, drawY, drawX + kTileSize, drawY + kTileSize, chipHandles_[tileType], TRUE);
            }

            // 2. ★追加：マップチップ番号の描画
            // 黄色い文字で見やすく番号を表示します
            DrawFormatString(drawX + 2, drawY + 2, GetColor(255, 255, 0), "%d", tileType);
        }
    }

    // ゴール(GoalMan)の描画
    if (goalTex_) {
        const int goalDrawX = static_cast<int>(goalX_ - cameraX);
        float groundY = GetGroundYAtX(goalX_);
        if (groundY >= 2000.0f) groundY = 768.0f; // 地面がない時の保険

        int handle = goalTex_->GetHandle();
        int w, h; GetGraphSize(handle, &w, &h);
        float scale = 0.3f;
        int goalDrawY = static_cast<int>(groundY - (h * scale / 2.0f));
        DrawRotaGraph(goalDrawX, goalDrawY, static_cast<double>(scale), 0.0, handle, TRUE);
    }
}

/**
 * ★当たり判定の「仕分け」設定
 * 背景（通り抜けたいチップ）のIDをここで除外します
 */
bool Stage::IsSolid(int tileID) const {
    // IDが 60以上 かつ 69以下 の場合だけ true（地面）を返す
    if (tileID >= 60 && tileID <= 69) {
        return true;
    }

    if (tileID >= 70 && tileID <= 79) return true;
	if (tileID == 910) return true;
    // それ以外（背景の487番や1264番など）はすべて false（通り抜け）
    return false;
}

float Stage::GetGroundYAtX(float x) const
{
    int tx = static_cast<int>(x) / kTileSize;
    if (tx < 0 || tx >= kMapWidth) return groundY_;

    for (int ty = 0; ty < kMapHeight; ty++) {
        // IsSolidの結果がtrueの場所だけを地面とする
        if (IsSolid(mapData_[ty][tx])) {
            return static_cast<float>(ty * kTileSize);
        }
    }
    return groundY_;
}

bool Stage::IsWall(float x, float y) const
{
    int tx = static_cast<int>(x) / kTileSize;
    int ty = static_cast<int>(y) / kTileSize;
    if (tx < 0 || tx >= kMapWidth || ty < 0 || ty >= kMapHeight) return false;
    return IsSolid(mapData_[ty][tx]);
}

bool Stage::IsBlock(float x, float y) const
{
    return IsWall(x, y);
}

void Stage::LoadMapCSV(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) return;

    std::string line;
    int y = 0;
    while (std::getline(file, line) && y < kMapHeight) {
        if (!line.empty() && line.back() == '\r') line.pop_back(); // 改行除去
        std::stringstream ss(line);
        std::string cell;
        int x = 0;
        while (std::getline(ss, cell, ',') && x < kMapWidth) {
            if (!cell.empty()) {
                try { mapData_[y][x] = std::stoi(cell); }
                catch (...) { mapData_[y][x] = 0; }
            }
            x++;
        }
        y++;
    }
}

float Stage::GetGroundY() const { return groundY_; }
float Stage::GetGoalX() const { return goalX_; }
float Stage::GetStageWidth() const { return stageWidth_; }