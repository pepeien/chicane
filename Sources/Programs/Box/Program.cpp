#include "Program.hpp"

#include <Chicane/Box/Asset/Header.hpp>
#include <Chicane/Box/Font.hpp>

Program::Program()
    : Chicane::Program("Box Utils")
{
    addOption({.bIsRequired = true, .name = ID_OPTION_NAME, .description = ID_OPTION_DESCRIPTION});
    addOption(
        {.bIsRequired = true,
         .name        = TYPE_OPTION_NAME,
         .description = TYPE_OPTION_DESCRIPTION,
         .validValues = Chicane::Box::AssetHeader::getTypeTags()}
    );
    addOption({.bIsRequired = false, .name = OUTPUT_OPTION_NAME, .description = OUTPUT_OPTION_DESCRIPTION});
}

void Program::onExec(const Chicane::ProgramParam& inParam)
{
    const Chicane::String&                    id = inParam.getOption(ID_OPTION_NAME)->getValue();
    const Chicane::FileSystem::Path&          output(inParam.getOption(OUTPUT_OPTION_NAME)->getValue().toStandard());
    const Chicane::ProgramParam::Positionals& sources = inParam.getPositionals();

    switch (Chicane::Box::AssetHeader::getTypeFromTag(inParam.getOption(TYPE_OPTION_NAME)->getValue()))
    {
    case Chicane::Box::AssetType::Font:
        createFont(id, sources, output);

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

void Program::createFont(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    if (inSources.empty())
    {
        throw std::runtime_error("The font source file is missing");
    }

    const Chicane::FileSystem::Path source = inSources.at(0).toStandard();

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The font source file is invalid");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.empty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Font));

        output = location.toStandard();
    }

    Chicane::Box::Font asset(output);
    asset.setId(inId);
    asset.setData(source);
    asset.saveXML();
}