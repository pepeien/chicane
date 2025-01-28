#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/CubeMap/Data.hpp"
#include "Chicane/Box/Asset/CubeMap/Side.hpp"
#include "Chicane/Box/Asset/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace CubeMap
        {
            class Instance : public Asset::Instance
            {
            public:
                Instance(const std::string& inFilepath);

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
}