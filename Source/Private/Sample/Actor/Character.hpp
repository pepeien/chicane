#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Character : public Chicane::ACharacter
{
public:
    Character();

protected:
    void onControlAttachment() override;

private:
    void onLook(const Chicane::Controller::MouseMotionEvent& inEvent);

    void onLeftClick();

    void onMoveForward();
    void onMoveBackward();
    void onMoveLeft();
    void onMoveRight();

private:
    // Camera
    Chicane::CCamera* m_camera;

    // Mesh
    Chicane::CMesh*   m_wand;
    Chicane::CMesh*   m_body;

    // Audio
    Chicane::CAudio*  m_hitAudio;
    Chicane::CAudio*  m_victoryAudio;
};