#include "Chicane/Grid/Component/Input/Color.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Window/Event/Type.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Grid
    {
        static String toHexRgb(std::uint8_t inR, std::uint8_t inG, std::uint8_t inB)
        {
            return String::sprint("#%02X%02X%02X", inR, inG, inB);
        }

        static String toHexRgba(std::uint8_t inR, std::uint8_t inG, std::uint8_t inB, std::uint8_t inA)
        {
            return String::sprint("#%02X%02X%02X%02X", inR, inG, inB, inA);
        }

        static std::uint8_t toByte(float inValue)
        {
            return static_cast<std::uint8_t>(std::round(std::clamp(inValue, 0.0f, 1.0f) * 255.0f));
        }

        static void rgbToHsv(float inR, float inG, float inB, float& outH, float& outS, float& outV)
        {
            const float max   = std::max(inR, std::max(inG, inB));
            const float min   = std::min(inR, std::min(inG, inB));
            const float delta = max - min;

            outV = max;
            outS = max <= Math::EPSILON ? 0.0f : delta / max;

            if (delta <= Math::EPSILON)
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

        static void hsvToRgb(float inH, float inS, float inV, float& outR, float& outG, float& outB)
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

        static float wheelHue(float inDx, float inDy)
        {
            float hue = std::atan2(inDx, -inDy) * Math::RAD_TO_DEG;
            if (hue < 0.0f)
            {
                hue += 360.0f;
            }

            return hue;
        }

        static void rgbToHsl(float inR, float inG, float inB, float& outH, float& outS, float& outL)
        {
            const float max   = std::max(inR, std::max(inG, inB));
            const float min   = std::min(inR, std::min(inG, inB));
            const float delta = max - min;

            outL = (max + min) * 0.5f;

            if (delta <= Math::EPSILON)
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

        static float hueToRgb(float inP, float inQ, float inT)
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

        static void hslToRgb(float inH, float inS, float inL, float& outR, float& outG, float& outB)
        {
            const float s = std::clamp(inS, 0.0f, 1.0f);
            const float l = std::clamp(inL, 0.0f, 1.0f);

            if (s <= Math::EPSILON)
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

        InputColor::InputColor(const XmlNode& inNode)
            : Container(inNode),
              hex("#FFFFFF"),
              swatchHex("#FFFFFF"),
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
              model("HSV"),
              models({"RGB", "RGBA", "HSV"}),
              isHsl(false),
              isHsv(true),
              isRgb(false),
              isRgba(false),
              isOpen(false),
              openState("closed"),
              rgbState("idle"),
              rgbaState("idle"),
              hsvState("active"),
              wheelCursorX(50.0f),
              wheelCursorY(50.0f),
              valueDim(0.0f),
              valueThumbY(0.0f),
              alphaUnit(1.0f),
              presets({"#FFFFFF", "#000000", "#808080", "#FF0000", "#0000FF"}),
              m_bIsPickingWheel(false),
              m_bIsPickingValue(false)
        {
            load("Assets/Engine/UI/Components/Input/Color.grid", "Assets/Engine/UI/Components/Input/Color.decal");
            refreshWheel();
        }

        bool InputColor::isFocusable() const
        {
            return false;
        }

        bool InputColor::escapesOverflow() const
        {
            return isOpen;
        }

        bool InputColor::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                if (m_bIsPickingWheel || m_bIsPickingValue)
                {
                    m_bIsPickingWheel = false;
                    m_bIsPickingValue = false;

                    return true;
                }

                if (isOpen && inEvent.data)
                {
                    const Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                    if (event.button == Input::MouseButton::Left)
                    {
                        Component* hit = hasRoot() ? getRoot()->getHitAt(event.location) : nullptr;
                        bool       bInside = false;
                        for (Component* node = hit; node != nullptr; node = node->getParent())
                        {
                            if (node == this)
                            {
                                bInside = true;

                                break;
                            }

                            if (node->isRoot())
                            {
                                break;
                            }
                        }

                        if (!bInside)
                        {
                            close();
                        }
                    }
                }

                return false;
            }

            if (inEvent.type == WindowEventType::MouseButtonDown)
            {
                if (!inEvent.data)
                {
                    return false;
                }

                const Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                if (event.button != Input::MouseButton::Left)
                {
                    return false;
                }

                if (pickWheelAt(event.location))
                {
                    m_bIsPickingWheel = true;
                    m_bIsPickingValue = false;

                    return true;
                }

                if (pickValueAt(event.location))
                {
                    m_bIsPickingValue = true;
                    m_bIsPickingWheel = false;

                    return true;
                }

                return false;
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                if (!inEvent.data)
                {
                    return false;
                }

                const Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);
                if (m_bIsPickingWheel)
                {
                    pickWheelAt(event.location);

                    return true;
                }

                if (m_bIsPickingValue)
                {
                    pickValueAt(event.location);

                    return true;
                }

                return false;
            }

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
            const String trimmed = hex.trim();
            const bool   bHasAlpha = trimmed.startsWith("#") && trimmed.size() == 9;
            const Color::Rgba parsed = Color::toRgba(hex);
            const float       a      = bHasAlpha ? parsed.a / 255.0f : alphaUnit;
            syncFromRgb(parsed.r / 255.0f, parsed.g / 255.0f, parsed.b / 255.0f, a);
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

            applyRgb(r, g, b, alphaUnit);
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

            applyRgb(r, g, b, alphaUnit);
            applyToBinding();
            emitInput();
        }

        void InputColor::commitRgb()
        {
            syncFromRgb(red / 255.0f, green / 255.0f, blue / 255.0f, alphaUnit);
            applyToBinding();
            emitInput();
        }

        void InputColor::commitModel()
        {
            refreshModelFlags();
            refreshTabState();
            syncFromRgb(red / 255.0f, green / 255.0f, blue / 255.0f, alphaUnit);
            refreshStyleSubtree();
        }

        void InputColor::setRgbModel()
        {
            if (model.equals("RGB"))
            {
                return;
            }

            model = "RGB";
            commitModel();
        }

        void InputColor::setRgbaModel()
        {
            if (model.equals("RGBA"))
            {
                return;
            }

            model = "RGBA";
            commitModel();
        }

        void InputColor::setHsvModel()
        {
            if (model.equals("HSV"))
            {
                return;
            }

            model = "HSV";
            commitModel();
        }

        void InputColor::pickPreset(String inHex)
        {
            if (inHex.isEmpty())
            {
                return;
            }

            const Color::Rgba parsed = Color::toRgba(inHex);
            syncFromRgb(parsed.r / 255.0f, parsed.g / 255.0f, parsed.b / 255.0f, alphaUnit);
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

                    if (const Vec4* vector = accessor.getValue<Vec4>(instance))
                    {
                        syncFromRgb(vector->x, vector->y, vector->z, vector->w);

                        return;
                    }

                    if (const Vec3* vector = accessor.getValue<Vec3>(instance))
                    {
                        syncFromRgb(vector->x, vector->y, vector->z, alphaUnit);

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

                    if (Vec4* vector = accessor.getValue<Vec4>(instance))
                    {
                        *vector = Vec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);

                        return;
                    }

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

            alpha     = a * 100.0f;
            alphaUnit = a;
            red       = r * 255.0f;
            green     = g * 255.0f;
            blue      = b * 255.0f;
            hex       = isRgba ? toHexRgba(toByte(r), toByte(g), toByte(b), toByte(a))
                               : toHexRgb(toByte(r), toByte(g), toByte(b));
            swatchHex = toHexRgb(toByte(r), toByte(g), toByte(b));

            refreshDerivedColors();
            refreshWheel();
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

            if ((isHsv ? hsvS : hslS) > Math::EPSILON)
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
            isHsl  = model.equals("HSL");
            isHsv  = model.equals("HSV");
            isRgb  = model.equals("RGB");
            isRgba = model.equals("RGBA");

            if (!isHsl && !isHsv && !isRgb && !isRgba)
            {
                model = "HSV";
                isHsv = true;
            }

            refreshTabState();
        }

        void InputColor::refreshWheel()
        {
            const float radius = saturation / 100.0f;
            const float angle  = hue * Math::DEG_TO_RAD;

            wheelCursorX = (0.5f + std::sin(angle) * radius * 0.5f) * 100.0f;
            wheelCursorY = (0.5f - std::cos(angle) * radius * 0.5f) * 100.0f;
            valueDim     = 1.0f - std::clamp(brightness / 100.0f, 0.0f, 1.0f);
            valueThumbY  = valueDim * 100.0f;
        }

        void InputColor::refreshTabState()
        {
            rgbState  = isRgb ? "active" : "idle";
            rgbaState = isRgba ? "active" : "idle";
            hsvState  = isHsv ? "active" : "idle";
        }

        Color::Rgba InputColor::currentRgba() const
        {
            return Color::Rgba(
                toByte(red / 255.0f),
                toByte(green / 255.0f),
                toByte(blue / 255.0f),
                toByte(alphaUnit)
            );
        }

        Component* InputColor::findChildId(const String& inId) const
        {
            if (inId.isEmpty())
            {
                return nullptr;
            }

            if (getId().equals(inId))
            {
                return const_cast<InputColor*>(this);
            }

            for (Component* child : getChildrenFlat())
            {
                if (child && child->getId().equals(inId))
                {
                    return child;
                }
            }

            return nullptr;
        }

        bool InputColor::pickWheelAt(const Vec2& inLocation)
        {
            Component* wheel = findChildId("colorWheel");
            if (!wheel || !wheel->isDisplayable())
            {
                return false;
            }

            const Bounds2D box    = wheel->getDrawBounds();
            const float    width  = box.right - box.left;
            const float    height = box.bottom - box.top;
            const float    radius = std::min(width, height) * 0.5f;
            if (radius <= 1.0f)
            {
                return false;
            }

            const float dx   = inLocation.x - (box.left + width * 0.5f);
            const float dy   = inLocation.y - (box.top + height * 0.5f);
            const float dist = std::sqrt(dx * dx + dy * dy);
            if (!m_bIsPickingWheel && dist > radius)
            {
                return false;
            }

            hue        = wheelHue(dx, dy);
            saturation = std::clamp(dist / radius, 0.0f, 1.0f) * 100.0f;

            commitHsv();
            refreshStyleSubtree();

            return true;
        }

        bool InputColor::pickValueAt(const Vec2& inLocation)
        {
            Component* bar = findChildId("colorValue");
            if (!bar || !bar->isDisplayable())
            {
                return false;
            }

            const Bounds2D box    = bar->getDrawBounds();
            const float    height = box.bottom - box.top;
            if (height <= 1.0f)
            {
                return false;
            }

            if (!m_bIsPickingValue && !box.contains(inLocation))
            {
                return false;
            }

            const float t = std::clamp((inLocation.y - box.top) / height, 0.0f, 1.0f);
            brightness    = (1.0f - t) * 100.0f;
            commitHsv();
            refreshStyleSubtree();

            return true;
        }
    }
}
