#include "Chicane/Box/Asset/Type.hpp"

#include <unordered_map>

#include "Chicane/Box/Animation.hpp"
#include "Chicane/Box/Effect.hpp"
#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Material.hpp"
#include "Chicane/Box/Mesh.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Skeleton.hpp"
#include "Chicane/Box/Sky.hpp"
#include "Chicane/Box/Sound.hpp"
#include "Chicane/Box/Texture.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<AssetType, String> TAGS = {
            {AssetType::Animation, Animation::TAG},
            {AssetType::Font,      Font::TAG     },
            {AssetType::Mesh,      Mesh::TAG     },
            {AssetType::Model,     Model::TAG    },
            {AssetType::Skeleton,  Skeleton::TAG },
            {AssetType::Sky,       Sky::TAG      },
            {AssetType::Sound,     Sound::TAG    },
            {AssetType::Texture,   Texture::TAG  },
            {AssetType::Material,  Material::TAG },
            {AssetType::Effect,    Effect::TAG   }
        };

        static const std::unordered_map<AssetType, String> EXTENSIONS = {
            {AssetType::Animation, Animation::EXTENSION},
            {AssetType::Font,      Font::EXTENSION     },
            {AssetType::Mesh,      Mesh::EXTENSION     },
            {AssetType::Model,     Model::EXTENSION    },
            {AssetType::Skeleton,  Skeleton::EXTENSION },
            {AssetType::Sky,       Sky::EXTENSION      },
            {AssetType::Sound,     Sound::EXTENSION    },
            {AssetType::Texture,   Texture::EXTENSION  },
            {AssetType::Material,  Material::EXTENSION },
            {AssetType::Effect,    Effect::EXTENSION   }
        };

        static const std::unordered_map<AssetType, std::vector<FileSystem::Path>> RAW_EXTENSIONS = {
            {AssetType::Font,    {".ttf", ".otf"}                                         },
            {AssetType::Model,   {".gltf", ".glb", ".obj"}                                },
            {AssetType::Texture, {".png", ".jpg", ".jpeg", ".gif", ".tga", ".bmp", ".hdr"}},
            {AssetType::Sound,   {".wav", ".mp3", ".flac", ".ogg"}                        },
        };

        bool isFileAsset(const FileSystem::Path& inFilepath)
        {
            if (inFilepath.isEmpty() || !inFilepath.hasExtension())
            {
                return false;
            }

            const String value = inFilepath.extension().toString();

            for (const auto& [type, extension] : EXTENSIONS)
            {
                if (!extension.equals(value))
                {
                    continue;
                }

                return true;
            }

            return value.equals(".anim");
        }

        AssetType getTypeFromExtension(const FileSystem::Path& inValue)
        {
            if (!isFileAsset(inValue))
            {
                return AssetType::Undefined;
            }

            const String value = inValue.extension().toString();

            for (const auto& [type, extension] : EXTENSIONS)
            {
                if (!extension.equals(value))
                {
                    continue;
                }

                return type;
            }

            if (value.equals(".anim"))
            {
                return AssetType::Animation;
            }

            return AssetType::Undefined;
        }

        AssetType getTypeFromTag(const String& inValue)
        {
            if (inValue.isEmpty())
            {
                return AssetType::Undefined;
            }

            const String value = inValue.trim();

            for (const auto& [type, tag] : TAGS)
            {
                if (!tag.equals(value))
                {
                    continue;
                }

                return type;
            }

            return AssetType::Undefined;
        }

        std::vector<String> getTypeTags()
        {
            std::vector<String> result;

            for (const auto& [type, name] : TAGS)
            {
                result.push_back(name);
            }

            return result;
        }

        const String& getTypeTag(AssetType inValue)
        {
            const auto& found = TAGS.find(inValue);

            if (found == TAGS.end())
            {
                return String::sEmpty();
            }

            return found->second;
        }

        std::vector<String> getExtensions(bool bHasDots)
        {
            std::vector<String> result;

            for (const auto& [type, extension] : EXTENSIONS)
            {
                if (bHasDots || extension.size() == 0 || !extension.startsWith('.'))
                {
                    result.push_back(extension);

                    continue;
                }

                result.push_back(extension.substr(1));
            }

            return result;
        }

        const std::vector<FileSystem::Path>& getTypeRawExtensions(AssetType inValue)
        {
            const auto& found = RAW_EXTENSIONS.find(inValue);

            if (found == RAW_EXTENSIONS.end())
            {
                static const std::vector<FileSystem::Path> empty;

                return empty;
            }

            return found->second;
        }

        const String& getTypeExtension(AssetType inValue)
        {
            const auto& found = EXTENSIONS.find(inValue);

            if (found == EXTENSIONS.end())
            {
                return String::sEmpty();
            }

            return found->second;
        }
    }

    String toString(Box::AssetType inValue)
    {
        switch (inValue)
        {
        case Box::AssetType::Undefined:
            return "Undefined";

        case Box::AssetType::Animation:
            return "Animation";

        case Box::AssetType::Font:
            return "Font";

        case Box::AssetType::Mesh:
            return "Mesh";

        case Box::AssetType::Model:
            return "Model";

        case Box::AssetType::Skeleton:
            return "Skeleton";

        case Box::AssetType::Texture:
            return "Texture";

        case Box::AssetType::Material:
            return "Material";

        case Box::AssetType::Sky:
            return "Sky";

        case Box::AssetType::Sound:
            return "Sound";

        case Box::AssetType::Effect:
            return "Effect";

        default:
            return "";
        }
    }
}
