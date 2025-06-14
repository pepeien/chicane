#pragma once

#include "Essential.hpp"

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