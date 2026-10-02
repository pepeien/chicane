#pragma once

#include <vector>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid/Component/Container.hpp"
#include "Chicane/Grid/Route.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID RouterView : public Container
        {
        public:
            static constexpr inline const char* TAG_ID = "RouterView";

        public:
            CH_CONSTRUCTOR()
            RouterView(const XmlNode& inNode);

        public:
            void tick(float inDeltaTime) override;

        protected:
            void refreshSize() override;

        private:
            struct MountedPage
            {
                String     path = {};
                Component* page = nullptr;
            };

            void syncRoute();
            void applyVisibility();
            void mount(const Route& inRoute);
            Component* findPage(const String& inPath) const;

        private:
            String                   m_mounted;
            std::vector<MountedPage> m_pages;
        };
    }
}
