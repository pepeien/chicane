#pragma once

#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class Vec3 : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* VALUE_ATTRIBUTE_NAME = "value";

    public:
        CH_CONSTRUCTOR()
        Vec3(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void commit();

    private:
        bool isEditing() const;
        bool resolveBinding(Chicane::ReflectionFieldAccessor& outAccessor, void*& outInstance);
        void refreshValue();
        void applyValue();

    public:
        CH_FIELD()
        Chicane::Vec3 value;
    };
}
