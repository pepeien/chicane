#pragma once

#include "Chicane.hpp"
#include "Chicane/Runtime/Game.hpp"

class Level : public Chicane::Level
{
public:
    Level();

public:
    void onActivation() override;

private:
    void spawnSky();
    void spawnLights();
    void spawnStructures();
    void spawnApples();
};