#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Level : public Chicane::Level
{
public:
    Level();

public:
    void onActivation() override;

private:
    void spawnStructures();
    void spawnApples();
};