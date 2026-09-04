#include "Chicane/Grid/Component/Image.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Core/Size.hpp"

namespace Chicane
{
    namespace Grid
    {
        Image::Image(const pugi::xml_node& inNode)
            : Container(inNode),
              src(""),
              playbackRate(1.0f),
              m_load(Box::AssetLoad::Full),
              m_previewImage(nullptr),
              m_previewId(""),
              m_texture(nullptr),
              m_frame(0),
              m_elapsed(0.0f)
        {}

        void Image::onRefresh()
        {
            Container::onRefresh();

            refreshSource();
            refreshPlaybackRate();
            bindFrame();
        }

        void Image::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshSource();
            refreshPlaybackRate();
            advanceFrame(inDeltaTime);
            bindFrame();
        }

        void Image::refreshSize()
        {
            const bool bIsWidthAuto  = m_style.width.getRaw().isEmpty() || m_style.width.isRaw(Size::AUTO_KEYWORD);
            const bool bIsHeightAuto = m_style.height.getRaw().isEmpty() || m_style.height.isRaw(Size::AUTO_KEYWORD);

            Container::refreshSize();

            Chicane::Image::Instance frame = m_previewImage;
            if (!frame && m_texture)
            {
                frame = m_texture->getData().lock();
            }

            if (!frame || frame->getWidth() <= 0 || frame->getHeight() <= 0)
            {
                return;
            }

            const float imageWidth  = static_cast<float>(frame->getWidth());
            const float imageHeight = static_cast<float>(frame->getHeight());
            const float aspect      = imageWidth / imageHeight;

            if (bIsWidthAuto && bIsHeightAuto)
            {
                setSize(imageWidth, imageHeight);

                return;
            }

            if (bIsWidthAuto)
            {
                setSize(m_size.y * aspect, m_size.y);

                return;
            }

            if (bIsHeightAuto)
            {
                setSize(m_size.x, m_size.x / aspect);
            }
        }

        void Image::refreshSource()
        {
            refreshLoad();

            const String parsed = parseText(getAttribute(SRC_ATTRIBUTE_NAME)).trim();
            if (!parsed.equals(src))
            {
                src            = parsed;
                m_previewImage = nullptr;
                m_previewId    = "";
                m_texture      = nullptr;
                m_frame        = 0;
                m_elapsed      = 0.0f;
            }

            if (src.isEmpty())
            {
                m_previewImage = nullptr;
                m_previewId    = "";
                m_texture      = nullptr;

                return;
            }

            if (m_load == Box::AssetLoad::Preview)
            {
                m_texture = nullptr;
                Box::requestPreview(src);

                if (const Box::AssetPreview* preview = Box::findPreview(src))
                {
                    m_previewImage = preview->image;
                    m_previewId    = preview->textureId();
                }
                else
                {
                    m_previewImage = nullptr;
                    m_previewId    = "";
                }

                return;
            }

            m_previewImage = nullptr;
            m_previewId    = "";

            if (m_texture)
            {
                return;
            }

            if (src.endsWith(Box::Texture::EXTENSION))
            {
                m_texture = Box::load<Box::Texture>(src);
            }
            else
            {
                m_texture = Box::getById<Box::Texture>(src);
            }
        }

        void Image::refreshLoad()
        {
            const String parsed = parseText(getAttribute(LOAD_ATTRIBUTE_NAME)).trim();
            if (parsed.equals("Preview", "preview"))
            {
                m_load = Box::AssetLoad::Preview;

                return;
            }

            if (parsed.equals("Header", "header"))
            {
                m_load = Box::AssetLoad::Header;

                return;
            }

            m_load = Box::AssetLoad::Full;
        }

        void Image::refreshPlaybackRate()
        {
            const String value = parseText(getAttribute(PLAYBACK_RATE_ATTRIBUTE_NAME)).trim();
            if (value.isEmpty())
            {
                playbackRate = 1.0f;

                return;
            }

            playbackRate = static_cast<float>(std::strtod(value.toChar(), nullptr));
        }

        void Image::bindFrame()
        {
            if (m_previewImage)
            {
                m_style.background.image.setRaw(m_previewId);

                return;
            }

            if (!m_texture)
            {
                m_style.background.image.setRaw("");

                return;
            }

            m_style.background.image.setRaw(m_texture->getFrameId(m_frame));
        }

        void Image::advanceFrame(float inDeltaTime)
        {
            if (m_previewImage || !m_texture || m_texture->getFrameCount() <= 1 || playbackRate == 0.0f)
            {
                return;
            }

            m_elapsed += inDeltaTime;

            const std::size_t count      = m_texture->getFrameCount();
            const bool        bIsReverse = playbackRate < 0.0f;
            const float       rate       = std::abs(playbackRate);

            for (std::size_t step = 0; step < count; step++)
            {
                const int   frameDelay = m_texture->getDelay(m_frame);
                const float delay =
                    std::max(static_cast<float>(frameDelay > 0 ? frameDelay : 100) / rate, GIF_MIN_FRAME_MS);
                if (m_elapsed < delay)
                {
                    break;
                }

                m_elapsed -= delay;
                m_frame = bIsReverse ? (m_frame + count - 1) % count : (m_frame + 1) % count;
            }
        }
    }
}
