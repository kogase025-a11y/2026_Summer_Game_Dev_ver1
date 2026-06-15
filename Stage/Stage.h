#pragma once

class Stage
{
public:
    Stage();
    ~Stage() = default;

    void Update();
    void Draw(float cameraX, int screenWidth, int screenHeight) const;
    // ★【追加】ステージ番号を受け取って地形を初期化する関数
    void Init(int stageNum);


    float GetGroundY() const;
    float GetStepStartX() const;
    float GetStepEndX() const;
    float GetStepTopY() const;

    float GetStep2StartX() const;
    float GetStep2EndX() const;
    float GetStep2TopY() const;

    float GetStep3StartX() const;
    float GetStep3EndX() const;
    float GetStep3TopY() const;

	float GetSlopeStartX() const;
	float GetSlopeEndX() const;
	float GetSlopeStartY() const;

    float GetGroundYAtX(float x) const;

    float GetGoalX() const;
    float GetStageWidth() const;

    float GetPuddleStartX() const;
    float GetPuddleEndX() const;

    float GetDiatyStartX() const;
	float GetDiatyEndX() const;

	bool IsWall(float x, float y) const;

	bool IsBlock(float x, float y) const;
    
private:

    // ★【追加】1マスのサイズ（64ピクセル）
    static const int kTileSize = 64;

    // ★【追加】マップの大きさ（縦17マス × 横30マス ＝ ちょうど画面に収まるくらい）
    static const int kMapHeight = 17;
    static const int kMapWidth = 30;

    // ★【追加】マップデータを保持する2次元配列（最初は全部0＝空気にしておく）
    int mapData_[kMapHeight][kMapWidth] = { 0 };

    float groundY_;

    float stepStartX_;
    float stepEndX_;
    float stepTopY_;

    float step2StartX_;
    float step2EndX_;
    float step2TopY_;

    float step3StartX_;
    float step3EndX_;
    float step3TopY_;

	float slopeStartX_;
	float slopeEndX_;
     
    float goalX_;
    float stageWidth_;

    float puddleStartX_;
    float puddleEndX_;

	float diatyStartX_;
	float diatyEndX_;
   
};
