#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Svg.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Icon : public Svg
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Icon";

            // Attributes
            static constexpr inline const char* NAME_ATTRIBUTE_NAME   = "name";
            static constexpr inline const char* SOURCE_ATTRIBUTE_NAME = "source";

        public:
            CH_CONSTRUCTOR()
            Icon(const XmlNode& inNode);

        protected:
            void onRefresh() override;
            void onTick(float inDeltaTime) override;

        private:
            String toCamelCase(const String& inValue) const;

            FileSystem::Path resolveSource(const String& inName, const String& inFolder) const;
            void applySource(const FileSystem::Path& inPath);
            void refreshSource();

        private:
            String          m_nameBinding;
            String          m_sourceBinding;
            String          m_resolvedName;
            Xml::Attributes m_usageAttributes;
        };
    }
}
