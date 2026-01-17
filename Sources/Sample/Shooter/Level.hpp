#pragma once

#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor/Camera.hpp"

class Level : public Chicane::Scene
{
public:
    using CameraObservable   = Chicane::EventObservable<Chicane::ACamera*>;
    using CameraSubscription = Chicane::EventSubscription<Chicane::ACamera*>;

public:
    Level();

public:
    void onActivation() override;

public:
    void activateLeftCamera();
    void activateCenterCamera();
    void activateRightCamera();
    void disableCameras();
    CameraSubscription watchActiveCamera(
        CameraSubscription::NextCallback     inNext,
        CameraSubscription::ErrorCallback    inError    = nullptr,
        CameraSubscription::CompleteCallback inComplete = nullptr
    );

private:
    void spawnSky();
    void spawnLights();
    void spawnCameras();
    void spawnStructures();
    void spawnApples();

private:
    Chicane::ACamera*                           m_leftCamera;
    Chicane::ACamera*                           m_centerCamera;
    Chicane::ACamera*                           m_rightCamera;
    Chicane::EventObservable<Chicane::ACamera*> m_cameraObservable;
};