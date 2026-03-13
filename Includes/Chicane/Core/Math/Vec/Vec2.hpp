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
        template <typename... A>
        inline constexpr Vec2(A... args)
        {
            glm::vec2 value(std::forward<A>(args)...);

            x = value.x;
            y = value.y;
        }

    public:
        inline static constexpr Vec2 Zero() { return Vec2(0.0f); }

        inline static constexpr Vec2 One() { return Vec2(1.0f); }

        inline static constexpr Vec2 Right() { return Vec2(1.0f, 0.0f); }

        inline static constexpr Vec2 Up() { return Vec2(0.0f, 1.0f); }

    public:
        // Conversion
        inline operator String() const { return toString(); }

        inline operator glm::vec2() const { return {x, y}; }

        // Comparassion
        friend inline bool operator==(Vec2 inLeft, Vec2 inRight)
        {
            return glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(
                       inLeft.x,
                       inRight.x
                   ) &&
                   glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(inLeft.y, inRight.y);
        }

        // Addition
        inline Vec2& operator+=(Vec2 inValue)
        {
            x += static_cast<float>(inValue.x);
            y += static_cast<float>(inValue.y);

            return *this;
        }

        template <typename T>
        inline Vec2 operator+(T inScalar)
        {
            x += static_cast<float>(inScalar);
            y += static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec2 operator+(Vec2 inLeft, Vec2 inRight)
        {
            return Vec2(inLeft.x + inRight.x, inLeft.y + inRight.y);
        }

        template <typename T>
        friend inline Vec2 operator+(Vec2 inValue, T inScalar)
        {
            return Vec2(inValue.x + inScalar, inValue.y + inScalar);
        }

        template <typename T>
        friend inline Vec2 operator+(T inScalar, Vec2 inValue)
        {
            return Vec2(inValue.x + inScalar, inValue.y + inScalar);
        }

        // Substraction
        inline Vec2& operator-=(Vec2 inValue)
        {
            x -= static_cast<float>(inValue.x);
            y -= static_cast<float>(inValue.y);

            return *this;
        }

        template <typename T>
        inline Vec2 operator-(T inScalar)
        {
            x -= static_cast<float>(inScalar);
            y -= static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec2 operator-(Vec2 inLeft, const Vec2 inRight)
        {
            return Vec2(inLeft.x - inRight.x, inLeft.y - inRight.y);
        }

        template <typename T>
        friend inline Vec2 operator-(Vec2 inValue, T inScalar)
        {
            return Vec2(inValue.x - inScalar, inValue.y - inScalar);
        }

        template <typename T>
        friend inline Vec2 operator-(T inScalar, Vec2 inValue)
        {
            return Vec2(inValue.x - inScalar, inValue.y - inScalar);
        }

        // Multiplication
        inline Vec2& operator*=(Vec2 inValue)
        {
            x *= static_cast<float>(inValue.x);
            y *= static_cast<float>(inValue.y);

            return *this;
        }

        template <typename T>
        inline Vec2& operator*=(T inScalar)
        {
            x *= static_cast<float>(inScalar);
            y *= static_cast<float>(inScalar);

            return *this;
        }

        template <typename T>
        inline Vec2 operator*(T inScalar)
        {
            x *= static_cast<float>(inScalar);
            y *= static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec2 operator*(Vec2 inLeft, Vec2 inRight)
        {
            return Vec2(inLeft.x * inRight.x, inLeft.y * inRight.y);
        }

        template <typename T>
        friend inline Vec2 operator*(Vec2 inValue, T inScalar)
        {
            return Vec2(inValue.x * inScalar, inValue.y * inScalar);
        }

        template <typename T>
        friend inline Vec2 operator*(T inScalar, Vec2 inValue)
        {
            return Vec2(inValue.x * inScalar, inValue.y * inScalar);
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