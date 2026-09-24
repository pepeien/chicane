#include "Chicane/Box/Import/Source.hpp"

#include <array>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Chicane/Box/Animation.hpp"
#include "Chicane/Box/Animation/Gltf.hpp"
#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Box/Asset/Type.hpp"
#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Import/Texture.hpp"
#include "Chicane/Box/Material.hpp"
#include "Chicane/Box/Mesh.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Model/Gltf.hpp"
#include "Chicane/Box/Model/Vendor.hpp"
#include "Chicane/Box/Skeleton.hpp"
#include "Chicane/Box/Skeleton/Gltf.hpp"
#include "Chicane/Box/Sound.hpp"
#include "Chicane/Box/Texture.hpp"
#include "Chicane/Box/Texture/Gltf.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Worker/Pool.hpp"

namespace Chicane
{
    namespace Box
    {
        static String sanitizeName(const String& inValue)
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

            return result.empty() ? String("Asset") : String(result);
        }

        static String uniqueName(const String& inName, std::unordered_set<String>& outUsed)
        {
            String name = inName.isEmpty() ? String("Mesh") : inName;
            if (outUsed.insert(name).second)
            {
                return name;
            }

            std::uint32_t index = 1;
            while (true)
            {
                const String candidate = String::sprint("%s_%u", name.toChar(), index);
                if (outUsed.insert(candidate).second)
                {
                    return candidate;
                }

                index++;
            }
        }

        static void ensureParent(const FileSystem::Path& inFile)
        {
            const FileSystem::Path parent = inFile.parent();
            if (parent.isEmpty() || parent.exists())
            {
                return;
            }

            std::filesystem::create_directories(parent.toStandard());
        }

        static bool hasRawExtension(AssetType inType, const String& inExtension)
        {
            for (const FileSystem::Path& extension : getTypeRawExtensions(inType))
            {
                if (inExtension.equals(extension.toString()))
                {
                    return true;
                }
            }

            return false;
        }

        template <typename T>
        static ImportResult importSingle(
            const FileSystem::Path& inSource, const FileSystem::Path& inDirectory, const String& inId, AssetType inType
        )
        {
            const FileSystem::Path output = inDirectory / (inId + getTypeExtension(inType));
            ensureParent(output);

            T asset(output);
            asset.setId(inId);
            asset.setData(inSource);
            asset.saveXML();

            ImportResult result;
            result.primary = output;
            result.written.push_back(output);

            return result;
        }

        static void generateTangents(ModelParsed::Map& outGeometry)
        {
            std::vector<ModelParsed*> models;
            models.reserve(outGeometry.size());
            for (auto& [name, model] : outGeometry)
            {
                models.push_back(&model);
            }

            WorkerPool::parallel(
                models.size(),
                [&models](std::size_t inIndex) { Model::generateTangents(*models[inIndex]); }
            );
        }

        static ImportResult importGltf(
            const FileSystem::Path& inSource, const FileSystem::Path& inDirectory, const String& inId
        )
        {
            ModelGltf::Scene scene = ModelGltf::parseScene(FileSystem::readUnsigned(inSource));
            if (scene.geometry.empty())
            {
                throw std::runtime_error("The glTF/GLB file has no meshes");
            }

            generateTangents(scene.geometry);

            const FileSystem::Path modelPath = inDirectory / (inId + getTypeExtension(AssetType::Model));
            ensureParent(modelPath);

            Model model(modelPath);
            model.setId(inId);
            model.setCooked(scene.geometry);
            model.saveXML();

            ImportResult result;
            result.written.push_back(modelPath);

            const TextureGltf::Parsed  textures = TextureGltf::parse(inSource);
            std::vector<ImportTexture> written(textures.images.size());
            for (std::size_t index = 0; index < textures.images.size(); index++)
            {
                const TextureGltf::Entry& image = textures.images[index];
                if (image.data.empty())
                {
                    continue;
                }

                const String name = sanitizeName(image.id);
                if (index == 0)
                {
                    written[index].path = inDirectory / (inId + getTypeExtension(AssetType::Texture));
                    written[index].id   = inId;
                }
                else
                {
                    written[index].path = inDirectory / inId / (name + getTypeExtension(AssetType::Texture));
                    written[index].id   = inId + "/" + name;
                }

                ensureParent(written[index].path);
            }

            WorkerPool::parallel(
                written.size(),
                [&textures, &written](std::size_t inIndex)
                {
                    const TextureGltf::Entry& image = textures.images[inIndex];
                    if (image.data.empty() || written[inIndex].path.isEmpty())
                    {
                        return;
                    }

                    Texture asset(written[inIndex].path);
                    asset.setId(written[inIndex].id);
                    asset.setVendor(image.vendor);
                    asset.setData(image.data);
                    asset.saveXML();

                    if (Image::Instance data = asset.getData().lock())
                    {
                        written[inIndex].image = data;
                    }
                }
            );

            for (const ImportTexture& texture : written)
            {
                if (!texture.path.isEmpty())
                {
                    result.written.push_back(texture.path);
                }
            }

            FileSystem::Path fallbackPath = Texture::DEFAULT_SOURCE;
            String           fallbackId   = Texture::DEFAULT_REFERENCE;
            Image::Instance  fallbackImage;
            for (const ImportTexture& texture : written)
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

            if (fallbackPath == Texture::DEFAULT_SOURCE && !FileSystem::exists(fallbackPath))
            {
                const std::array<unsigned char, 4> pixel = {255, 255, 255, 255};
                const Image                        image(pixel.data(), 1, 1, 4, 4);
                const FileSystem::Path             path = inDirectory / (inId + getTypeExtension(AssetType::Texture));

                ensureParent(path);

                Texture asset(path);
                asset.setId(inId);
                asset.setVendor(ImageVendor::Png);
                asset.setData(image.encode());
                asset.saveXML();

                fallbackPath = path;
                fallbackId   = inId;
                if (Image::Instance data = asset.getData().lock())
                {
                    fallbackImage = data;
                }

                result.written.push_back(path);
            }

            const std::vector<String> animationNames = AnimationGltf::list(inSource);
            FileSystem::Path          skeletonPath;
            if (SkeletonGltf::hasSkin(inSource) || !animationNames.empty())
            {
                skeletonPath = inDirectory / (inId + getTypeExtension(AssetType::Skeleton));
                ensureParent(skeletonPath);

                Skeleton asset(skeletonPath);
                asset.setId(inId);
                asset.setBones(SkeletonGltf::parse(inSource));
                asset.saveXML();
                result.written.push_back(skeletonPath);
            }

            std::vector<FileSystem::Path> animations;
            if (!skeletonPath.isEmpty())
            {
                Skeleton bones(skeletonPath);

                for (std::uint32_t index = 0; index < animationNames.size(); index++)
                {
                    AnimationClip clip;
                    try
                    {
                        clip = AnimationGltf::parse(inSource, index);
                    }
                    catch (const std::exception& exception)
                    {
                        std::cerr << "Skipped animation [" << animationNames[index] << "]: " << exception.what()
                                  << std::endl;

                        continue;
                    }

                    AnimationTrack::List tracks;
                    for (const AnimationTrack& track : clip.tracks)
                    {
                        if (track.name.isEmpty() || !bones.hasBone(track.name))
                        {
                            continue;
                        }

                        tracks.push_back(track);
                    }

                    if (tracks.empty())
                    {
                        std::cerr << "Skipped animation [" << animationNames[index]
                                  << "]: no tracks match skeleton bones" << std::endl;

                        continue;
                    }

                    clip.tracks = std::move(tracks);

                    const String           animId = sanitizeName(animationNames[index]);
                    const FileSystem::Path animPath =
                        animationNames.size() == 1
                            ? inDirectory / (inId + getTypeExtension(AssetType::Animation))
                            : inDirectory / inId / (animId + getTypeExtension(AssetType::Animation));

                    ensureParent(animPath);

                    Animation asset(animPath);
                    asset.setId(animId);
                    asset.setSkeleton(skeletonPath);
                    asset.setClip(clip);
                    asset.saveXML();

                    animations.push_back(animPath);
                    result.written.push_back(animPath);
                }
            }

            const FileSystem::Path meshPath = inDirectory / (inId + getTypeExtension(AssetType::Mesh));
            ensureParent(meshPath);

            Mesh mesh(meshPath);
            mesh.setId(inId);
            mesh.setGroups({});
            mesh.setAnimations({});

            std::vector<PreviewGeometryBatch>                batches;
            std::unordered_set<String>                       usedGroups;
            std::unordered_map<std::int32_t, AssetReference> writtenMaterials;
            std::unordered_set<String>                       usedMaterials;
            std::size_t                                      groupCount = 0;

            auto writeMaterial = [&](std::int32_t inIndex) -> AssetReference
            {
                if (inIndex >= 0)
                {
                    const auto found = writtenMaterials.find(inIndex);
                    if (found != writtenMaterials.end())
                    {
                        return found->second;
                    }
                }

                AssetReference reference;
                reference.setSource(Material::GRAY_SOURCE);
                reference.setReference(Material::GRAY_REFERENCE);

                if (inIndex >= 0)
                {
                    const auto materialMaps = textures.materials.find(inIndex);
                    if (materialMaps != textures.materials.end())
                    {
                        Material::TextureMaps maps       = {};
                        bool                  bHasAlbedo = false;
                        for (const auto& [map, imageIndex] : materialMaps->second)
                        {
                            if (imageIndex < 0 || static_cast<std::size_t>(imageIndex) >= written.size())
                            {
                                continue;
                            }

                            const ImportTexture& texture = written.at(static_cast<std::size_t>(imageIndex));
                            if (texture.path.isEmpty())
                            {
                                continue;
                            }

                            AssetReference textureRef;
                            textureRef.setSource(texture.path);
                            textureRef.setReference(texture.id);
                            maps[map] = textureRef;
                            if (map == TextureMaterial::Albedo)
                            {
                                bHasAlbedo = true;
                            }
                        }

                        if (bHasAlbedo)
                        {
                            const String           name = uniqueName("Material", usedMaterials);
                            const FileSystem::Path path =
                                inDirectory / inId / (name + getTypeExtension(AssetType::Material));
                            ensureParent(path);

                            Material asset(path);
                            asset.setId(name);
                            for (const auto& [map, texture] : maps)
                            {
                                asset.setTexture(map, texture.getSource().toString(), texture.getReference());
                            }
                            asset.saveXML();
                            result.written.push_back(path);

                            reference.setSource(path);
                            reference.setReference(name);
                        }
                    }
                }

                if (inIndex >= 0)
                {
                    writtenMaterials[inIndex] = reference;
                }

                return reference;
            };

            for (const ModelGltf::Instance& instance : scene.instances)
            {
                const auto found = scene.geometry.find(instance.id);
                if (found == scene.geometry.end())
                {
                    continue;
                }

                const ModelParsed& data      = found->second;
                const String       groupName = instance.bone.isEmpty()
                                                   ? instance.id
                                                   : String::sprint("%s_%s", instance.id.toChar(), instance.bone.toChar());

                MeshGroup group;
                group.setId(uniqueName(groupName, usedGroups));
                group.setBone(instance.bone);
                group.setModel(model.getFilepath(), instance.id);
                group.setTransform(instance.transform);
                group.setMaterial(writeMaterial(data.material));

                if (data.material >= 0)
                {
                    const auto strength = textures.emissiveStrengths.find(data.material);
                    if (strength != textures.emissiveStrengths.end())
                    {
                        group.setEmissiveStrength(strength->second);
                    }
                }

                if (!group.isValid())
                {
                    std::cerr << "Skipping mesh group [" << group.getId() << "]" << std::endl;

                    continue;
                }

                mesh.appendGroup(group);
                groupCount++;
            }

            if (groupCount == 0)
            {
                throw std::runtime_error("Failed to generate a mesh from the glTF/GLB file");
            }

            for (const auto& [reference, data] : scene.geometry)
            {
                PreviewGeometryBatch batch;
                batch.vertices = data.vertices;
                if (data.indices.empty())
                {
                    for (Vertex::Index index = 0; index < static_cast<Vertex::Index>(data.vertices.size()); index++)
                    {
                        batch.indices.push_back(index);
                    }
                }
                else
                {
                    batch.indices = data.indices;
                }

                batch.texture = fallbackImage;
                if (data.material >= 0)
                {
                    const auto material = textures.materials.find(data.material);
                    if (material != textures.materials.end())
                    {
                        const auto albedo = material->second.find(TextureMaterial::Albedo);
                        if (albedo != material->second.end() && albedo->second >= 0 &&
                            static_cast<std::size_t>(albedo->second) < written.size() &&
                            written[static_cast<std::size_t>(albedo->second)].image)
                        {
                            batch.texture = written[static_cast<std::size_t>(albedo->second)].image;
                        }
                    }
                }

                batches.push_back(std::move(batch));
            }

            if (!skeletonPath.isEmpty())
            {
                mesh.setSkeleton(skeletonPath);
            }

            for (const FileSystem::Path& animation : animations)
            {
                mesh.appendAnimation(animation);
            }

            if (std::unique_ptr<AssetPreview> preview = AssetPreview::createFromGeometry(meshPath, batches))
            {
                if (preview->image)
                {
                    AssetPreview::write(mesh.getXML(), AssetType::Mesh, *preview->image);
                }
            }

            mesh.saveXML();
            result.written.push_back(meshPath);
            result.primary = meshPath;

            return result;
        }

        ImportResult importSource(
            const FileSystem::Path& inSource, const FileSystem::Path& inDirectory, const String& inId
        )
        {
            if (inSource.isEmpty() || !FileSystem::exists(inSource))
            {
                throw std::runtime_error("The source file doesn't exist");
            }

            FileSystem::Path directory = inDirectory;
            if (directory.isEmpty())
            {
                directory = inSource.parent();
            }
            if (directory.isEmpty())
            {
                directory = ".";
            }

            const String id        = sanitizeName(inId.isEmpty() ? inSource.stem().toString() : inId);
            const String extension = inSource.extension().toString().toLower();

            if (hasRawExtension(AssetType::Model, extension))
            {
                if (Model::parseVendor(extension) == ModelVendor::Gltf)
                {
                    return importGltf(inSource, directory, id);
                }

                return importSingle<Model>(inSource, directory, id, AssetType::Model);
            }

            if (hasRawExtension(AssetType::Sound, extension))
            {
                return importSingle<Sound>(inSource, directory, id, AssetType::Sound);
            }

            if (hasRawExtension(AssetType::Font, extension))
            {
                return importSingle<Font>(inSource, directory, id, AssetType::Font);
            }

            return importSingle<Texture>(inSource, directory, id, AssetType::Texture);
        }
    }
}
