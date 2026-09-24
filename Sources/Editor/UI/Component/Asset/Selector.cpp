#include "Editor/UI/Component/Asset/Selector.reflected.hpp"

#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/FileSystem/File/Dialog.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Grid.hpp>

namespace Editor
{
    AssetSelector::AssetSelector(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          value(Chicane::String::empty()),
          kind(Chicane::String::empty()),
          label("None"),
          hasPreview(false),
          isEmpty(true),
          hasValue(false)
    {
        load("Assets/Editor/UI/Components/Asset/Selector.grid", "Assets/Editor/UI/Components/Asset/Selector.decal");
    }

    void AssetSelector::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        const Chicane::String rawKind = parseText(getAttribute(KIND_ATTRIBUTE_NAME)).trim();
        if (!rawKind.isEmpty())
        {
            kind = rawKind;
        }

        refreshFromBinding();
        refreshPreview();
    }

    void AssetSelector::onBrowse()
    {
        Chicane::FileSystem::FileDialog dialog;
        dialog.bCanSelectMany = false;
        dialog.title          = "Select " + typeLabel();

        const Chicane::Box::AssetType type = assetType();
        if (type == Chicane::Box::AssetType::Undefined)
        {
            Chicane::FileSystem::FileFilter::Extensions extensions;
            for (const Chicane::String& extension : Chicane::Box::getExtensions(true))
            {
                extensions.push_back(extension);
            }

            dialog.addFilter("Assets", extensions);
        }
        else
        {
            dialog.addFilter(typeLabel() + "s", {typeExtension()});
        }

        dialog.open(
            [this](const Chicane::FileSystem::Item::List& inFiles)
            {
                for (const Chicane::FileSystem::Item& item : inFiles)
                {
                    if (item.type != Chicane::FileSystem::ItemType::File)
                    {
                        continue;
                    }

                    value = item.path.toString();
                    applyToBinding();
                    emitInput();
                    refreshPreview();

                    return;
                }
            }
        );
    }

    void AssetSelector::onClear()
    {
        value = Chicane::String::empty();
        applyToBinding();
        emitInput();
        refreshPreview();
    }

    void AssetSelector::refreshFromBinding()
    {
        const Chicane::String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
        if (raw.isEmpty() || !isReference(raw))
        {
            return;
        }

        const Chicane::String id =
            raw.getBetween(Chicane::Grid::REFERENCE_VALUE_OPENING, Chicane::Grid::REFERENCE_VALUE_CLOSING).trim();
        if (id.isEmpty())
        {
            return;
        }

        for (Chicane::Grid::Component* node = getParent(); node != nullptr; node = node->getParent())
        {
            const Chicane::ReflectionFieldAccessor accessor = node->getField(id);
            if (accessor.isValid())
            {
                const void* instance =
                    accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(node);

                if (const Chicane::String* text = accessor.getValue<Chicane::String>(instance))
                {
                    value = *text;

                    return;
                }

                if (const Chicane::FileSystem::Path* path = accessor.getValue<Chicane::FileSystem::Path>(instance))
                {
                    value = path->toString();

                    return;
                }
            }

            if (node->isRoot())
            {
                return;
            }
        }
    }

    void AssetSelector::applyToBinding()
    {
        const Chicane::String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
        if (!isReference(raw) || !hasParent())
        {
            return;
        }

        const Chicane::String id =
            raw.getBetween(Chicane::Grid::REFERENCE_VALUE_OPENING, Chicane::Grid::REFERENCE_VALUE_CLOSING).trim();
        if (id.isEmpty())
        {
            return;
        }

        for (Chicane::Grid::Component* node = getParent(); node != nullptr; node = node->getParent())
        {
            const Chicane::ReflectionFieldAccessor accessor = node->getField(id);
            if (accessor.isValid())
            {
                void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                   : static_cast<void*>(node);

                if (Chicane::String* text = accessor.getValue<Chicane::String>(instance))
                {
                    *text = value;

                    return;
                }

                if (Chicane::FileSystem::Path* path = accessor.getValue<Chicane::FileSystem::Path>(instance))
                {
                    *path = Chicane::FileSystem::Path(value);

                    return;
                }
            }

            if (node->isRoot())
            {
                return;
            }
        }
    }

    void AssetSelector::emitInput()
    {
        getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
    }

    void AssetSelector::refreshPreview()
    {
        isEmpty    = value.isEmpty();
        hasValue   = !isEmpty;
        hasPreview = false;
        label      = isEmpty ? "None" : Chicane::FileSystem::Path(value).filename().toString();

        if (isEmpty)
        {
            return;
        }

        const Chicane::FileSystem::Path path(value);
        hasPreview = Chicane::FileSystem::exists(path) && Chicane::Box::isFileAsset(path);
    }

    Chicane::Box::AssetType AssetSelector::assetType() const
    {
        const Chicane::String key = kind.trim().toLower();
        if (key.isEmpty())
        {
            return Chicane::Box::AssetType::Undefined;
        }

        static const Chicane::Box::AssetType types[] = {
            Chicane::Box::AssetType::Animation,
            Chicane::Box::AssetType::Font,
            Chicane::Box::AssetType::Mesh,
            Chicane::Box::AssetType::Model,
            Chicane::Box::AssetType::Skeleton,
            Chicane::Box::AssetType::Texture,
            Chicane::Box::AssetType::Material,
            Chicane::Box::AssetType::Sky,
            Chicane::Box::AssetType::Sound,
            Chicane::Box::AssetType::Effect
        };

        for (const Chicane::Box::AssetType type : types)
        {
            if (Chicane::toString(type).toLower().equals(key))
            {
                return type;
            }

            if (Chicane::Box::getTypeTag(type).toLower().equals(key))
            {
                return type;
            }
        }

        return Chicane::Box::getTypeFromExtension(Chicane::FileSystem::Path(value));
    }

    Chicane::String AssetSelector::typeLabel() const
    {
        const Chicane::Box::AssetType type = assetType();
        if (type == Chicane::Box::AssetType::Undefined)
        {
            return "Asset";
        }

        return Chicane::toString(type);
    }

    Chicane::String AssetSelector::typeExtension() const
    {
        return Chicane::Box::getTypeExtension(assetType());
    }
}
