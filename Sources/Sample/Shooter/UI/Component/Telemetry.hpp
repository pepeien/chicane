#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>
#include <Chicane/Runtime/Application/Telemetry.hpp>

CH_TYPE(Manual)
class Telemetry : public Chicane::Grid::Container
{
public:
    CH_CONSTRUCTOR()
    Telemetry(const pugi::xml_node& inNode);

public:
    CH_FIELD()
    const Chicane::ApplicationTelemetry* telemetry;
};
