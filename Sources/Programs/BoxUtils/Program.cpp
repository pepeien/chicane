#include "Program.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

#include <Chicane/Box.hpp>
#include <Chicane/Box/Asset/Header.hpp>
#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Core/Image.hpp>
#include <Chicane/Core/Math/Vertex.hpp>
#include <Chicane/Box/Animation.hpp>
#include <Chicane/Box/Animation/Loop.hpp>
#include <Chicane/Box/Animation/Track.hpp>
#include <Chicane/Box/Font.hpp>
#include <Chicane/Box/Gltf.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Box/Model.hpp>
#include <Chicane/Box/Model/Vendor.hpp>
#include <Chicane/Box/Skeleton.hpp>
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
    const Chicane::ProgramOption* bakeOption = inParam.getOption(BAKE_OPTION_NAME);
    const bool bBake = inParam.hasFlag(BAKE_OPTION_NAME) || (bakeOption && !bakeOption->getValue().isEmpty());

    if (bBake)
    {
        Chicane::FileSystem::Path bakePath =
            bakeOption ? Chicane::FileSystem::Path(bakeOption->getValue()) : Chicane::FileSystem::Path();

        if (bakePath.isEmpty())
        {
            const Chicane::ProgramParam::Positionals& positionals = inParam.getPositionals();
            if (!positionals.empty())
            {
                bakePath = positionals.front();
            }
        }

        bakePreviews(bakePath);

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
    case Chicane::Box::AssetType::Animation:
        createAnimation(id, sources, output);

        break;

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

    case Chicane::Box::AssetType::Skeleton:
        createSkeleton(id, sources, output);

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
        {Chicane::Box::AssetType::Animation, {}},
        {Chicane::Box::AssetType::Model,     {}},
        {Chicane::Box::AssetType::Skeleton,  {}},
        {Chicane::Box::AssetType::Texture,   {}}
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

    Chicane::Box::Texture    texture(textures.at(0));
    Chicane::Image::Instance textureImage = {};
    if (Chicane::Image::Instance data = texture.getData().lock())
    {
        textureImage = data;

        if (Chicane::Box::AssetPreview::write(texture.getXML(), Chicane::Box::AssetType::Texture, *data))
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

    const std::vector<Chicane::FileSystem::Path>& skeletons = sources.at(Chicane::Box::AssetType::Skeleton);
    if (!skeletons.empty())
    {
        asset.setSkeleton(skeletons.at(0));
    }

    for (const Chicane::FileSystem::Path& animation : sources.at(Chicane::Box::AssetType::Animation))
    {
        asset.appendAnimation(animation);
    }

    std::vector<Chicane::Box::PreviewGeometryBatch> batches = {};
    for (const auto& [reference, data] : modelGroups)
    {
        Chicane::Box::PreviewGeometryBatch batch;
        batch.texture = textureImage;

        const Chicane::Vertex::Index base = 0;
        batch.vertices                    = data.vertices;

        if (data.indices.empty())
        {
            for (Chicane::Vertex::Index i = 0; i < static_cast<Chicane::Vertex::Index>(data.vertices.size()); i++)
            {
                batch.indices.push_back(base + i);
            }
        }
        else
        {
            batch.indices = data.indices;
        }

        batches.push_back(std::move(batch));
    }

    if (std::unique_ptr<Chicane::Box::AssetPreview> preview =
            Chicane::Box::AssetPreview::createFromGeometry(output, batches))
    {
        if (preview->image)
        {
            Chicane::Box::AssetPreview::write(asset.getXML(), Chicane::Box::AssetType::Mesh, *preview->image);
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

    Chicane::Vertex::List            vertices = {};
    Chicane::Vertex::Indices         indices  = {};
    const Chicane::Box::ModelParsed& parsed   = modelGroups.begin()->second;
    vertices                                  = parsed.vertices;
    indices                                   = parsed.indices;

    std::vector<Chicane::Image::Instance> faces = {};
    for (const Chicane::FileSystem::Path& path : textures)
    {
        Chicane::Box::Texture texture(path);
        if (Chicane::Image::Instance data = texture.getData().lock())
        {
            faces.push_back(data);
            if (Chicane::Box::AssetPreview::write(texture.getXML(), Chicane::Box::AssetType::Texture, *data))
            {
                texture.saveXML();
            }
        }
    }

    if (std::unique_ptr<Chicane::Box::AssetPreview> preview =
            Chicane::Box::AssetPreview::createFromSky(output, vertices, indices, faces))
    {
        if (preview->image)
        {
            Chicane::Box::AssetPreview::write(asset.getXML(), Chicane::Box::AssetType::Sky, *preview->image);
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

void Program::createSkeleton(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Skeleton));

        output = location;
    }

    Chicane::Box::SkeletonBone::List bones;

    std::optional<Chicane::FileSystem::Path> gltf;
    for (const Chicane::String& source : inSources)
    {
        const Chicane::FileSystem::Path path(source);
        if (Chicane::Box::Model::parseVendor(path.extension().toString()) == Chicane::Box::ModelVendor::Gltf)
        {
            gltf = path;

            break;
        }
    }

    if (gltf)
    {
        bones = Chicane::Box::SkeletonGltf::parse(*gltf);
    }
    else
    {
        for (const Chicane::String& source : inSources)
        {
            Chicane::Box::SkeletonBone bone;
            bone.id = source;
            bones.push_back(bone);
        }
    }

    Chicane::Box::Skeleton asset(output);
    asset.setId(inId);
    asset.setBones(bones);
    asset.saveXML();
}

void Program::createAnimation(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    if (inSources.empty())
    {
        throw std::runtime_error("The animation skeleton file is missing");
    }

    std::optional<Chicane::FileSystem::Path> gltf;
    std::optional<Chicane::FileSystem::Path> skeleton;
    for (const Chicane::String& source : inSources)
    {
        const Chicane::FileSystem::Path path(source);
        if (Chicane::Box::Model::parseVendor(path.extension().toString()) == Chicane::Box::ModelVendor::Gltf)
        {
            gltf = path;

            continue;
        }

        if (Chicane::Box::AssetHeader::getTypeFromExtension(path) == Chicane::Box::AssetType::Skeleton)
        {
            skeleton = path;
        }
    }

    if (!skeleton)
    {
        throw std::runtime_error(
            gltf ? "The animation skeleton file is missing"
                 : "The animation source must be a skeleton or a glTF file"
        );
    }

    if (!Chicane::FileSystem::exists(*skeleton))
    {
        throw std::runtime_error("The animation skeleton file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Animation));

        output = location;
    }

    Chicane::Box::AnimationClip clip(inId);
    clip.loop = Chicane::Box::AnimationLoop::Once;

    if (gltf)
    {
        clip = Chicane::Box::AnimationGltf::parse(*gltf, inId);

        Chicane::Box::Skeleton           bones(*skeleton);
        Chicane::Box::AnimationTrack::List tracks;
        for (const Chicane::Box::AnimationTrack& track : clip.tracks)
        {
            if (track.name.isEmpty() || !bones.hasBone(track.name))
            {
                continue;
            }

            tracks.push_back(track);
        }

        if (tracks.empty())
        {
            throw std::runtime_error(
                "Animation [" + inId.toStandard() + "] has no tracks on skeleton [" +
                bones.getId().toStandard() + "]"
            );
        }

        clip.tracks = std::move(tracks);
    }

    Chicane::Box::Animation asset(output);
    asset.setId(inId);
    asset.setSkeleton(*skeleton);
    asset.setClip(clip);
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
            std::cout << "Generated a preview for [" << inPath.toString() << "]" << std::endl;

            return;
        }

        std::cerr << "Failed to generated a preview for[" << inPath.toString() << "]" << std::endl;
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