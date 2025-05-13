#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Sky/Data.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sky
        {
            class CHICANE Instance : public Asset::Instance
            {
            public:
                Instance(const std::string& inFilepath);

            public:
                const RawSides& getSides() const;
                const RawSide& getSide(Side inSide) const;
                void setSide(Side inSide, const RawSide& inFilepath);

                const std::string& getModel() const;
                void setModel(const std::string& inModel);

            private:
                Side getSideFromString(const std::string& inValue) const;
                void fetchSides();
                void fetchModel();

            private:
                RawSides    m_sides;
                std::string m_model;
            };
        }
    }
}