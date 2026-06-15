#pragma once

#include <memory>
#include <vector>
#include "../Resource/ImageFile.h"

class FileManager;

/**
 * @brief ステージの地形・描画・判定を管理するクラス
 */
class Stage {
public:
    Stage();
    ~Stage() = default;

    // --- メイン工程 ---
    void Init(int stageNum, FileManager& fileMng); // ステージ番号による初期化
    void Update();
    void Draw(float cameraX, int screenWidth, int screenHeight) const;

    // --- 判定用アクセッサ ---
    float GetGroundYAtX(float x) const; // 指定X座標における地面の高さを取得
    bool  IsWall(float x, float y) const;
    bool  IsBlock(float x, float y) const;

    // --- 基本パラメータ取得 ---
    float GetGroundY() const;
    float GetStageWidth() const;
    float GetGoalX() const;

    // 各地形の範囲取得
    float GetStepStartX() const; float GetStepEndX() const; float GetStepTopY() const;
    float GetStep2StartX() const; float GetStep2EndX() const; float GetStep2TopY() const;
    float GetStep3StartX() const; float GetStep3EndX() const; float GetStep3TopY() const;
    float GetSlopeStartX() const; float GetSlopeEndX() const; float GetSlopeStartY() const;
    float GetPuddleStartX() const; float GetPuddleEndX() const;
    float GetDiatyStartX() const; float GetDiatyEndX() const;

private:
    // --- 定数 ---
    static const int kTileSize = 64;   // 1マスのサイズ
    static const int kMapHeight = 17;  // マップの縦マス数
    static const int kMapWidth = 30;   // マップの横マス数

    // --- メンバ変数 ---
    int mapData_[kMapHeight][kMapWidth] = { 0 }; // タイルマップデータ

    float groundY_;      // 基本の地面の高さ
    float stageWidth_;   // ステージ全体の横幅
    float goalX_;        // ゴール地点のX座標

    // 地形オブジェクトの座標データ
    float stepStartX_, stepEndX_, stepTopY_;  // 段差1
    float step2StartX_, step2EndX_, step2TopY_; // 段差2
    float step3StartX_, step3EndX_, step3TopY_; // 段差3
    float slopeStartX_, slopeEndX_;             // 坂道
    float puddleStartX_, puddleEndX_;           // 水たまり
    float diatyStartX_, diatyEndX_;            // 汚れ地面

    // リソース
    std::shared_ptr<ImageFile> goalTex_;
};