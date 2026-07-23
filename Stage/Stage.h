#pragma once

// Stage.h
#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../Resource/ImageFile.h"

class FileManager;

class Stage {
public:
    Stage();
    void Init(int stageNum, FileManager& fileMng);
    void Update();
    void Draw(float cameraX, int screenWidth, int screenHeight) const;

    float GetGroundYAtX(float x) const;
    bool  IsWall(float x, float y) const;
	bool  IsBlock(float x, float y) const;
    float GetGroundY() const;
    float GetStageWidth() const;
    float GetGoalX() const;

private:
    void LoadMapCSV(const std::string& filePath);
    // ★追加：IDが solid(壁) かどうかを判定する補助関数
    bool IsSolid(int tileID) const;

    static const int kTileSize = 32;
    static const int kMapHeight = 34;
    static const int kMapWidth = 250; // 横に長いCSVに対応するため拡張

    int mapData_[kMapHeight][kMapWidth] = { 0 };

    // ★追加：マップチップ画像をバラバラにして保存する配列
    // CSVのIDが2000近くあるので、余裕を持って設定
    static const int kMaxChips = 4000; // 90枚×40行分くらいを確保
    int chipHandles_[kMaxChips];

    float groundY_;
    float stageWidth_;
    float goalX_;
    std::shared_ptr<ImageFile> goalTex_;
};