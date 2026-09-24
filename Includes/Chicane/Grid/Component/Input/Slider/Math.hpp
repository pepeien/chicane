#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#include "Chicane/Core/Input/Keyboard/Button/Modifier.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace SliderMath
        {
            inline bool hasModifier(Input::KeyboardButtonModifier inValue, Input::KeyboardButtonModifier inFlag)
            {
                return (static_cast<std::uint16_t>(inValue) & static_cast<std::uint16_t>(inFlag)) != 0;
            }

            inline float parseNumber(const String& inValue, float inFallback)
            {
                const String value = inValue.trim();
                if (value.isEmpty())
                {
                    return inFallback;
                }

                char*       end    = nullptr;
                const float parsed = static_cast<float>(std::strtod(value.toChar(), &end));
                if (!end || end == value.toChar())
                {
                    return inFallback;
                }

                return parsed;
            }

            inline float stepSize(float inStep, float inMin, float inMax)
            {
                if (inStep > 0.0f)
                {
                    return inStep;
                }

                return std::max(0.01f, (inMax - inMin) * 0.01f);
            }

            inline float modifierScale(Input::KeyboardButtonModifier inModifier)
            {
                if (hasModifier(inModifier, Input::KeyboardButtonModifier::Ctrl))
                {
                    return 10.0f;
                }

                if (hasModifier(inModifier, Input::KeyboardButtonModifier::Shift))
                {
                    return 0.1f;
                }

                return 1.0f;
            }

            inline int wheelSteps(const Vec2& inDelta)
            {
                const float primary = std::fabs(inDelta.y) >= std::fabs(inDelta.x) ? inDelta.y : inDelta.x;
                if (std::fabs(primary) <= 0.0001f)
                {
                    return 0;
                }

                const int steps = static_cast<int>(std::round(primary));

                return steps != 0 ? steps : (primary > 0.0f ? 1 : -1);
            }

            inline float clampValue(float inValue, float inMin, float inMax)
            {
                return std::clamp(inValue, inMin, inMax);
            }

            inline float snapValue(float inValue, float inMin, float inMax, float inStep)
            {
                const float clamped = clampValue(inValue, inMin, inMax);
                if (inStep <= 0.0f)
                {
                    return clamped;
                }

                const float origin  = clamped - inMin;
                const float snapped = inMin + std::round(origin / inStep) * inStep;

                return clampValue(snapped, inMin, inMax);
            }

            inline float percent(float inValue, float inMin, float inMax)
            {
                const float span = inMax - inMin;
                if (span <= 0.0001f)
                {
                    return 0.0f;
                }

                return std::clamp(((inValue - inMin) / span) * 100.0f, 0.0f, 100.0f);
            }

            inline float fromLocation(
                const Vec2& inLocation, const Bounds2D& inBox, float inMin, float inMax, float inStep
            )
            {
                const float width = inBox.right - inBox.left;
                if (width <= 0.0f)
                {
                    return inMin;
                }

                const float t = std::clamp((inLocation.x - inBox.left) / width, 0.0f, 1.0f);

                return snapValue(inMin + t * (inMax - inMin), inMin, inMax, inStep);
            }

            inline String formatValue(float inValue, float inStep, int inPrecision)
            {
                if (inStep >= 1.0f && std::fabs(inStep - std::round(inStep)) <= 0.0001f)
                {
                    return String::sprint("%d", static_cast<int>(std::round(inValue)));
                }

                return String::sprint("%.*f", inPrecision, inValue);
            }

            inline Component* findChildId(const Component* inRoot, const String& inId)
            {
                if (!inRoot || inId.isEmpty())
                {
                    return nullptr;
                }

                if (inRoot->getId().equals(inId))
                {
                    return const_cast<Component*>(inRoot);
                }

                for (Component* child : inRoot->getChildrenFlat())
                {
                    if (child && child->getId().equals(inId))
                    {
                        return child;
                    }
                }

                return nullptr;
            }

            inline bool isDescendantId(const Component* inRoot, const Component* inHit, const String& inId)
            {
                if (!inRoot || !inHit || inId.isEmpty())
                {
                    return false;
                }

                for (const Component* node = inHit; node != nullptr; node = node->getParent())
                {
                    if (node->getId().equals(inId))
                    {
                        return true;
                    }

                    if (node == inRoot || node->isRoot())
                    {
                        break;
                    }
                }

                return false;
            }

            inline void commitFloat(Component* inComponent, const String& inAttribute, float inValue)
            {
                if (!inComponent)
                {
                    return;
                }

                const String raw = inComponent->getAttribute(inAttribute);
                if (raw.firstOf(REFERENCE_VALUE_OPENING) == String::npos ||
                    raw.lastOf(REFERENCE_VALUE_CLOSING) == String::npos || !inComponent->hasParent())
                {
                    return;
                }

                const String id = raw.getBetween(REFERENCE_VALUE_OPENING, REFERENCE_VALUE_CLOSING).trim();
                if (id.isEmpty())
                {
                    return;
                }

                for (Component* node = inComponent->getParent(); node != nullptr; node = node->getParent())
                {
                    const ReflectionFieldAccessor accessor = node->getField(id);
                    if (accessor.isValid() && accessor.isType<float>())
                    {
                        void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                           : static_cast<void*>(node);

                        if (float* target = accessor.getValue<float>(instance))
                        {
                            *target = inValue;
                        }

                        return;
                    }

                    if (node->isRoot())
                    {
                        return;
                    }
                }
            }
        }
    }
}
