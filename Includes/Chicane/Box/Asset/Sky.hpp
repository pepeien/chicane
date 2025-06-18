#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Sky/Types.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"
#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Sky : public Asset
        {
        public:
            static inline constexpr const char* EXTENSION = ".bsky";
            static inline constexpr const char* TAG       = "Sky";

            static inline constexpr const char* SIDES_TAG_NAME              = "Sides";
            static inline constexpr const char* TEXTURE_SIDE_ATTRIBUTE_NAME = "side";

            static inline const std::vector<SkySide> ORDER = {
                SkySide::Left,
                SkySide::Right,
                SkySide::Front,
                SkySide::Back,
                SkySide::Up,
                SkySide::Down
            };

            static inline const std::map<String, SkySide> SIDE_MAP = {
                { "UP",    SkySide::Up },
                { "DOWN",  SkySide::Down },
                { "LEFT",  SkySide::Left },
                { "RIGHT", SkySide::Right },
                { "FRONT", SkySide::Front },
                { "BACK",  SkySide::Back }
            };

        public:
            Sky(const FileSystem::Path& inFilepath);
            virtual ~Sky() = default;

        public:
            const SkyRawSides& getSides() const;
            const SkyRawSide& getSide(SkySide inSide) const;
            void setSide(SkySide inSide, const SkyRawSide& inFilepath);

            const String& getModel() const;
            void setModel(const String& inModel);

        private:
            SkySide getSideFromString(const String& inValue) const;
            void fetchSides();
            void fetchModel();

        private:
            SkyRawSides m_sides;
            String      m_model;
        };
    }
}