#include "Chicane/Runtime/Scene/Actor/Sky.reflected.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Box/Texture.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Renderer/Light/Type.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    static constexpr float SUN_DISTANCE = 16.0f;

    static Vec3 brightestPanoramaDirection(const Image& inImage)
    {
        const int width   = inImage.getWidth();
        const int height  = inImage.getHeight();
        const int channel = std::max(inImage.getChannel(), 1);
        if (width <= 0 || height <= 0)
        {
            return Vec3::Zero();
        }

        const float*         floats = inImage.getFloatPixels();
        const unsigned char* bytes  = inImage.getPixels();
        if (!floats && !bytes)
        {
            return Vec3::Zero();
        }

        double    sum   = 0.0;
        float     peak  = -1.0f;
        int       peakX = 0;
        int       peakY = 0;
        const int count = width * height;
        for (int index = 0; index < count; index++)
        {
            float red   = 0.0f;
            float green = 0.0f;
            float blue  = 0.0f;
            if (floats)
            {
                const float* pixel = floats + static_cast<std::size_t>(index) * static_cast<std::size_t>(channel);
                red                = pixel[0];
                green              = channel > 1 ? pixel[1] : red;
                blue               = channel > 2 ? pixel[2] : red;
            }
            else
            {
                const unsigned char* pixel =
                    bytes + static_cast<std::size_t>(index) * static_cast<std::size_t>(channel);
                red   = static_cast<float>(pixel[0]) / 255.0f;
                green = channel > 1 ? static_cast<float>(pixel[1]) / 255.0f : red;
                blue  = channel > 2 ? static_cast<float>(pixel[2]) / 255.0f : red;
            }

            const float luma = red + green + blue;
            sum += luma;
            if (luma <= peak)
            {
                continue;
            }

            peak  = luma;
            peakX = index % width;
            peakY = index / width;
        }

        const double average = sum / static_cast<double>(count);
        if (peak <= 0.0f || peak < average * 2.0)
        {
            return Vec3::Zero();
        }

        const float u        = (static_cast<float>(peakX) + 0.5f) / static_cast<float>(width);
        const float v        = (static_cast<float>(peakY) + 0.5f) / static_cast<float>(height);
        const float yaw      = (u - 0.5f) / 0.15915494309f;
        const float pitch    = (0.5f - v) / 0.31830988618f;
        const float cosPitch = std::cos(pitch);

        return Vec3(std::sin(yaw) * cosPitch, std::cos(yaw) * cosPitch, std::sin(pitch));
    }

    ASky::ASky()
        : Actor(),
          sky(),
          m_sun(nullptr),
          intensity(0.35f),
          exposure(1.0f),
          bIsVisible(true),
          m_asset(nullptr)
    {}

    void ASky::onLoad()
    {
        if (!m_sun)
        {
            for (Component* attachment : getAttachments())
            {
                if (CLight* light = dynamic_cast<CLight*>(attachment))
                {
                    m_sun = light;

                    break;
                }
            }
        }

        if (!m_sun)
        {
            m_sun = getScene()->createComponent<CLight>();
            m_sun->attachTo(this);
            m_sun->setType(LightType::Environment);
            m_sun->setColor(Vec3(1.0f));
            m_sun->setCanCastShadows(false);
            m_sun->activate();
        }

        setIntensity(intensity);

        placeEnvironmentLightAtSun();
    }

    void ASky::onPropertyEdited(const String& inName)
    {
        if (inName.equals(SKY_ATTRIBUTE_NAME))
        {
            if (sky.isEmpty() || !FileSystem::exists(sky))
            {
                m_asset = nullptr;

                return;
            }

            setSky(Box::load<Box::Sky>(sky));

            return;
        }

        if (inName.equals(INTENSITY_ATTRIBUTE_NAME))
        {
            setIntensity(intensity);

            return;
        }

        if (inName.equals(EXPOSURE_ATTRIBUTE_NAME))
        {
            setExposure(exposure);
        }
    }

    const Box::Sky* ASky::getSky() const
    {
        return m_asset;
    }

    void ASky::setSky(const Box::Sky* inSky)
    {
        m_asset = inSky;
        sky     = inSky ? inSky->getFilepath() : FileSystem::Path();
        placeEnvironmentLightAtSun();
    }

    float ASky::getIntensity() const
    {
        return intensity;
    }

    void ASky::setIntensity(float inValue)
    {
        intensity = std::max(inValue, 0.0f);

        if (m_sun)
        {
            m_sun->setIntensity(intensity);
        }
    }

    float ASky::getExposure() const
    {
        return exposure;
    }

    void ASky::setExposure(float inValue)
    {
        exposure = std::max(inValue, 0.0f);
    }

    bool ASky::isVisible() const
    {
        return bIsVisible;
    }

    void ASky::setVisible(bool inValue)
    {
        bIsVisible = inValue;
    }

    void ASky::placeEnvironmentLightAtSun()
    {
        if (!m_sun || !m_asset || m_asset->getKind() != Box::SkyKind::Panorama)
        {
            return;
        }

        const Box::AssetReference::List& textures = m_asset->getTextures();
        if (textures.empty())
        {
            return;
        }

        const Box::Texture*   texture   = Box::load<Box::Texture>(textures.front().getSource());
        const Image::Instance image     = texture ? texture->getData().lock() : Image::Instance();
        const Vec3            direction = image ? brightestPanoramaDirection(*image) : Vec3::Zero();
        if (direction.dot(direction) <= 1e-8f)
        {
            m_sun->setRelativeTranslation(Vec3::Zero());

            return;
        }

        const Vec3 position = direction.normalize() * SUN_DISTANCE;
        Rotator    facing;
        facing.lookAt(position, Vec3::Zero());
        m_sun->setRelativeTranslation(position);
        m_sun->setRelativeRotation(facing);
        m_sun->setAbsolute(*this);
    }
}
