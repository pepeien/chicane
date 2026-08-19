#pragma once

#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Actor/Camera.hpp>

class Scene : public Chicane::Scene
{
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
    void spawnSky();
    void spawnLights();
    void spawnCameras();
    void spawnStructures();
    void spawnCharacter();
    void spawnApples();

private:
    Chicane::ACamera* m_leftCamera;
    Chicane::ACamera* m_centerCamera;
    Chicane::ACamera* m_rightCamera;
};