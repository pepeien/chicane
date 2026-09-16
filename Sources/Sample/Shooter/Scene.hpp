#pragma once

#include <Chicane/Runtime/Scene.hpp>

class Scene : public Chicane::Scene
{
public:
    static constexpr inline const char* DEFAULT_TRACK    = "Assets/Sample/Shooter/Levels/Default.track";
    static constexpr inline const char* LEFT_CAMERA_ID   = "Left";
    static constexpr inline const char* CENTER_CAMERA_ID = "Center";
    static constexpr inline const char* RIGHT_CAMERA_ID  = "Right";

public:
    Scene();

public:
    void onLoad() override;

public:
    void activateLeftCamera();
    void activateCenterCamera();
    void activateRightCamera();
    void disableCameras();

private:
    void spawnApples();
};