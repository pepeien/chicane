#pragma once

#include "Chicane/Runtime/Scene/Actor/Camera.hpp"
#include "Essential.hpp"

class Level : public Chicane::Scene
{
public:
    Level();

public:
    void onActivation() override;

public:
    void activateLeftCamera();
    void activateCenterCamera();
    void activateRightCamera();
    void disableCameras();

private:
    void spawnSky();
    void spawnLights();
    void spawnCameras();
    void spawnStructures();
    void spawnApples();

private:
    Chicane::ACamera* m_leftCamera;
    Chicane::ACamera* m_centerCamera;
    Chicane::ACamera* m_rightCamera;
};