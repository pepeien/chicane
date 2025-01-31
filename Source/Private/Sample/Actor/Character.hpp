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
    void onLook(const SDL_MouseMotionEvent& inEvent);

    void onLeftClick(bool bInIsButtonPressed);

    void onMoveForward(bool bInIsButtonPressed);
    void onMoveBackward(bool bInIsButtonPressed);
    void onMoveLeft(bool bInIsButtonPressed);
    void onMoveRight(bool bInIsButtonPressed);

    void onJump(bool bInIsButtonPressed);

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