#include "Program.hpp"

#include <Chicane/Box/Asset/Header.hpp>

Program::Program()
    : Chicane::Program("Box Utils")
{
    addOption({.bIsRequired = true, .name = ID_OPTION_NAME, .description = ID_OPTION_DESCRIPTION});
    addOption(
        {.bIsRequired = true,
         .name        = ASSET_OPTION_NAME,
         .description = ASSET_OPTION_DESCRIPTION,
         .validValues = Chicane::Box::AssetHeader::getTypeTags()}
    );
    addOption({.bIsRequired = false, .name = OUTPUT_OPTION_NAME, .description = OUTPUT_OPTION_DESCRIPTION});
}

void Program::onExec(const Chicane::ProgramParam& inParam)
{
    switch (Chicane::Box::AssetHeader::getTypeFromTag(inParam.getOption("type")->getValue()))
    {
    case Chicane::Box::AssetType::Font:
        break;

    case Chicane::Box::AssetType::Mesh:
        break;

    case Chicane::Box::AssetType::Model:
        break;

    case Chicane::Box::AssetType::Sky:
        break;

    case Chicane::Box::AssetType::Sound:
        break;

    case Chicane::Box::AssetType::Texture:
        break;

    default:
        break;
    }
}