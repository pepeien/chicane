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
        // File
        static constexpr inline const char*   EXTENSION         = ".track";
        static constexpr inline const char*   PREVIEW_DIRECTORY = "Assets/Engine/Levels/Preview";

        // Tag
        static constexpr inline const char*   TAG_ID = "Track";

        // Attributes
        static constexpr inline const char*   VERSION_ATTRIBUTE_NAME              = "version";
        static constexpr inline const char*   ID_ATTRIBUTE_NAME                   = "id";
        static constexpr inline const char*   RELATIVE_TRANSLATION_ATTRIBUTE_NAME = "relativeTranslation";
        static constexpr inline const char*   RELATIVE_ROTATION_ATTRIBUTE_NAME    = "relativeRotation";
        static constexpr inline const char*   RELATIVE_SCALE_ATTRIBUTE_NAME       = "relativeScale";
        static constexpr inline const char*   ABSOLUTE_TRANSLATION_ATTRIBUTE_NAME = "absoluteTranslation";
        static constexpr inline const char*   ABSOLUTE_ROTATION_ATTRIBUTE_NAME    = "absoluteRotation";
        static constexpr inline const char*   ABSOLUTE_SCALE_ATTRIBUTE_NAME       = "absoluteScale";

        // Versioning
        static constexpr inline std::uint32_t CURRENT_VERSION = 1;

        CHICANE_RUNTIME const ReflectionTypeInfo* findType(const String& inTag);

        CHICANE_RUNTIME void applyAttributes(Object& inObject, const XmlNode& inNode);
        CHICANE_RUNTIME bool applyField(Object& inObject, const String& inName, const String& inValue);

        CHICANE_RUNTIME Actor* spawnActor(Scene& inScene, const XmlNode& inNode);
        CHICANE_RUNTIME Component* spawnComponent(Scene& inScene, const XmlNode& inNode, Object* inParent);

        CHICANE_RUNTIME void open(Scene& inScene, const FileSystem::Path& inFilepath);
        CHICANE_RUNTIME void save(const Scene& inScene, const FileSystem::Path& inFilepath);
    }
}
