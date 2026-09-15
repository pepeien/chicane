#include "Chicane/Grid/Component/Input/Color.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace
        {
            constexpr float kEpsilon = 0.0001f;

            String toHexRgb(std::uint8_t inR, std::uint8_t inG, std::uint8_t inB)
            {
                return String::sprint("#%02X%02X%02X", inR, inG, inB);
            }

            std::uint8_t toByte(float inValue)
            {
                return static_cast<std::uint8_t>(std::round(std::clamp(inValue, 0.0f, 1.0f) * 255.0f));
            }

            String formatInt(float inValue)
            {
                return String::sprint("%d", static_cast<int>(std::round(inValue)));
            }

            void rgbToHsv(float inR, float inG, float inB, float& outH, float& outS, float& outV)
            {
                const float max   = std::max(inR, std::max(inG, inB));
                const float min   = std::min(inR, std::min(inG, inB));
                const float delta = max - min;

                outV = max;
                outS = max <= kEpsilon ? 0.0f : delta / max;

                if (delta <= kEpsilon)
                {
                    outH = 0.0f;

                    return;
                }

                if (max == inR)
                {
                    outH = std::fmod((inG - inB) / delta, 6.0f);
                }
                else if (max == inG)
                {
                    outH = (inB - inR) / delta + 2.0f;
                }
                else
                {
                    outH = (inR - inG) / delta + 4.0f;
                }

                outH *= 60.0f;
                if (outH < 0.0f)
                {
                    outH += 360.0f;
                }
            }

            void hsvToRgb(float inH, float inS, float inV, float& outR, float& outG, float& outB)
            {
                const float s = std::clamp(inS, 0.0f, 1.0f);
                const float v = std::clamp(inV, 0.0f, 1.0f);
                const float c = v * s;
                const float x = c * (1.0f - std::fabs(std::fmod(inH / 60.0f, 2.0f) - 1.0f));
                const float m = v - c;

                float r = 0.0f;
                float g = 0.0f;
                float b = 0.0f;

                if (inH < 60.0f)
                {
                    r = c;
                    g = x;
                }
                else if (inH < 120.0f)
                {
                    r = x;
                    g = c;
                }
                else if (inH < 180.0f)
                {
                    g = c;
                    b = x;
                }
                else if (inH < 240.0f)
                {
                    g = x;
                    b = c;
                }
                else if (inH < 300.0f)
                {
                    r = x;
                    b = c;
                }
                else
                {
                    r = c;
                    b = x;
                }

                outR = r + m;
                outG = g + m;
                outB = b + m;
            }

            void rgbToHsl(float inR, float inG, float inB, float& outH, float& outS, float& outL)
            {
                const float max   = std::max(inR, std::max(inG, inB));
                const float min   = std::min(inR, std::min(inG, inB));
                const float delta = max - min;

                outL = (max + min) * 0.5f;

                if (delta <= kEpsilon)
                {
                    outH = 0.0f;
                    outS = 0.0f;

                    return;
                }

                outS = outL > 0.5f ? delta / (2.0f - max - min) : delta / (max + min);

                if (max == inR)
                {
                    outH = std::fmod((inG - inB) / delta, 6.0f);
                }
                else if (max == inG)
                {
                    outH = (inB - inR) / delta + 2.0f;
                }
                else
                {
                    outH = (inR - inG) / delta + 4.0f;
                }

                outH *= 60.0f;
                if (outH < 0.0f)
                {
                    outH += 360.0f;
                }
            }

            float hueToRgb(float inP, float inQ, float inT)
            {
                float t = inT;
                if (t < 0.0f)
                {
                    t += 1.0f;
                }

                if (t > 1.0f)
                {
                    t -= 1.0f;
                }

                if (t < 1.0f / 6.0f)
                {
                    return inP + (inQ - inP) * 6.0f * t;
                }

                if (t < 0.5f)
                {
                    return inQ;
                }

                if (t < 2.0f / 3.0f)
                {
                    return inP + (inQ - inP) * (2.0f / 3.0f - t) * 6.0f;
                }

                return inP;
            }

            void hslToRgb(float inH, float inS, float inL, float& outR, float& outG, float& outB)
            {
                const float s = std::clamp(inS, 0.0f, 1.0f);
                const float l = std::clamp(inL, 0.0f, 1.0f);

                if (s <= kEpsilon)
                {
                    outR = l;
                    outG = l;
                    outB = l;

                    return;
                }

                const float q  = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
                const float p  = 2.0f * l - q;
                const float hk = inH / 360.0f;

                outR = hueToRgb(p, q, hk + 1.0f / 3.0f);
                outG = hueToRgb(p, q, hk);
                outB = hueToRgb(p, q, hk - 1.0f / 3.0f);
            }
        }

        InputColor::InputColor(const pugi::xml_node& inNode)
            : Container(inNode),
              hex("#FFFFFF"),
              vividHex("#FFFFFF"),
              grayHex("#808080"),
              toneHex("#808080"),
              hue(0.0f),
              saturation(0.0f),
              lightness(100.0f),
              brightness(100.0f),
              alpha(100.0f),
              red(255.0f),
              green(255.0f),
              blue(255.0f),
              hueText("0"),
              saturationText("0"),
              lightnessText("100"),
              brightnessText("100"),
              alphaText("100"),
              redText("255"),
              greenText("255"),
              blueText("255"),
              model("HSL"),
              models({"HSL", "HSV", "RGB"}),
              isHsl(true),
              isHsv(false),
              isRgb(false),
              isOpen(false),
              openState("closed"),
              presets({"#FFFFFF", "#000000", "#808080", "#FF0000", "#0000FF"})
        {
            load("Assets/Engine/UI/Components/Input/Color.grid", "Assets/Engine/UI/Components/Input/Color.decal");
        }

        bool InputColor::isFocusable() const
        {
            return false;
        }

        void InputColor::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            if (isEditing())
            {
                return;
            }

            refreshFromBinding();
        }

        void InputColor::toggle()
        {
            if (isOpen)
            {
                close();

                return;
            }

            open();
        }

        void InputColor::open()
        {
            if (isOpen)
            {
                return;
            }

            isOpen    = true;
            openState = "open";
            refreshStyleSubtree();
        }

        void InputColor::close()
        {
            if (!isOpen)
            {
                return;
            }

            isOpen    = false;
            openState = "closed";
            refreshStyleSubtree();
        }

        void InputColor::commitHex()
        {
            const Color::Rgba parsed = Color::toRgba(hex);
            syncFromRgb(parsed.r / 255.0f, parsed.g / 255.0f, parsed.b / 255.0f, parsed.a / 255.0f);
            applyToBinding();
            emitInput();
        }

        void InputColor::commitHsl()
        {
            float r = 0.0f;
            float g = 0.0f;
            float b = 0.0f;
            hslToRgb(hue, saturation / 100.0f, lightness / 100.0f, r, g, b);

            float hsvH = 0.0f;
            float hsvS = 0.0f;
            float hsvV = 0.0f;
            rgbToHsv(r, g, b, hsvH, hsvS, hsvV);
            brightness = hsvV * 100.0f;

            applyRgb(r, g, b, alpha / 100.0f);
            applyToBinding();
            emitInput();
        }

        void InputColor::commitHsv()
        {
            float r = 0.0f;
            float g = 0.0f;
            float b = 0.0f;
            hsvToRgb(hue, saturation / 100.0f, brightness / 100.0f, r, g, b);

            float hslH = 0.0f;
            float hslS = 0.0f;
            float hslL = 0.0f;
            rgbToHsl(r, g, b, hslH, hslS, hslL);
            lightness = hslL * 100.0f;

            applyRgb(r, g, b, alpha / 100.0f);
            applyToBinding();
            emitInput();
        }

        void InputColor::commitRgb()
        {
            syncFromRgb(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 100.0f);
            applyToBinding();
            emitInput();
        }

        void InputColor::commitModel()
        {
            refreshModelFlags();
            syncFromRgb(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 100.0f);
            refreshStyleSubtree();
        }

        void InputColor::pickPreset(String inHex)
        {
            if (inHex.isEmpty())
            {
                return;
            }

            const Color::Rgba parsed = Color::toRgba(inHex);
            syncFromRgb(parsed.r / 255.0f, parsed.g / 255.0f, parsed.b / 255.0f, alpha / 100.0f);
            applyToBinding();
            emitInput();
        }

        void InputColor::addPreset()
        {
            for (const String& preset : presets)
            {
                if (preset.equals(hex))
                {
                    return;
                }
            }

            presets.push_back(hex);
            refreshStyleSubtree();
        }

        bool InputColor::isEditing() const
        {
            if (isOpen || isFocused())
            {
                return true;
            }

            for (Component* child : getChildrenFlat())
            {
                if (child && child->isFocused())
                {
                    return true;
                }
            }

            return false;
        }

        void InputColor::refreshFromBinding()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (raw.isEmpty() || !isReference(raw))
            {
                return;
            }

            const String id = raw.getBetween(REFERENCE_VALUE_OPENING, REFERENCE_VALUE_CLOSING).trim();
            if (id.isEmpty())
            {
                return;
            }

            for (Component* node = getParent(); node != nullptr; node = node->getParent())
            {
                const ReflectionFieldAccessor accessor = node->getField(id);
                if (accessor.isValid())
                {
                    const void* instance =
                        accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(node);

                    if (const Vec3* vector = accessor.getValue<Vec3>(instance))
                    {
                        syncFromRgb(vector->x, vector->y, vector->z, alpha / 100.0f);

                        return;
                    }

                    if (const String* text = accessor.getValue<String>(instance))
                    {
                        const String      value  = text->isEmpty() ? String("#FFFFFF") : *text;
                        const Color::Rgba parsed = Color::toRgba(value);
                        syncFromRgb(parsed.r / 255.0f, parsed.g / 255.0f, parsed.b / 255.0f, parsed.a / 255.0f);

                        return;
                    }
                }

                if (node->isRoot())
                {
                    return;
                }
            }
        }

        void InputColor::applyToBinding()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (!isReference(raw) || !hasParent())
            {
                return;
            }

            const String id = raw.getBetween(REFERENCE_VALUE_OPENING, REFERENCE_VALUE_CLOSING).trim();
            if (id.isEmpty())
            {
                return;
            }

            const Color::Rgba color = currentRgba();

            for (Component* node = getParent(); node != nullptr; node = node->getParent())
            {
                const ReflectionFieldAccessor accessor = node->getField(id);
                if (accessor.isValid())
                {
                    void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                       : static_cast<void*>(node);

                    if (Vec3* vector = accessor.getValue<Vec3>(instance))
                    {
                        *vector = Vec3(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);

                        return;
                    }

                    if (String* text = accessor.getValue<String>(instance))
                    {
                        *text = hex;

                        return;
                    }
                }

                if (node->isRoot())
                {
                    return;
                }
            }
        }

        void InputColor::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        void InputColor::applyRgb(float inR, float inG, float inB, float inA)
        {
            const float r = std::clamp(inR, 0.0f, 1.0f);
            const float g = std::clamp(inG, 0.0f, 1.0f);
            const float b = std::clamp(inB, 0.0f, 1.0f);
            const float a = std::clamp(inA, 0.0f, 1.0f);

            alpha = a * 100.0f;
            red   = r * 255.0f;
            green = g * 255.0f;
            blue  = b * 255.0f;
            hex   = toHexRgb(toByte(r), toByte(g), toByte(b));

            refreshDerivedColors();
            refreshLabels();
        }

        void InputColor::syncFromRgb(float inR, float inG, float inB, float inA)
        {
            const float r = std::clamp(inR, 0.0f, 1.0f);
            const float g = std::clamp(inG, 0.0f, 1.0f);
            const float b = std::clamp(inB, 0.0f, 1.0f);

            float hslH = 0.0f;
            float hslS = 0.0f;
            float hslL = 0.0f;
            rgbToHsl(r, g, b, hslH, hslS, hslL);

            float hsvH = 0.0f;
            float hsvS = 0.0f;
            float hsvV = 0.0f;
            rgbToHsv(r, g, b, hsvH, hsvS, hsvV);

            if ((isHsv ? hsvS : hslS) > kEpsilon)
            {
                hue        = isHsv ? hsvH : hslH;
                saturation = (isHsv ? hsvS : hslS) * 100.0f;
            }

            lightness  = hslL * 100.0f;
            brightness = hsvV * 100.0f;

            applyRgb(r, g, b, inA);
        }

        void InputColor::refreshDerivedColors()
        {
            float vividR = 0.0f;
            float vividG = 0.0f;
            float vividB = 0.0f;
            hslToRgb(hue, 1.0f, 0.5f, vividR, vividG, vividB);
            vividHex = toHexRgb(toByte(vividR), toByte(vividG), toByte(vividB));

            float grayR = 0.0f;
            float grayG = 0.0f;
            float grayB = 0.0f;
            hslToRgb(hue, 0.0f, isHsv ? brightness / 100.0f : lightness / 100.0f, grayR, grayG, grayB);
            grayHex = toHexRgb(toByte(grayR), toByte(grayG), toByte(grayB));

            float toneR = 0.0f;
            float toneG = 0.0f;
            float toneB = 0.0f;
            hslToRgb(hue, saturation / 100.0f, 0.5f, toneR, toneG, toneB);
            toneHex = toHexRgb(toByte(toneR), toByte(toneG), toByte(toneB));
        }

        void InputColor::refreshModelFlags()
        {
            isHsl = model.equals("HSL");
            isHsv = model.equals("HSV");
            isRgb = model.equals("RGB");

            if (!isHsl && !isHsv && !isRgb)
            {
                model = "HSL";
                isHsl = true;
            }
        }

        void InputColor::refreshLabels()
        {
            hueText        = formatInt(hue);
            saturationText = formatInt(saturation);
            lightnessText  = formatInt(lightness);
            brightnessText = formatInt(brightness);
            alphaText      = formatInt(alpha);
            redText        = formatInt(red);
            greenText      = formatInt(green);
            blueText       = formatInt(blue);
        }

        Color::Rgba InputColor::currentRgba() const
        {
            return Color::Rgba(
                toByte(red / 255.0f),
                toByte(green / 255.0f),
                toByte(blue / 255.0f),
                toByte(alpha / 100.0f)
            );
        }
    }
}
