#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Character : public Chicane::ACameraPawn
{
public:
    Character();

protected:
    void onCollision(const Chicane::Actor* inSubject) override;
    void onControlAttachment() override;

private:
    Chicane::CAudio*     m_hitAudio;
    Chicane::CAudio*     m_victoryAudio;
    Chicane::CCollision* m_collision;
    Chicane::CMesh*      m_wand;
};