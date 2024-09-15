#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Character : public Chicane::CameraActor
{
public:
    Character();

protected:
    void onControlAttachment() override;
};