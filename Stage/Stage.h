#pragma once

class Stage
{
public:
    Stage();
    ~Stage() = default;

    void Update();
    void Draw(float cameraX, int screenWidth, int screenHeight) const;

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

    
private:
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
