#include "Chicane/Box/Texture/Gltf.hpp"

#include <algorithm>

#include "../Gltf/Parser.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Image.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace TextureGltf
        {
            static ImageVendor vendorFromBytes(const unsigned char* inBytes, std::uint64_t inCount)
            {
                if (!inBytes || inCount < 4)
                {
                    return ImageVendor::Undefined;
                }

                if (inBytes[0] == 0x89 && inBytes[1] == 0x50 && inBytes[2] == 0x4E && inBytes[3] == 0x47)
                {
                    return ImageVendor::Png;
                }

                if (inBytes[0] == 0xFF && inBytes[1] == 0xD8 && inBytes[2] == 0xFF)
                {
                    return ImageVendor::Jpg;
                }

                if (inCount >= 6 && inBytes[0] == 'G' && inBytes[1] == 'I' && inBytes[2] == 'F')
                {
                    return ImageVendor::Gif;
                }

                return ImageVendor::Undefined;
            }

            static ImageVendor vendorFromImage(const tg3_image& inImage, const Image::Raw& inData)
            {
                const String mime = toString(inImage.mime_type).toUpper();
                if (mime.contains("PNG"))
                {
                    return ImageVendor::Png;
                }

                if (mime.contains("JPEG") || mime.contains("JPG"))
                {
                    return ImageVendor::Jpg;
                }

                if (mime.contains("GIF"))
                {
                    return ImageVendor::Gif;
                }

                const String      uri     = toString(inImage.uri);
                const ImageVendor fromUri = Chicane::Image::parseVendor(uri);
                if (fromUri != ImageVendor::Undefined)
                {
                    return fromUri;
                }

                const ImageVendor fromBytes = vendorFromBytes(
                    inData.empty() ? nullptr : inData.data(),
                    static_cast<std::uint64_t>(inData.size())
                );
                if (fromBytes != ImageVendor::Undefined)
                {
                    return fromBytes;
                }

                return ImageVendor::Png;
            }

            static String imageId(const tg3_image& inImage, std::uint32_t inIndex)
            {
                const String name = toString(inImage.name);
                if (!name.isEmpty())
                {
                    return name;
                }

                const String uri = toString(inImage.uri);
                if (!uri.isEmpty() && !uri.startsWith("data:"))
                {
                    const FileSystem::Path path(uri);
                    const String           stem = path.stem().toString();
                    if (!stem.isEmpty())
                    {
                        return stem;
                    }
                }

                return String::sprint("Image_%u", inIndex);
            }

            static Image::Raw readBufferView(const tg3_model& inModel, std::int32_t inView)
            {
                if (inView < 0 || static_cast<std::uint32_t>(inView) >= inModel.buffer_views_count)
                {
                    return {};
                }

                const tg3_buffer_view& view = inModel.buffer_views[inView];
                if (view.buffer < 0 || static_cast<std::uint32_t>(view.buffer) >= inModel.buffers_count)
                {
                    return {};
                }

                const tg3_buffer& buffer = inModel.buffers[view.buffer];
                if (!buffer.data.data)
                {
                    return {};
                }

                if (view.byte_offset + view.byte_length > buffer.data.count)
                {
                    return {};
                }

                const unsigned char* start = buffer.data.data + view.byte_offset;

                return Image::Raw(start, start + view.byte_length);
            }

            static Image::Raw readUri(const FileSystem::Path& inFilepath, const String& inUri)
            {
                if (inUri.isEmpty())
                {
                    return {};
                }

                if (inUri.startsWith("data:"))
                {
                    const std::size_t comma = inUri.find(',');
                    if (comma == String::npos)
                    {
                        return {};
                    }

                    return Base64::decodeToUnsigned(inUri.substr(comma + 1));
                }

                FileSystem::Path path(inUri);
                if (path.isRelative())
                {
                    path = inFilepath.parent() / path;
                }

                if (!FileSystem::exists(path))
                {
                    return {};
                }

                return FileSystem::readUnsigned(path);
            }

            static Image::Raw readImageData(
                const tg3_model& inModel, const FileSystem::Path& inFilepath, const tg3_image& inImage
            )
            {
                if (inImage.as_is && inImage.image.data && inImage.image.count > 0)
                {
                    return Image::Raw(inImage.image.data, inImage.image.data + inImage.image.count);
                }

                Image::Raw data = readBufferView(inModel, inImage.buffer_view);
                if (!data.empty())
                {
                    return data;
                }

                data = readUri(inFilepath, toString(inImage.uri));
                if (!data.empty())
                {
                    return data;
                }

                if (inImage.image.data && inImage.image.count > 0 && inImage.width > 0 && inImage.height > 0)
                {
                    const int      channels = inImage.component > 0 ? inImage.component : 4;
                    Chicane::Image pixels(inImage.image.data, inImage.width, inImage.height, channels, channels);

                    return pixels.encode();
                }

                if (inImage.image.data && inImage.image.count > 0)
                {
                    return Image::Raw(inImage.image.data, inImage.image.data + inImage.image.count);
                }

                return {};
            }

            static std::int32_t imageIndex(const tg3_model& inModel, std::int32_t inTextureIndex)
            {
                if (inTextureIndex < 0 || static_cast<std::uint32_t>(inTextureIndex) >= inModel.textures_count)
                {
                    return -1;
                }

                const std::int32_t source = inModel.textures[inTextureIndex].source;
                if (source < 0 || static_cast<std::uint32_t>(source) >= inModel.images_count)
                {
                    return -1;
                }

                return source;
            }

            static void assignMap(
                std::map<TextureMap, std::int32_t>& outMaps,
                TextureMap                          inMap,
                const tg3_model&                    inModel,
                std::int32_t                        inTextureIndex
            )
            {
                const std::int32_t index = imageIndex(inModel, inTextureIndex);
                if (index < 0)
                {
                    return;
                }

                outMaps[inMap] = index;
            }

            static float emissiveStrength(const tg3_material& inMaterial)
            {
                const tg3_extras_ext& extras = inMaterial.ext;
                for (std::uint32_t i = 0; i < extras.extensions_count; ++i)
                {
                    const tg3_extension& extension = extras.extensions[i];
                    if (!equals(extension.name, "KHR_materials_emissive_strength"))
                    {
                        continue;
                    }

                    if (extension.value.type != TG3_VALUE_OBJECT || !extension.value.object_data)
                    {
                        break;
                    }

                    for (std::uint32_t k = 0; k < extension.value.object_count; ++k)
                    {
                        const tg3_kv_pair& field = extension.value.object_data[k];
                        if (!equals(field.key, "emissiveStrength"))
                        {
                            continue;
                        }

                        if (field.value.type == TG3_VALUE_REAL)
                        {
                            return static_cast<float>(std::max(0.0, field.value.real_val));
                        }

                        if (field.value.type == TG3_VALUE_INT)
                        {
                            return static_cast<float>(std::max<std::int64_t>(0, field.value.int_val));
                        }
                    }

                    break;
                }

                return 1.0f;
            }

            Parsed parse(const FileSystem::Path& inFilepath)
            {
                const Document   document(inFilepath);
                const tg3_model& model = document.get();

                Parsed result;
                result.images.resize(model.images_count);

                for (std::uint32_t i = 0; i < model.images_count; ++i)
                {
                    const tg3_image& image = model.images[i];
                    Entry            parsed;
                    parsed.id     = imageId(image, i);
                    parsed.data   = readImageData(model, inFilepath, image);
                    parsed.vendor = vendorFromImage(image, parsed.data);

                    result.images[i] = std::move(parsed);
                }

                for (std::uint32_t i = 0; i < model.materials_count; ++i)
                {
                    const tg3_material&                 material = model.materials[i];
                    std::map<TextureMap, std::int32_t>& maps     = result.materials[static_cast<std::int32_t>(i)];

                    assignMap(maps, TextureMap::Base, model, material.pbr_metallic_roughness.base_color_texture.index);
                    assignMap(maps, TextureMap::Normal, model, material.normal_texture.index);
                    // glTF packs AO in R (when shared), roughness in G, metalness in B.
                    assignMap(
                        maps,
                        TextureMap::Roughness,
                        model,
                        material.pbr_metallic_roughness.metallic_roughness_texture.index
                    );
                    assignMap(
                        maps,
                        TextureMap::Metalness,
                        model,
                        material.pbr_metallic_roughness.metallic_roughness_texture.index
                    );
                    assignMap(maps, TextureMap::AmbientOcclusion, model, material.occlusion_texture.index);
                    assignMap(maps, TextureMap::SelfIllumination, model, material.emissive_texture.index);

                    const float strength = emissiveStrength(material);
                    if (strength != 1.0f)
                    {
                        result.emissiveStrengths[static_cast<std::int32_t>(i)] = strength;
                    }
                }

                return result;
            }
        }
    }
}
