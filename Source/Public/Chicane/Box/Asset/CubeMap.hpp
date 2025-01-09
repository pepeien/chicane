#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/CubeMap.hpp"
#include "Chicane/Box/Asset/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        class CubeMap : public Asset::Instance
        {
        public:
            typedef Chicane::CubeMap::Side Side;

        public:
            static inline std::string EXTENSION = ".bcmp";
            static inline std::string TAG       = "Cubemap";

        public:
            CubeMap(const std::string& inFilepath);

        public:
            const std::unordered_map<Side, std::string>& getTextures() const;
            const std::string& getTexture(Side inSide) const;
            void setTexture(Side inSide, const std::string& inFilepath);

        private:
            Side getSideFromString(const std::string& inValue) const;
            void fetchTextures();

        private:
            std::unordered_map<Side, std::string> m_textures;
        };
    }
}