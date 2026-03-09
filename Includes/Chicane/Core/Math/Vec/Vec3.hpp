#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct Vec4;

    CH_TYPE(Automatic)
    struct CHICANE_CORE Vec3
    {
    public:
        static const Vec3 Zero;
        static const Vec3 One;

        static const Vec3 Right;
        static const Vec3 Forward;
        static const Vec3 Up;

    public:
        template <typename... A>
        constexpr Vec3(A... args)
            : x(0.0f),
              y(0.0f),
              z(0.0f)
        {
            glm::vec3 value(std::forward<A>(args)...);

            x = value.x;
            y = value.y;
            z = value.z;
        }

        constexpr Vec3(const Vec2& inValue)
            : x(inValue.x),
              y(inValue.y),
              z(0.0f)
        {}

    public:
        Vec3(const Vec4& inValue);

    public:
        // Conversion
        inline operator String() const { return toString(); }

        inline operator glm::vec3() const { return {x, y, z}; }

        // Comparassion
        friend inline bool operator==(Vec3 inLeft, Vec3 inRight)
        {
            return glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(
                       inLeft.x,
                       inRight.x
                   ) &&
                   glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(
                       inLeft.y,
                       inRight.y
                   ) &&
                   glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(inLeft.z, inRight.z);
        }

        // Addition
        inline Vec3& operator+=(Vec3 inValue)
        {
            x += inValue.x;
            y += inValue.y;
            z += inValue.z;
            return *this;
        }

        template <typename T>
        inline Vec3& operator+=(T inScalar)
        {
            x += static_cast<float>(inScalar);
            y += static_cast<float>(inScalar);
            z += static_cast<float>(inScalar);
            return *this;
        }

        friend inline Vec3 operator+(Vec3 inLeft, Vec3 inRight) { return inLeft += inRight; }

        template <typename T>
        friend inline Vec3 operator+(Vec3 inValue, T inScalar)
        {
            return inValue += inScalar;
        }

        template <typename T>
        friend inline Vec3 operator+(T inScalar, Vec3 inValue)
        {
            return inValue += inScalar;
        }

        // Subtraction
        inline Vec3& operator-=(Vec3 inValue)
        {
            x -= inValue.x;
            y -= inValue.y;
            z -= inValue.z;
            return *this;
        }

        template <typename T>
        inline Vec3& operator-=(T inScalar)
        {
            x -= static_cast<float>(inScalar);
            y -= static_cast<float>(inScalar);
            z -= static_cast<float>(inScalar);
            return *this;
        }

        friend inline Vec3 operator-(Vec3 inLeft, Vec3 inRight) { return inLeft -= inRight; }

        template <typename T>
        friend inline Vec3 operator-(Vec3 inValue, T inScalar)
        {
            return inValue -= inScalar;
        }

        template <typename T>
        friend inline Vec3 operator-(T inScalar, Vec3 inValue)
        {
            return inValue -= inScalar;
        }

        // Multiplication
        inline Vec3& operator*=(Vec3 inValue)
        {
            x *= inValue.x;
            y *= inValue.y;
            z *= inValue.z;
            return *this;
        }

        template <typename T>
        inline Vec3& operator*=(T inScalar)
        {
            x *= static_cast<float>(inScalar);
            y *= static_cast<float>(inScalar);
            z *= static_cast<float>(inScalar);
            return *this;
        }

        friend inline Vec3 operator*(Vec3 inLeft, Vec3 inRight) { return inLeft *= inRight; }

        template <typename T>
        friend inline Vec3 operator*(Vec3 inValue, T inScalar)
        {
            return inValue *= inScalar;
        }

        template <typename T>
        friend inline Vec3 operator*(T inScalar, Vec3 inValue)
        {
            return inValue *= inScalar;
        }

        // Division
        inline Vec3& operator/=(Vec3 inValue)
        {
            x /= inValue.x;
            y /= inValue.y;
            z /= inValue.z;

            return *this;
        }

        template <typename T>
        inline Vec3& operator/=(T inScalar)
        {
            x /= static_cast<float>(inScalar);
            y /= static_cast<float>(inScalar);
            z /= static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec3 operator/(Vec3 inLeft, Vec3 inRight) { return inLeft /= inRight; }

        template <typename T>
        friend inline Vec3 operator/(Vec3 inValue, T inScalar)
        {
            return inValue /= inScalar;
        }

        template <typename T>
        friend inline Vec3 operator/(T inScalar, Vec3 inValue)
        {
            return inValue /= inScalar;
        }

    public:
        String toString() const;

        Vec3 min(const Vec3& inValue) const;
        Vec3 max(const Vec3& inValue) const;
        Vec3 cross(const Vec3& inValue) const;

        float dot(const Vec3& inValue) const;

        Vec3 normalize() const;

    public:
        union
        {
            float x, r, s;
        };
        union
        {
            float y, g, t;
        };
        union
        {
            float z, b, p;
        };
    };
}