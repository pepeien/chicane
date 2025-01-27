#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Character : public Chicane::ACameraPawn
{
public:
    Character();

protected:
    void onControlAttachment() override;

private:
    Chicane::CAudio* m_hitAudio;
    Chicane::CAudio* m_victoryAudio;
    Chicane::CMesh*  m_wand;
};