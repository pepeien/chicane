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
                Front,
                Back,
                Left,
                Right,
                Up,
                Down
            };

        public:
            static inline std::string EXTENSION = ".bcbm";
            static inline std::string TAG       = "Cubemap";

        public:
            Cubemap(const std::string& inFilepath);

        public:
            const std::unordered_map<Side, std::string>& getTextures() const;
            const std::string& getTexture(Side inSide) const;

        private:
            void fetchTextures();

        private:
            std::unordered_map<Side, std::string> m_textures;
        };
    }
}