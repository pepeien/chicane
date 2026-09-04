#include "Program.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <Chicane/Box.hpp>
#include <Chicane/Box/Asset/Header.hpp>
#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Core/Image.hpp>
#include <Chicane/Core/Math/Vertex.hpp>
#include <Chicane/Box/Font.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Box/Model.hpp>
#include <Chicane/Box/Texture.hpp>
#include <Chicane/Box/Sky.hpp>
#include <Chicane/Box/Sound.hpp>

Program::Program()
    : Chicane::Program("Box Utils")
{
    Chicane::ProgramOptionSetting idOption;
    idOption.bIsRequired = false;
    idOption.name        = ID_OPTION_NAME;
    idOption.description = ID_OPTION_DESCRIPTION;
    addOption(idOption);

    Chicane::ProgramOptionSetting typeOption;
    typeOption.bIsRequired = false;
    typeOption.name        = TYPE_OPTION_NAME;
    typeOption.description = TYPE_OPTION_DESCRIPTION;
    typeOption.validValues = Chicane::Box::AssetHeader::getTypeTags();
    addOption(typeOption);

    Chicane::ProgramOptionSetting outputOption;
    outputOption.bIsRequired = false;
    outputOption.name        = OUTPUT_OPTION_NAME;
    outputOption.description = OUTPUT_OPTION_DESCRIPTION;
    addOption(outputOption);

    Chicane::ProgramOptionSetting bakeOption;
    bakeOption.bIsRequired = false;
    bakeOption.name        = BAKE_OPTION_NAME;
    bakeOption.description = BAKE_OPTION_DESCRIPTION;
    addOption(bakeOption);
}

void Program::onExec(const Chicane::ProgramParam& inParam)
{
    const Chicane::String& bake = inParam.getOption(BAKE_OPTION_NAME)->getValue();
    if (!bake.isEmpty())
    {
        bakePreviews(bake);

        return;
    }

    const Chicane::ProgramOption* idOption   = inParam.getOption(ID_OPTION_NAME);
    const Chicane::ProgramOption* typeOption = inParam.getOption(TYPE_OPTION_NAME);
    if (!idOption || idOption->getValue().isEmpty() || !typeOption || typeOption->getValue().isEmpty())
    {
        showHelp();

        return;
    }

    const Chicane::String&                    id = idOption->getValue();
    const Chicane::FileSystem::Path&          output(inParam.getOption(OUTPUT_OPTION_NAME)->getValue());
    const Chicane::ProgramParam::Positionals& sources = inParam.getPositionals();

    switch (Chicane::Box::AssetHeader::getTypeFromTag(typeOption->getValue()))
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

    const Chicane::FileSystem::Path source = inSources.at(0);

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The font source file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Font));

        output = location;
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
{
    std::unordered_map<Chicane::Box::AssetType, std::vector<Chicane::FileSystem::Path>> sources = {
        {Chicane::Box::AssetType::Model,   {}},
        {Chicane::Box::AssetType::Texture, {}}
    };

    for (const Chicane::String& source : inSources)
    {
        const Chicane::FileSystem::Path path(source);
        const Chicane::Box::AssetType   type = Chicane::Box::AssetHeader::getTypeFromExtension(path);

        if (sources.find(type) == sources.end())
        {
            continue;
        }

        if (!Chicane::FileSystem::exists(path))
        {
            throw std::runtime_error(
                "The Mesh [" + Chicane::Box::AssetHeader::getTypeTag(type) + "] reference file doesn't exist"
            );
        }

        sources.at(type).push_back(path);
    }

    const std::vector<Chicane::FileSystem::Path>& models = sources.at(Chicane::Box::AssetType::Model);
    if (models.empty())
    {
        throw std::runtime_error(
            "The Mesh [" + Chicane::Box::AssetHeader::getTypeTag(Chicane::Box::AssetType::Model) +
            "] reference file is missing"
        );
    }

    const std::vector<Chicane::FileSystem::Path>& textures = sources.at(Chicane::Box::AssetType::Texture);
    if (textures.empty())
    {
        throw std::runtime_error(
            "The Mesh [" + Chicane::Box::AssetHeader::getTypeTag(Chicane::Box::AssetType::Texture) +
            "] reference file is missing"
        );
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Mesh));

        output = location;
    }

    Chicane::Box::Mesh asset(output);
    asset.setId(inId);

    Chicane::Box::Model            model(models.at(0));
    Chicane::Box::ModelParsed::Map modelGroups = model.getData();

    if (modelGroups.empty())
    {
        throw std::runtime_error(
            "The Mesh [" + Chicane::Box::AssetHeader::getTypeTag(Chicane::Box::AssetType::Model) +
            "] groups are missing"
        );
    }

    Chicane::Box::Texture texture(textures.at(0));
    if (Chicane::Image::Instance data = texture.getData().lock())
    {
        if (Chicane::Box::AssetPreview::write(
                texture.getXML(),
                texture.getId(),
                Chicane::Box::AssetType::Texture,
                *data
            ))
        {
            texture.saveXML();
        }
    }

    for (const auto& [reference, data] : modelGroups)
    {
        Chicane::Box::MeshGroup group;
        group.setId(reference);
        group.setModel(model.getFilepath(), reference);
        group.setTexture(texture.getFilepath(), texture.getId());

        asset.appendGroup(group);
    }

    Chicane::Vertex::List    vertices = {};
    Chicane::Vertex::Indices indices  = {};
    for (const auto& [reference, data] : modelGroups)
    {
        const Chicane::Vertex::Index base = static_cast<Chicane::Vertex::Index>(vertices.size());
        vertices.insert(vertices.end(), data.vertices.begin(), data.vertices.end());

        if (data.indices.empty())
        {
            for (Chicane::Vertex::Index i = 0; i < static_cast<Chicane::Vertex::Index>(data.vertices.size()); i++)
            {
                indices.push_back(base + i);
            }

            continue;
        }

        for (const Chicane::Vertex::Index index : data.indices)
        {
            indices.push_back(base + index);
        }
    }

    if (std::unique_ptr<Chicane::Box::AssetPreview> preview =
            Chicane::Box::AssetPreview::createFromGeometry(output, inId, vertices, indices))
    {
        if (preview->image)
        {
            Chicane::Box::AssetPreview::write(
                asset.getXML(),
                inId,
                Chicane::Box::AssetType::Mesh,
                *preview->image
            );
        }
    }

    asset.saveXML();
}

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

    const Chicane::FileSystem::Path source = inSources.at(0);

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The model source file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Model));

        output = location;
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
        const Chicane::FileSystem::Path path(source);
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

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Sky));

        output = location;
    }

    Chicane::Box::Sky asset(output);
    asset.setId(inId);

    Chicane::Box::Model            model(models.at(0));
    Chicane::Box::ModelParsed::Map modelGroups = model.getData();

    if (modelGroups.empty())
    {
        throw std::runtime_error(
            "The Sky [" + Chicane::Box::AssetHeader::getTypeTag(Chicane::Box::AssetType::Model) + "] groups are missing"
        );
    }

    asset.setModel(model.getFilepath(), modelGroups.begin()->first);
    asset.addTexture(textures);

    Chicane::Vertex::List    vertices = {};
    Chicane::Vertex::Indices indices  = {};
    const Chicane::Box::ModelParsed& parsed = modelGroups.begin()->second;
    vertices = parsed.vertices;
    indices  = parsed.indices;

    std::vector<Chicane::Image::Instance> faces = {};
    for (const Chicane::FileSystem::Path& path : textures)
    {
        Chicane::Box::Texture texture(path);
        if (Chicane::Image::Instance data = texture.getData().lock())
        {
            faces.push_back(data);
            if (Chicane::Box::AssetPreview::write(
                    texture.getXML(),
                    texture.getId(),
                    Chicane::Box::AssetType::Texture,
                    *data
                ))
            {
                texture.saveXML();
            }
        }
    }

    if (std::unique_ptr<Chicane::Box::AssetPreview> preview =
            Chicane::Box::AssetPreview::createFromSky(output, inId, vertices, indices, faces))
    {
        if (preview->image)
        {
            Chicane::Box::AssetPreview::write(
                asset.getXML(),
                inId,
                Chicane::Box::AssetType::Sky,
                *preview->image
            );
        }
    }

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

    const Chicane::FileSystem::Path source = inSources.at(0);

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The texture source file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Texture));

        output = location;
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

    const Chicane::FileSystem::Path source = inSources.at(0);

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The sound source file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Sound));

        output = location;
    }

    Chicane::Box::Sound asset(output);
    asset.setId(inId);
    asset.setData(source);
    asset.saveXML();
}

void Program::bakePreviews(const Chicane::FileSystem::Path& inRoot)
{
    const Chicane::FileSystem::Path root = inRoot.isEmpty() ? Chicane::FileSystem::Path("Assets") : inRoot;
    if (!Chicane::FileSystem::exists(root))
    {
        throw std::runtime_error("Bake path does not exist");
    }

    auto bakeOne = [](const Chicane::FileSystem::Path& inPath)
    {
        if (!Chicane::Box::AssetHeader::isFileAsset(inPath))
        {
            return;
        }

        if (Chicane::Box::embedPreview(inPath))
        {
            std::cout << "Preview " << inPath.toString() << std::endl;

            return;
        }

        std::cerr << "Failed " << inPath.toString() << std::endl;
    };

    if (root.isFile())
    {
        bakeOne(root);

        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root.toStandard()))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        bakeOne(Chicane::FileSystem::Path(entry.path()));
    }
}