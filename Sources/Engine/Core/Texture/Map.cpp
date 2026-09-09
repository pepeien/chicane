#include "Chicane/Core/Texture/Map.hpp"

namespace Chicane
{
    String toString(TextureMap inValue)
    {
        switch (inValue)
        {
        case TextureMap::AmbientOcclusion:
            return "AmbientOcclusion";

        case TextureMap::Normal:
            return "Normal";

        case TextureMap::Roughness:
            return "Roughness";

        case TextureMap::Metalness:
            return "Metalness";

        case TextureMap::Height:
            return "Height";

        case TextureMap::Specular:
            return "Specular";

        case TextureMap::Opacity:
            return "Opacity";

        case TextureMap::Refraction:
            return "Refraction";

        case TextureMap::SelfIllumination:
            return "SelfIllumination";

        case TextureMap::Bump:
            return "Bump";

        case TextureMap::Reflection:
            return "Reflection";

        case TextureMap::Base:
        default:
            return "Texture";
        }
    }

    TextureMap toTextureMap(const String& inValue)
    {
        if (inValue.equals("AmbientOcclusion"))
        {
            return TextureMap::AmbientOcclusion;
        }

        if (inValue.equals("Normal"))
        {
            return TextureMap::Normal;
        }

        if (inValue.equals("Roughness"))
        {
            return TextureMap::Roughness;
        }

        if (inValue.equals("Metalness"))
        {
            return TextureMap::Metalness;
        }

        if (inValue.equals("Height"))
        {
            return TextureMap::Height;
        }

        if (inValue.equals("Specular"))
        {
            return TextureMap::Specular;
        }

        if (inValue.equals("Opacity"))
        {
            return TextureMap::Opacity;
        }

        if (inValue.equals("Refraction"))
        {
            return TextureMap::Refraction;
        }

        if (inValue.equals("SelfIllumination"))
        {
            return TextureMap::SelfIllumination;
        }

        if (inValue.equals("Bump"))
        {
            return TextureMap::Bump;
        }

        if (inValue.equals("Reflection"))
        {
            return TextureMap::Reflection;
        }

        if (inValue.equals("Texture"))
        {
            return TextureMap::Base;
        }

        return TextureMap::Count;
    }
}
