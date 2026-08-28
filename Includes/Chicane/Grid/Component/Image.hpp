#pragma once

#include "Chicane/Box/Texture.hpp"

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Image : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Image";

            // Attributes
            static constexpr inline const char* SRC_ATTRIBUTE_NAME           = "src";
            static constexpr inline const char* PLAYBACK_RATE_ATTRIBUTE_NAME = "playback-rate";

            static constexpr inline float       GIF_MAX_FPS      = 24.0f;
            static constexpr inline float       GIF_MIN_FRAME_MS = 1000.0f / GIF_MAX_FPS;

        public:
            CH_CONSTRUCTOR()
            Image(const pugi::xml_node& inNode);

        protected:
            void onRefresh() override;
            void onTick(float inDeltaTime) override;
            void refreshSize() override;

        private:
            void refreshSource();
            void refreshPlaybackRate();
            void bindFrame();
            void advanceFrame(float inDeltaTime);

        public:
            CH_FIELD()
            String src;

            CH_FIELD()
            float playbackRate;

        private:
            const Box::Texture* m_texture;
            std::size_t         m_frame;
            float               m_elapsed;
        };
    }
}
