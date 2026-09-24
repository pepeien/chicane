#include "Chicane/Core/Texture/Material.hpp"

namespace Chicane
{
    String toString(TextureMaterial inValue)
    {
        switch (inValue)
        {
        case TextureMaterial::AmbientOcclusion:
            return "AmbientOcclusion";

        case TextureMaterial::Normal:
            return "Normal";

        case TextureMaterial::Roughness:
            return "Roughness";

        case TextureMaterial::Metalness:
            return "Metalness";

        case TextureMaterial::Height:
            return "Height";

        case TextureMaterial::Specular:
            return "Specular";

        case TextureMaterial::Opacity:
            return "Opacity";

        case TextureMaterial::Refraction:
            return "Refraction";

        case TextureMaterial::SelfIllumination:
            return "SelfIllumination";

        case TextureMaterial::Bump:
            return "Bump";

        case TextureMaterial::Reflection:
            return "Reflection";

        case TextureMaterial::Albedo:
        default:
            return "Albedo";
        }
    }

    TextureMaterial toTextureMaterial(const String& inValue)
    {
        if (inValue.equals("AmbientOcclusion"))
        {
            return TextureMaterial::AmbientOcclusion;
        }

        if (inValue.equals("Normal"))
        {
            return TextureMaterial::Normal;
        }

        if (inValue.equals("Roughness"))
        {
            return TextureMaterial::Roughness;
        }

        if (inValue.equals("Metalness"))
        {
            return TextureMaterial::Metalness;
        }

        if (inValue.equals("Height"))
        {
            return TextureMaterial::Height;
        }

        if (inValue.equals("Specular"))
        {
            return TextureMaterial::Specular;
        }

        if (inValue.equals("Opacity"))
        {
            return TextureMaterial::Opacity;
        }

        if (inValue.equals("Refraction"))
        {
            return TextureMaterial::Refraction;
        }

        if (inValue.equals("SelfIllumination"))
        {
            return TextureMaterial::SelfIllumination;
        }

        if (inValue.equals("Bump"))
        {
            return TextureMaterial::Bump;
        }

        if (inValue.equals("Reflection"))
        {
            return TextureMaterial::Reflection;
        }

        if (inValue.equals("Albedo") || inValue.equals("Texture"))
        {
            return TextureMaterial::Albedo;
        }

        return TextureMaterial::Count;
    }
}
