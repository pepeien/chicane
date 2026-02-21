#include "Program.hpp"

#include <unordered_map>

#include <Chicane/Box/Asset/Header.hpp>
#include <Chicane/Box/Font.hpp>
#include <Chicane/Box/Model.hpp>
#include <Chicane/Box/Texture.hpp>
#include <Chicane/Box/Sky.hpp>
#include <Chicane/Box/Sound.hpp>

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
        createMesh(id, sources, output);

        break;

    case Chicane::Box::AssetType::Model:
        createModel(id, sources, output);

        break;

    case Chicane::Box::AssetType::Sky:
        createSky(id, sources, output);

        break;

    case Chicane::Box::AssetType::Sound:
        createSound(id, sources, output);

        break;

    case Chicane::Box::AssetType::Texture:
        createTexture(id, sources, output);

        break;

    default:
        showHelp();

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
        throw std::runtime_error("The font source file doesn't exist");
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

void Program::createMesh(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{}

void Program::createModel(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    if (inSources.empty())
    {
        throw std::runtime_error("The model source file is missing");
    }

    const Chicane::FileSystem::Path source = inSources.at(0).toStandard();

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The model source file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.empty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Model));

        output = location.toStandard();
    }

    Chicane::Box::Model asset(output);
    asset.setId(inId);
    asset.setData(source);
    asset.saveXML();
}

void Program::createSky(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    std::unordered_map<Chicane::Box::AssetType, std::vector<Chicane::FileSystem::Path>> sources = {
        {Chicane::Box::AssetType::Model,   {}},
        {Chicane::Box::AssetType::Texture, {}}
    };

    for (const Chicane::String& source : inSources)
    {
        const Chicane::FileSystem::Path path(source.toStandard());
        const Chicane::Box::AssetType   type = Chicane::Box::AssetHeader::getTypeFromExtension(path);

        if (sources.find(type) == sources.end())
        {
            continue;
        }

        if (!Chicane::FileSystem::exists(path))
        {
            throw std::runtime_error(
                "The Sky [" + Chicane::Box::AssetHeader::getTypeTag(type) + "] reference file doesn't exist"
            );
        }

        sources.at(type).push_back(path);
    }

    const std::vector<Chicane::FileSystem::Path>& models = sources.at(Chicane::Box::AssetType::Model);
    if (models.empty())
    {
        throw std::runtime_error(
            "The Sky [" + Chicane::Box::AssetHeader::getTypeTag(Chicane::Box::AssetType::Model) +
            "] reference file is missing"
        );
    }

    const std::vector<Chicane::FileSystem::Path>& textures = sources.at(Chicane::Box::AssetType::Texture);
    if (textures.empty())
    {
        throw std::runtime_error(
            "The Sky [" + Chicane::Box::AssetHeader::getTypeTag(Chicane::Box::AssetType::Texture) +
            "] reference files are missing"
        );
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.empty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Sky));

        output = location.toStandard();
    }

    Chicane::Box::Sky asset(output);
    asset.setId(inId);
    asset.setModel(models.at(0));
    asset.addTexture(textures);
    asset.saveXML();
}

void Program::createTexture(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    if (inSources.empty())
    {
        throw std::runtime_error("The texture source file is missing");
    }

    const Chicane::FileSystem::Path source = inSources.at(0).toStandard();

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The texture source file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.empty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Texture));

        output = location.toStandard();
    }

    Chicane::Box::Texture asset(output);
    asset.setId(inId);
    asset.setData(source);
    asset.saveXML();
}

void Program::createSound(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    if (inSources.empty())
    {
        throw std::runtime_error("The sound source file is missing");
    }

    const Chicane::FileSystem::Path source = inSources.at(0).toStandard();

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The sound source file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.empty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Sound));

        output = location.toStandard();
    }

    Chicane::Box::Sound asset(output);
    asset.setId(inId);
    asset.setData(source);
    asset.saveXML();
}