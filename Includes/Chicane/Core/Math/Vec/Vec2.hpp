#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    CH_TYPE(Automatic)
    struct CHICANE_CORE Vec2
    {
    public:
        inline static constexpr Vec2 sZero() { return Vec2(0.0f); }

        inline static constexpr Vec2 sOne() { return Vec2(1.0f); }

        inline static constexpr Vec2 sRight() { return Vec2(1.0f, 0.0f); }

        inline static constexpr Vec2 sUp() { return Vec2(0.0f, 1.0f); }

    public:
        inline constexpr Vec2()
            : x(0.0f),
              y(0.0f)
        {}

        inline constexpr Vec2(float inValue)
            : x(inValue),
              y(inValue)
        {}

        inline constexpr Vec2(float inX, float inY)
            : x(inX),
              y(inY)
        {}

        template <typename T, glm::qualifier Q>
        inline constexpr Vec2(const glm::vec<2, T, Q>& inValue)
            : x(static_cast<float>(inValue.x)),
              y(static_cast<float>(inValue.y))
        {}

    public:
        // Conversion
        inline operator String() const { return toString(); }

        inline operator glm::vec2() const { return {x, y}; }

        // Comparassion
        friend inline bool operator==(Vec2 inLeft, Vec2 inRight)
        {
            return inLeft.x == inRight.x && inLeft.y == inRight.y;
        }

        // Addition
        inline Vec2& operator+=(Vec2 inValue)
        {
            x += inValue.x;
            y += inValue.y;

            return *this;
        }

        template <typename T>
        inline Vec2& operator+=(T inScalar)
        {
            x += static_cast<float>(inScalar);
            y += static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec2 operator+(Vec2 inLeft, Vec2 inRight) { return inLeft += inRight; }

        template <typename T>
        friend inline Vec2 operator+(Vec2 inValue, T inScalar)
        {
            return inValue += inScalar;
        }

        template <typename T>
        friend inline Vec2 operator+(T inScalar, Vec2 inValue)
        {
            return inValue += inScalar;
        }

        // Substraction
        inline Vec2& operator-=(Vec2 inValue)
        {
            x -= inValue.x;
            y -= inValue.y;

            return *this;
        }

        template <typename T>
        inline Vec2& operator-=(T inScalar)
        {
            x -= static_cast<float>(inScalar);
            y -= static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec2 operator-(Vec2 inLeft, Vec2 inRight) { return inLeft -= inRight; }

        template <typename T>
        friend inline Vec2 operator-(Vec2 inValue, T inScalar)
        {
            return inValue -= inScalar;
        }

        template <typename T>
        friend inline Vec2 operator-(T inScalar, Vec2 inValue)
        {
            return inValue -= inScalar;
        }

        // Multiplication
        inline Vec2& operator*=(Vec2 inValue)
        {
            x *= inValue.x;
            y *= inValue.y;

            return *this;
        }

        template <typename T>
        inline Vec2& operator*=(T inScalar)
        {
            x *= static_cast<float>(inScalar);
            y *= static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec2 operator*(Vec2 inLeft, Vec2 inRight) { return inLeft *= inRight; }

        template <typename T>
        friend inline Vec2 operator*(Vec2 inValue, T inScalar)
        {
            return inValue *= inScalar;
        }

        template <typename T>
        friend inline Vec2 operator*(T inScalar, Vec2 inValue)
        {
            return inValue *= inScalar;
        }

    public:
        String toString() const;

    public:
        union
        {
            float x, r, s;
        };
        union
        {
            float y, g, t;
        };
    };
}