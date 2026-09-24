#include "Program.hpp"
#include "Preview.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <Chicane/Box.hpp>
#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Core/Image.hpp>
#include <Chicane/Core/Math/Mat/Mat4.hpp>
#include <Chicane/Core/Math/Vertex.hpp>
#include <Chicane/Box/Animation.hpp>
#include <Chicane/Box/Animation/Gltf.hpp>
#include <Chicane/Box/Animation/Loop.hpp>
#include <Chicane/Box/Animation/Track.hpp>
#include <Chicane/Box/Effect.hpp>
#include <Chicane/Box/Font.hpp>
#include <Chicane/Box/Import/Source.hpp>
#include <Chicane/Box/Material.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Box/Model.hpp>
#include <Chicane/Box/Model/Gltf.hpp>
#include <Chicane/Box/Model/Vendor.hpp>
#include <Chicane/Box/Skeleton.hpp>
#include <Chicane/Box/Skeleton/Gltf.hpp>
#include <Chicane/Box/Texture.hpp>
#include <Chicane/Box/Texture/Gltf.hpp>
#include <Chicane/Box/Sky.hpp>
#include <Chicane/Box/Sky/Kind.hpp>
#include <Chicane/Box/Sky/Side.hpp>
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
    typeOption.validValues = Chicane::Box::getTypeTags();
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

    switch (Chicane::Box::getTypeFromTag(typeOption->getValue()))
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

    case Chicane::Box::AssetType::Material:
        createMaterial(id, sources, output);

        break;

    case Chicane::Box::AssetType::Effect:
        createEffect(id, sources, output);

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
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Font));

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
        const Chicane::Box::AssetType   type = Chicane::Box::getTypeFromExtension(path);

        if (sources.find(type) == sources.end())
        {
            continue;
        }

        if (!Chicane::FileSystem::exists(path))
        {
            throw std::runtime_error("The Mesh [" + Chicane::Box::getTypeTag(type) + "] reference file doesn't exist");
        }

        sources.at(type).push_back(path);
    }

    const std::vector<Chicane::FileSystem::Path>& models = sources.at(Chicane::Box::AssetType::Model);
    if (models.empty())
    {
        throw std::runtime_error(
            "The Mesh [" + Chicane::Box::getTypeTag(Chicane::Box::AssetType::Model) + "] reference file is missing"
        );
    }

    const std::vector<Chicane::FileSystem::Path>& textures = sources.at(Chicane::Box::AssetType::Texture);
    if (textures.empty())
    {
        throw std::runtime_error(
            "The Mesh [" + Chicane::Box::getTypeTag(Chicane::Box::AssetType::Texture) + "] reference file is missing"
        );
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Mesh));

        output = location;
    }

    Chicane::Box::Mesh asset(output);
    asset.setId(inId);

    Chicane::Box::Model            model(models.at(0));
    Chicane::Box::ModelParsed::Map modelGroups = model.getData();

    if (modelGroups.empty())
    {
        throw std::runtime_error(
            "The Mesh [" + Chicane::Box::getTypeTag(Chicane::Box::AssetType::Model) + "] groups are missing"
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

    Chicane::FileSystem::Path materialPath =
        output.withExtension(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Material));

    Chicane::Box::Material material(materialPath);
    material.setId(texture.getId());
    material.setTexture(texture.getFilepath(), texture.getId());
    material.saveXML();

    for (const auto& [reference, data] : modelGroups)
    {
        Chicane::Box::MeshGroup group;
        group.setId(reference);
        group.setModel(model.getFilepath(), reference);
        group.setMaterial(material.getFilepath(), material.getId());

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
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Model));

        output = location;
    }

    Chicane::Box::Model asset(output);
    asset.setId(inId);
    asset.setData(source);
    asset.saveXML();
    Chicane::Box::embedPreview(output);
}

namespace
{
    Chicane::FileSystem::Path skyTexturePath(
        const Chicane::FileSystem::Path& inDirectory, const Chicane::String& inId, const Chicane::String& inSuffix
    )
    {
        Chicane::String filename = inId;
        if (!inSuffix.isEmpty())
        {
            filename.append("_");
            filename.append(inSuffix);
        }
        filename.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Texture));

        return inDirectory / filename;
    }

    void bakeTexturePreview(Chicane::Box::Texture& inTexture)
    {
        if (Chicane::Image::Instance data = inTexture.getData().lock())
        {
            if (Chicane::Box::AssetPreview::write(inTexture.getXML(), Chicane::Box::AssetType::Texture, *data))
            {
                inTexture.saveXML();
            }
        }
    }

    Chicane::FileSystem::Path writeSkyTexture(
        const Chicane::String&           inId,
        const Chicane::FileSystem::Path& inSource,
        const Chicane::FileSystem::Path& inOutput
    )
    {
        Chicane::Box::Texture asset(inOutput);
        asset.setId(inId);
        asset.setData(inSource);
        bakeTexturePreview(asset);
        asset.saveXML();

        return asset.getFilepath();
    }

    Chicane::FileSystem::Path writeSkyTexture(
        const Chicane::String& inId, const Chicane::Image& inImage, const Chicane::FileSystem::Path& inOutput
    )
    {
        Chicane::Box::Texture asset(inOutput);
        asset.setId(inId);
        asset.setVendor(Chicane::ImageVendor::Png);
        asset.setData(inImage.encode());
        bakeTexturePreview(asset);
        asset.saveXML();

        return asset.getFilepath();
    }

    Chicane::Image cropSkyFace(const Chicane::Image& inImage, int inX, int inY, int inSize)
    {
        const int channels = std::max(1, inImage.getChannel());
        const int width    = inImage.getWidth();
        const int height   = inImage.getHeight();
        if (!inImage.getPixels() || inSize <= 0 || inX < 0 || inY < 0 || inX + inSize > width || inY + inSize > height)
        {
            throw std::runtime_error("The sky cube image does not contain a complete set of faces");
        }

        std::vector<unsigned char> pixels(
            static_cast<std::size_t>(inSize) * static_cast<std::size_t>(inSize) * channels
        );
        const unsigned char* source = inImage.getPixels();
        for (int row = 0; row < inSize; row++)
        {
            std::memcpy(
                pixels.data() + static_cast<std::size_t>(row) * inSize * channels,
                source + (static_cast<std::size_t>(inY + row) * width + inX) * channels,
                static_cast<std::size_t>(inSize) * channels
            );
        }

        return Chicane::Image(pixels.data(), inSize, inSize, channels, inImage.getFormat());
    }

    std::vector<Chicane::FileSystem::Path> splitCubeImage(
        const Chicane::String& inId, const Chicane::Image& inImage, const Chicane::FileSystem::Path& inDirectory
    )
    {
        const int                          width  = inImage.getWidth();
        const int                          height = inImage.getHeight();
        int                                face   = 0;
        std::array<std::pair<int, int>, 6> cells  = {};

        if (width > 0 && height > 0 && (width % 4) == 0 && (height % 3) == 0 && (width / 4) == (height / 3))
        {
            face  = width / 4;
            cells = {
                {{2, 1}, {0, 1}, {1, 1}, {3, 1}, {1, 0}, {1, 2}}
            };
        }
        else if (width > 0 && height > 0 && (width % 3) == 0 && (height % 4) == 0 && (width / 3) == (height / 4))
        {
            face  = width / 3;
            cells = {
                {{2, 1}, {0, 1}, {1, 1}, {1, 3}, {1, 0}, {1, 2}}
            };
        }
        else if (height > 0 && width == height * 6)
        {
            face  = height;
            cells = {
                {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}}
            };
        }
        else if (width > 0 && height == width * 6)
        {
            face  = width;
            cells = {
                {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}}
            };
        }

        if (face <= 0)
        {
            return {};
        }

        static const std::array<Chicane::Box::SkySide, 6> sides = {
            Chicane::Box::SkySide::Right,
            Chicane::Box::SkySide::Left,
            Chicane::Box::SkySide::Front,
            Chicane::Box::SkySide::Back,
            Chicane::Box::SkySide::Up,
            Chicane::Box::SkySide::Down
        };

        std::vector<Chicane::FileSystem::Path> textures;
        textures.reserve(sides.size());
        for (std::size_t index = 0; index < sides.size(); index++)
        {
            const Chicane::String side = Chicane::toString(sides.at(index));
            Chicane::String       id   = inId;
            id.append("_");
            id.append(side);

            const Chicane::Image cropped =
                cropSkyFace(inImage, cells.at(index).first * face, cells.at(index).second * face, face);
            textures.push_back(writeSkyTexture(id, cropped, skyTexturePath(inDirectory, inId, side)));
        }

        return textures;
    }
}

void Program::createSky(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    std::vector<Chicane::FileSystem::Path> models;
    std::vector<Chicane::FileSystem::Path> textures;
    std::vector<Chicane::FileSystem::Path> images;

    for (const Chicane::String& source : inSources)
    {
        const Chicane::FileSystem::Path path(source);
        const Chicane::Box::AssetType   type = Chicane::Box::getTypeFromExtension(path);

        if (!Chicane::FileSystem::exists(path))
        {
            throw std::runtime_error("The Sky [" + Chicane::Box::getTypeTag(type) + "] reference file doesn't exist");
        }

        if (type == Chicane::Box::AssetType::Model)
        {
            models.push_back(path);

            continue;
        }

        if (type != Chicane::Box::AssetType::Texture)
        {
            continue;
        }

        const Chicane::String extension = path.extension().toString().toLower();
        if (extension.equals(Chicane::Box::Texture::EXTENSION))
        {
            textures.push_back(path);

            continue;
        }

        images.push_back(path);
    }

    Chicane::FileSystem::Path output = inOutput;
    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Sky));

        output = location;
    }

    Chicane::FileSystem::Path directory = output.parent();
    if (directory.isEmpty())
    {
        directory = ".";
    }

    if (textures.empty() && images.size() == 1)
    {
        const Chicane::Image source(images.front());
        if (!source.isHdr())
        {
            textures = splitCubeImage(inId, source, directory);
        }

        if (textures.empty())
        {
            textures.push_back(writeSkyTexture(inId, images.front(), skyTexturePath(directory, inId, "")));
        }
    }
    else if (textures.empty() && !images.empty())
    {
        static const std::array<Chicane::Box::SkySide, 6> sides = {
            Chicane::Box::SkySide::Right,
            Chicane::Box::SkySide::Left,
            Chicane::Box::SkySide::Front,
            Chicane::Box::SkySide::Back,
            Chicane::Box::SkySide::Up,
            Chicane::Box::SkySide::Down
        };

        for (std::size_t index = 0; index < images.size(); index++)
        {
            Chicane::String suffix;
            if (images.size() == 6 && index < sides.size())
            {
                suffix = Chicane::toString(sides.at(index));
            }
            else if (images.size() > 1)
            {
                suffix = Chicane::String(std::to_string(index));
            }

            Chicane::String id = inId;
            if (!suffix.isEmpty())
            {
                id.append("_");
                id.append(suffix);
            }

            textures.push_back(writeSkyTexture(id, images.at(index), skyTexturePath(directory, inId, suffix)));
        }
    }

    if (textures.empty())
    {
        throw std::runtime_error(
            "The Sky [" + Chicane::Box::getTypeTag(Chicane::Box::AssetType::Texture) + "] reference files are missing"
        );
    }

    const Chicane::Box::SkyKind kind =
        textures.size() == 1 ? Chicane::Box::SkyKind::Panorama : Chicane::Box::SkyKind::Cube;
    const Chicane::FileSystem::Path modelPath =
        models.empty() ? Chicane::FileSystem::Path(
                             kind == Chicane::Box::SkyKind::Panorama ? Chicane::Box::Sky::DOME_SOURCE
                                                                     : Chicane::Box::Sky::BOX_SOURCE
                         )
                       : models.front();

    if (!Chicane::FileSystem::exists(modelPath))
    {
        throw std::runtime_error(
            "The Sky [" + Chicane::Box::getTypeTag(Chicane::Box::AssetType::Model) + "] reference file is missing"
        );
    }

    Chicane::Box::Sky asset(output);
    asset.setId(inId);

    Chicane::XmlNode root = asset.getXML();
    if (!root.empty())
    {
        const Chicane::XmlNode existingTextures = root.getChild(Chicane::Box::Sky::TEXTURES_TAG);
        if (!existingTextures.empty())
        {
            root.removeChild(existingTextures);
        }

        const Chicane::XmlNode existingModel = root.getChild(Chicane::Box::Model::TAG);
        if (!existingModel.empty())
        {
            root.removeChild(existingModel);
        }

        const Chicane::XmlNode existingKind = root.getChild("Kind");
        if (!existingKind.empty())
        {
            root.removeChild(existingKind);
        }

        root.removeAttribute(Chicane::Box::Sky::KIND_ATTRIBUTE_NAME);
    }

    Chicane::Box::Model            model(modelPath);
    Chicane::Box::ModelParsed::Map modelGroups = model.getData();
    if (modelGroups.empty())
    {
        throw std::runtime_error(
            "The Sky [" + Chicane::Box::getTypeTag(Chicane::Box::AssetType::Model) + "] groups are missing"
        );
    }

    asset.setKind(kind);
    asset.setModel(model.getFilepath(), modelGroups.begin()->first);
    asset.addTexture(textures);

    const Chicane::Box::ModelParsed& parsed   = modelGroups.begin()->second;
    Chicane::Vertex::List            vertices = parsed.vertices;
    Chicane::Vertex::Indices         indices  = parsed.indices;

    std::vector<Chicane::Image::Instance> faces = {};
    for (const Chicane::FileSystem::Path& path : textures)
    {
        Chicane::Box::Texture texture(path);
        if (Chicane::Image::Instance data = texture.getData().lock())
        {
            faces.push_back(data);
            bakeTexturePreview(texture);
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
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Texture));

        output = location;
    }

    Chicane::Box::Texture asset(output);
    asset.setId(inId);
    asset.setData(source);
    asset.saveXML();
}

void Program::createMaterial(
    const Chicane::String&                    inId,
    const Chicane::ProgramParam::Positionals& inSources,
    const Chicane::FileSystem::Path&          inOutput
)
{
    if (inSources.empty())
    {
        throw std::runtime_error("The material texture file is missing");
    }

    const Chicane::FileSystem::Path source = inSources.at(0);

    if (!Chicane::FileSystem::exists(source))
    {
        throw std::runtime_error("The material texture file doesn't exist");
    }

    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Material));

        output = location;
    }

    Chicane::Box::Texture texture(source);

    Chicane::Box::Material asset(output);
    asset.setId(inId);
    asset.setTexture(texture.getFilepath(), texture.getId());
    asset.saveXML();
    Chicane::Box::embedPreview(output);
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
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Sound));

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
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Skeleton));

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

void Program::createEffect(
    const Chicane::String& inId, const Chicane::ProgramParam::Positionals&, const Chicane::FileSystem::Path& inOutput
)
{
    Chicane::FileSystem::Path output = inOutput;

    if (output.isEmpty())
    {
        Chicane::String location = inId;
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Effect));

        output = location;
    }

    Chicane::Box::Effect asset(output);
    asset.setId(inId);
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

        if (Chicane::Box::getTypeFromExtension(path) == Chicane::Box::AssetType::Skeleton)
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
        location.append(Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Animation));

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

static Chicane::FileSystem::Path resolveOutputDirectory(const Chicane::FileSystem::Path& inOutput)
{
    if (inOutput.isEmpty())
    {
        return ".";
    }

    if (inOutput.hasExtension() && Chicane::Box::getTypeFromExtension(inOutput) != Chicane::Box::AssetType::Undefined)
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

    const Chicane::Box::ImportResult result = Chicane::Box::importSource(inSource, directory, id);
    for (const Chicane::FileSystem::Path& path : result.written)
    {
        logGenerated(path);
    }
}

void Program::bakePreviews(const Chicane::FileSystem::Path& inRoot)
{
    const Chicane::FileSystem::Path root = inRoot.isEmpty() ? Chicane::FileSystem::Path("Assets") : inRoot;
    if (!Chicane::FileSystem::exists(root))
    {
        throw std::runtime_error("Bake path does not exist");
    }

    std::vector<Chicane::FileSystem::Path> runtimeAssets;
    std::vector<Chicane::FileSystem::Path> cpuAssets;

    auto collect = [&](const Chicane::FileSystem::Path& inPath)
    {
        if (!Chicane::Box::isFileAsset(inPath))
        {
            return;
        }

        if (Chicane::Box::getTypeFromExtension(inPath) == Chicane::Box::AssetType::Texture)
        {
            try
            {
                Chicane::Box::Texture texture(inPath);
                if (texture.bakeMips())
                {
                    texture.saveXML();
                    std::cout << "Baked mips for [" << inPath.toString() << "]" << std::endl;
                }
            }
            catch (const std::exception& exception)
            {
                std::cerr << "Failed to bake mips for [" << inPath.toString() << "]: " << exception.what() << std::endl;
            }
        }

        switch (Chicane::Box::getTypeFromExtension(inPath))
        {
        case Chicane::Box::AssetType::Mesh:
        case Chicane::Box::AssetType::Model:
        case Chicane::Box::AssetType::Material:
            runtimeAssets.push_back(inPath);
            break;

        default:
            cpuAssets.push_back(inPath);
            break;
        }
    };

    if (root.isFile())
    {
        collect(root);
    }
    else
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root.toStandard()))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            collect(Chicane::FileSystem::Path(entry.path()));
        }
    }

    auto bakeCpu = [](const Chicane::FileSystem::Path& inPath)
    {
        if (Chicane::Box::embedPreview(inPath))
        {
            std::cout << "Generated a preview for [" << inPath.toString() << "]" << std::endl;

            return;
        }

        std::cerr << "Failed to generated a preview for[" << inPath.toString() << "]" << std::endl;
    };

    for (const Chicane::FileSystem::Path& path : cpuAssets)
    {
        bakeCpu(path);
    }

    if (runtimeAssets.empty())
    {
        return;
    }

    try
    {
        bakePreviewsWithRuntime(runtimeAssets);
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Runtime preview bake failed: " << exception.what() << std::endl;
        for (const Chicane::FileSystem::Path& path : runtimeAssets)
        {
            bakeCpu(path);
        }
    }
}