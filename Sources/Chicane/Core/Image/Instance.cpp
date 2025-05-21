#include "Chicane/Core/Image/Instance.hpp"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Chicane
{
    namespace Image
    {
        Instance::Instance(const FileSystem::Path& inFilepath)
            : Instance()
        {
            m_format = STBI_rgb_alpha;

            m_pixels = stbi_load(
                inFilepath.string().c_str(),
                &m_width,
                &m_height,
                &m_channel,
                m_format
            );

            if (!m_pixels)
            {
                throw std::runtime_error(stbi_failure_reason());
            }
        }

        Instance::Instance(const Raw& inData)
            : Instance()
        {
            m_format = STBI_rgb_alpha;

            m_pixels = stbi_load_from_memory(
                &inData.at(0),
                static_cast<int>(inData.size()),
                &m_width,
                &m_height,
                &m_channel,
                m_format
            );

            if (!m_pixels)
            {
                throw std::runtime_error(stbi_failure_reason());
            }
        }

        Instance::Instance()
            : m_width(0),
            m_height(0),
            m_channel(0),
            m_format(0),
            m_pixels(nullptr)
        {}

        Instance::~Instance()
        {
            free();
        }

        
        int Instance::getWidth() const
        {
            return m_width;
        }

        int Instance::getHeight() const
        {
            return m_height;
        }

        int Instance::getChannel() const
        {
            return m_channel;
        }

        int Instance::getFormat() const
        {
            return m_format;
        }

        const Pixels Instance::getPixels() const
        {
            return m_pixels;
        }

        int Instance::getPitch() const
        {
            return m_format * m_width;
        }

        int Instance::getSize() const
        {
            return m_width * m_height;
        }

        void Instance::free()
        {
            if (m_pixels != nullptr)
            {
                return;
            }

            stbi_image_free(m_pixels);

            m_pixels = nullptr;
        }
    }
}