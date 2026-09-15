#pragma once

#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::AssetSelector))
    class AssetSelector : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* VALUE_ATTRIBUTE_NAME    = "value";
        static constexpr inline const char* KIND_ATTRIBUTE_NAME     = "kind";
        static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";

    public:
        CH_CONSTRUCTOR()
        AssetSelector(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onBrowse();

        CH_FUNCTION()
        void onClear();

    private:
        void refreshFromBinding();
        void applyToBinding();
        void emitInput();
        void refreshPreview();
        Chicane::Box::AssetType assetType() const;
        Chicane::String typeLabel() const;
        Chicane::String typeExtension() const;

    public:
        CH_FIELD()
        Chicane::String value;

        CH_FIELD()
        Chicane::String kind;

        CH_FIELD()
        Chicane::String label;

        CH_FIELD()
        bool hasPreview;

        CH_FIELD()
        bool isEmpty;

        CH_FIELD()
        bool hasValue;
    };
}
