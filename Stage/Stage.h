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

    float GetGroundYAtX(float x) const;

    float GetGoalX() const;
    float GetStageWidth() const;

    float GetPuddleStartX() const;
    float GetPuddleEndX() const;

private:
    float groundY_;
    float stepStartX_;
    float stepEndX_;
    float stepTopY_;

    float goalX_;
    float stageWidth_;

    float puddleStartX_;
    float puddleEndX_;
};
