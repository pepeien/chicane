#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Character : public Chicane::CameraPawn
{
public:
    Character();

protected:
    void onControlAttachment() override;

private:
    Chicane::MeshComponent* m_wand;
};