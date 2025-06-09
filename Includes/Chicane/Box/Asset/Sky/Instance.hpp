#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Sky/Types.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sky
        {
            class CHICANE_BOX Instance : public Asset::Instance
            {
            public:
                Instance(const FileSystem::Path& inFilepath);
                virtual ~Instance() = default;

            public:
                const RawSides& getSides() const;
                const RawSide& getSide(Side inSide) const;
                void setSide(Side inSide, const RawSide& inFilepath);

                const String& getModel() const;
                void setModel(const String& inModel);

            private:
                Side getSideFromString(const String& inValue) const;
                void fetchSides();
                void fetchModel();

            private:
                RawSides    m_sides;
                String m_model;
            };
        }
    }
}