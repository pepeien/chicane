#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        class Cubemap : public Asset
        {
        public:
            enum class Side
            {
                Left,
                Right,
                Front,
                Back,
                Up,
                Down
            };

        public:
            static inline std::string EXTENSION = ".bcmp";
            static inline std::string TAG       = "Cubemap";

        public:
            Cubemap(const std::string& inFilepath);

        public:
            const std::unordered_map<Side, std::string>& getTextures() const;
            const std::string& getTexture(Side inSide) const;

        private:
            Side getSideFromString(const std::string& inValue) const;
            void fetchTextures();

        private:
            std::unordered_map<Side, std::string> m_textures;
        };
    }
}