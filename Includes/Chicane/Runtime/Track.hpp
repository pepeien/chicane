#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Reflection/Type/Info.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class Actor;
    class Component;
    class Object;
    class Scene;

    namespace Track
    {
        static constexpr inline const char*   EXTENSION = ".track";
        static constexpr inline const char*   TAG_ID    = "Track";

        static constexpr inline const char*   VERSION_ATTRIBUTE_NAME     = "version";
        static constexpr inline const char*   ID_ATTRIBUTE_NAME          = "id";
        static constexpr inline const char*   TRANSLATION_ATTRIBUTE_NAME = "translation";
        static constexpr inline const char*   ROTATION_ATTRIBUTE_NAME    = "rotation";
        static constexpr inline const char*   SCALE_ATTRIBUTE_NAME       = "scale";

        static constexpr inline std::uint32_t CURRENT_VERSION = 1;

        CHICANE_RUNTIME const ReflectionTypeInfo* findType(const String& inTag);

        CHICANE_RUNTIME void applyAttributes(Object& inObject, const pugi::xml_node& inNode);
        CHICANE_RUNTIME bool applyField(Object& inObject, const String& inName, const String& inValue);

        CHICANE_RUNTIME Actor* spawnActor(Scene& inScene, const pugi::xml_node& inNode);
        CHICANE_RUNTIME Component* spawnComponent(Scene& inScene, const pugi::xml_node& inNode, Object* inParent);

        CHICANE_RUNTIME void open(Scene& inScene, const FileSystem::Path& inFilepath);
        CHICANE_RUNTIME void save(const Scene& inScene, const FileSystem::Path& inFilepath);
    }
}
