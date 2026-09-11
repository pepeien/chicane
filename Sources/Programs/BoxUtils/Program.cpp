#include "Program.hpp"

#include <cctype>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

#include <Chicane/Box.hpp>
#include <Chicane/Box/Asset/Header.hpp>
#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Core/Image.hpp>
#include <Chicane/Core/Math/Mat/Mat4.hpp>
#include <Chicane/Core/Math/Vertex.hpp>
#include <Chicane/Core/Texture/Map.hpp>
#include <Chicane/Box/Animation.hpp>
#include <Chicane/Box/Animation/Gltf.hpp>
#include <Chicane/Box/Animation/Loop.hpp>
#include <Chicane/Box/Animation/Track.hpp>
#include <Chicane/Box/Font.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Box/Model.hpp>
#include <Chicane/Box/Model/Gltf.hpp>
#include <Chicane/Box/Model/Vendor.hpp>
#include <Chicane/Box/Skeleton.hpp>
#include <Chicane/Box/Skeleton/Gltf.hpp>
#include <Chicane/Box/Texture.hpp>
#include <Chicane/Box/Texture/Gltf.hpp>
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

    Chicane::ProgramOptionSetting exportOption;
    exportOption.bIsRequired = false;
    exportOption.name        = EXPORT_OPTION_NAME;
    exportOption.description = EXPORT_OPTION_DESCRIPTION;
    addOption(exportOption);
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

    const Chicane::ProgramOption* exportOption = inParam.getOption(EXPORT_OPTION_NAME);
    const bool bExport = inParam.hasFlag(EXPORT_OPTION_NAME) || inParam.hasFlag(EXPORT_OPTION_NAME[0]) ||
                         (exportOption && !exportOption->getValue().isEmpty());

    if (bExport)
    {
        Chicane::FileSystem::Path source =
            exportOption ? Chicane::FileSystem::Path(exportOption->getValue()) : Chicane::FileSystem::Path();

        const Chicane::ProgramParam::Positionals& positionals = inParam.getPositionals();
        if (source.isEmpty())
        {
            for (const Chicane::String& positional : positionals)
            {
                const Chicane::FileSystem::Path path(positional);
                if (Chicane::Box::Model::parseVendor(path.extension().toString()) != Chicane::Box::ModelVendor::Gltf)
                {
                    continue;
                }

                source = path;

                break;
            }
        }

        if (source.isEmpty() && !positionals.empty())
        {
            source = positionals.front();
        }

        const Chicane::ProgramOption* idOption = inParam.getOption(ID_OPTION_NAME);
        Chicane::String               id       = idOption ? idOption->getValue() : Chicane::String();
        if (id.isEmpty() && !source.isEmpty())
        {
            id = source.stem().toString();
        }

        const Chicane::FileSystem::Path output(
            inParam.getOption(OUTPUT_OPTION_NAME) ? inParam.getOption(OUTPUT_OPTION_NAME)->getValue()
                                                  : Chicane::String()
        );

        createFromGltf(id, source, output);

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
            gltf ? "The animation skeleton file is missing" : "The animation source must be a skeleton or a glTF file"
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

        Chicane::Box::Skeleton             bones(*skeleton);
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
                "Animation [" + inId.toStandard() + "] has no tracks on skeleton [" + bones.getId().toStandard() + "]"
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

static Chicane::String sanitizeName(const Chicane::String& inValue)
{
    std::string result;
    result.reserve(inValue.size());

    for (char character : inValue.toStandard())
    {
        if (std::isalnum(static_cast<unsigned char>(character)) || character == '_' || character == '-')
        {
            result.push_back(character);

            continue;
        }

        if (result.empty() || result.back() == '_')
        {
            continue;
        }

        result.push_back('_');
    }

    while (!result.empty() && result.back() == '_')
    {
        result.pop_back();
    }

    return result.empty() ? Chicane::String("Asset") : Chicane::String(result);
}

static void ensureParent(const Chicane::FileSystem::Path& inFile)
{
    const Chicane::FileSystem::Path parent = inFile.parent();
    if (parent.isEmpty() || parent.exists())
    {
        return;
    }

    std::filesystem::create_directories(parent.toStandard());
}

static Chicane::FileSystem::Path resolveOutputDirectory(const Chicane::FileSystem::Path& inOutput)
{
    if (inOutput.isEmpty())
    {
        return ".";
    }

    if (inOutput.hasExtension() &&
        Chicane::Box::AssetHeader::getTypeFromExtension(inOutput) != Chicane::Box::AssetType::Undefined)
    {
        const Chicane::FileSystem::Path parent = inOutput.parent();

        return parent.isEmpty() ? Chicane::FileSystem::Path(".") : parent;
    }

    return inOutput;
}

static void logGenerated(const Chicane::FileSystem::Path& inPath)
{
    std::cout << "Generated [" << inPath.toString() << "]" << std::endl;
}

struct WrittenTexture
{
    Chicane::FileSystem::Path path;
    Chicane::String           id;
    Chicane::Image::Instance  image;
};

void Program::createFromGltf(
    const Chicane::String& inId, const Chicane::FileSystem::Path& inSource, const Chicane::FileSystem::Path& inOutput
)
{
    if (inSource.isEmpty())
    {
        throw std::runtime_error("The glTF/GLB source file is missing");
    }

    if (!Chicane::FileSystem::exists(inSource))
    {
        throw std::runtime_error("The glTF/GLB source file doesn't exist");
    }

    if (Chicane::Box::Model::parseVendor(inSource.extension().toString()) != Chicane::Box::ModelVendor::Gltf)
    {
        throw std::runtime_error("The source file must be a glTF or GLB file");
    }

    const Chicane::String     id        = sanitizeName(inId.isEmpty() ? inSource.stem().toString() : inId);
    Chicane::FileSystem::Path directory = resolveOutputDirectory(inOutput);
    if (directory.isEmpty())
    {
        directory = ".";
    }

    std::filesystem::create_directories(directory.toStandard());

    const Chicane::FileSystem::Path modelPath =
        directory / (id + Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Model));
    createModel(id, {inSource.toString()}, modelPath);
    logGenerated(modelPath);

    Chicane::Box::Model            model(modelPath);
    Chicane::Box::ModelParsed::Map modelGroups = model.getData();
    if (modelGroups.empty())
    {
        throw std::runtime_error("The glTF/GLB file has no meshes");
    }

    const Chicane::Box::TextureGltf::Parsed textures = Chicane::Box::TextureGltf::parse(inSource);

    std::vector<WrittenTexture> written(textures.images.size());
    for (std::size_t i = 0; i < textures.images.size(); ++i)
    {
        const Chicane::Box::TextureGltf::Entry& image = textures.images[i];
        if (image.data.empty())
        {
            continue;
        }

        const Chicane::String name = sanitizeName(image.id);
        WrittenTexture        texture;
        if (i == 0)
        {
            texture.path =
                directory / (id + Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Texture));
            texture.id = id;
        }
        else
        {
            texture.path =
                directory / id / (name + Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Texture));
            texture.id = id + "/" + name;
        }

        ensureParent(texture.path);

        try
        {
            Chicane::Box::Texture asset(texture.path);
            asset.setId(texture.id);
            asset.setVendor(image.vendor);
            asset.setData(image.data);
            asset.saveXML();

            if (Chicane::Image::Instance data = asset.getData().lock())
            {
                texture.image = data;
            }
        }
        catch (const std::exception& exception)
        {
            std::cerr << "Failed to export texture [" << texture.path.toString() << "]: " << exception.what()
                      << std::endl;

            continue;
        }

        written[i] = texture;
        logGenerated(texture.path);
    }

    Chicane::FileSystem::Path fallbackPath = Chicane::Box::Texture::DEFAULT_SOURCE;
    Chicane::String           fallbackId   = Chicane::Box::Texture::DEFAULT_REFERENCE;
    Chicane::Image::Instance  fallbackImage;
    for (const WrittenTexture& texture : written)
    {
        if (texture.path.isEmpty())
        {
            continue;
        }

        fallbackPath  = texture.path;
        fallbackId    = texture.id;
        fallbackImage = texture.image;

        break;
    }

    if (fallbackPath == Chicane::Box::Texture::DEFAULT_SOURCE && !Chicane::FileSystem::exists(fallbackPath))
    {
        const unsigned char             pixel[4] = {255, 255, 255, 255};
        const Chicane::Image            image(pixel, 1, 1, 4, 4);
        const Chicane::FileSystem::Path path =
            directory / (id + Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Texture));

        ensureParent(path);

        Chicane::Box::Texture asset(path);
        asset.setId(id);
        asset.setVendor(Chicane::ImageVendor::Png);
        asset.setData(image.encode());
        asset.saveXML();

        fallbackPath = path;
        fallbackId   = id;
        if (Chicane::Image::Instance data = asset.getData().lock())
        {
            fallbackImage = data;
        }

        logGenerated(path);
    }

    const std::vector<Chicane::String> animationNames = Chicane::Box::AnimationGltf::list(inSource);

    Chicane::FileSystem::Path skeletonPath;
    if (Chicane::Box::SkeletonGltf::hasSkin(inSource) || !animationNames.empty())
    {
        skeletonPath =
            directory / (id + Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Skeleton));
        createSkeleton(id, {inSource.toString()}, skeletonPath);
        logGenerated(skeletonPath);
    }

    std::vector<Chicane::FileSystem::Path> animations;
    if (!skeletonPath.isEmpty())
    {
        Chicane::Box::Skeleton bones(skeletonPath);

        for (std::uint32_t i = 0; i < animationNames.size(); ++i)
        {
            Chicane::Box::AnimationClip clip;
            try
            {
                clip = Chicane::Box::AnimationGltf::parse(inSource, i);
            }
            catch (const std::exception& exception)
            {
                std::cerr << "Skipped animation [" << animationNames[i] << "]: " << exception.what() << std::endl;

                continue;
            }

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
                std::cerr << "Skipped animation [" << animationNames[i] << "]: no tracks match skeleton bones"
                          << std::endl;

                continue;
            }

            clip.tracks = std::move(tracks);

            const Chicane::String     animId = sanitizeName(animationNames[i]);
            Chicane::FileSystem::Path animPath =
                animationNames.size() == 1
                    ? directory / (id + Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Animation))
                    : directory / id /
                          (animId + Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Animation));

            ensureParent(animPath);

            Chicane::Box::Animation asset(animPath);
            asset.setId(animId);
            asset.setSkeleton(skeletonPath);
            asset.setClip(clip);
            asset.saveXML();

            animations.push_back(animPath);
            logGenerated(animPath);
        }
    }

    const Chicane::FileSystem::Path meshPath =
        directory / (id + Chicane::Box::AssetHeader::getTypeExtension(Chicane::Box::AssetType::Mesh));
    ensureParent(meshPath);

    Chicane::Box::Mesh asset(meshPath);
    asset.setId(id);
    asset.setGroups({});
    asset.setAnimations({});

    std::vector<Chicane::Box::PreviewGeometryBatch> batches    = {};
    std::size_t                                     groupCount = 0;
    for (const auto& [reference, data] : modelGroups)
    {
        Chicane::Box::MeshGroup group;
        group.setId(reference);
        group.setBone(data.bone);
        group.setModel(model.getFilepath(), reference);

        bool                     hasBase    = false;
        Chicane::Image::Instance groupImage = fallbackImage;
        if (data.material >= 0)
        {
            const auto found = textures.materials.find(data.material);
            if (found != textures.materials.end())
            {
                for (const auto& [map, imageIndex] : found->second)
                {
                    if (imageIndex < 0 || static_cast<std::size_t>(imageIndex) >= written.size())
                    {
                        continue;
                    }

                    const WrittenTexture& texture = written.at(static_cast<std::size_t>(imageIndex));
                    if (texture.path.isEmpty())
                    {
                        continue;
                    }

                    group.setTexture(map, texture.path.toString(), texture.id);
                    if (map == Chicane::TextureMap::Base)
                    {
                        hasBase    = true;
                        groupImage = texture.image;
                    }
                }
            }
        }

        if (!hasBase)
        {
            group.setTexture(fallbackPath.toString(), fallbackId);
        }

        if (!group.isValid())
        {
            std::cerr << "Skipping mesh group [" << reference << "]" << std::endl;

            continue;
        }

        asset.appendGroup(group);
        groupCount++;

        Chicane::Box::PreviewGeometryBatch batch;
        batch.texture  = groupImage;
        batch.vertices = data.vertices;

        if (data.indices.empty())
        {
            for (Chicane::Vertex::Index i = 0; i < static_cast<Chicane::Vertex::Index>(data.vertices.size()); i++)
            {
                batch.indices.push_back(i);
            }
        }
        else
        {
            batch.indices = data.indices;
        }

        batches.push_back(std::move(batch));
    }

    if (groupCount == 0)
    {
        throw std::runtime_error("Failed to generate a mesh from the glTF/GLB file");
    }

    if (!skeletonPath.isEmpty())
    {
        asset.setSkeleton(skeletonPath);
    }

    for (const Chicane::FileSystem::Path& animation : animations)
    {
        asset.appendAnimation(animation);
    }

    if (std::unique_ptr<Chicane::Box::AssetPreview> preview =
            Chicane::Box::AssetPreview::createFromGeometry(meshPath, batches))
    {
        if (preview->image)
        {
            Chicane::Box::AssetPreview::write(asset.getXML(), Chicane::Box::AssetType::Mesh, *preview->image);
        }
    }

    asset.saveXML();
    logGenerated(meshPath);
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