#pragma once

#include <type_traits>

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
        inline static constexpr Vec3 sZero() { return Vec3(0.0f); }

        inline static constexpr Vec3 sOne() { return Vec3(1.0f); }

        inline static constexpr Vec3 sRight() { return Vec3(1.0f, 0.0f, 0.0f); }

        inline static constexpr Vec3 sForward() { return Vec3(0.0f, 1.0f, 0.0f); }

        inline static constexpr Vec3 sUp() { return Vec3(0.0f, 0.0f, 1.0f); }

    public:
        inline constexpr Vec3()
            : x(0.0f),
              y(0.0f),
              z(0.0f)
        {}

        inline constexpr Vec3(float inValue)
            : x(inValue),
              y(inValue),
              z(inValue)
        {}

        inline constexpr Vec3(float inX, float inY, float inZ)
            : x(inX),
              y(inY),
              z(inZ)
        {}

        inline constexpr Vec3(const Vec2& inValue, float inZ = 0.0f)
            : x(inValue.x),
              y(inValue.y),
              z(inZ)
        {}

        constexpr Vec3(const Vec4& inValue);

        template <typename T, glm::qualifier Q>
        inline constexpr Vec3(const glm::vec<3, T, Q>& inValue)
            : x(static_cast<float>(inValue.x)),
              y(static_cast<float>(inValue.y)),
              z(static_cast<float>(inValue.z))
        {}

        template <typename T, glm::qualifier Q>
        inline constexpr Vec3(const glm::vec<4, T, Q>& inValue)
            : x(static_cast<float>(inValue.x)),
              y(static_cast<float>(inValue.y)),
              z(static_cast<float>(inValue.z))
        {}

    public:
        // Conversion
        inline operator String() const { return toString(); }

        inline operator glm::vec3() const { return {x, y, z}; }

        // Comparassion
        friend inline bool operator==(Vec3 inLeft, Vec3 inRight)
        {
            return inLeft.x == inRight.x && inLeft.y == inRight.y && inLeft.z == inRight.z;
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
        friend inline std::enable_if_t<std::is_arithmetic_v<T>, Vec3> operator/(Vec3 inValue, T inScalar)
        {
            return inValue /= inScalar;
        }

        template <typename T>
        friend inline std::enable_if_t<std::is_arithmetic_v<T>, Vec3> operator/(T inScalar, Vec3 inValue)
        {
            return inValue /= inScalar;
        }

    public:
        String toString() const;

        inline Vec3 min(const Vec3& inValue) const
        {
            return Vec3(
                x < inValue.x ? x : inValue.x, y < inValue.y ? y : inValue.y, z < inValue.z ? z : inValue.z
            );
        }

        inline Vec3 max(const Vec3& inValue) const
        {
            return Vec3(
                x > inValue.x ? x : inValue.x, y > inValue.y ? y : inValue.y, z > inValue.z ? z : inValue.z
            );
        }

        inline Vec3 cross(const Vec3& inValue) const
        {
            return Vec3(
                (y * inValue.z) - (z * inValue.y),
                (z * inValue.x) - (x * inValue.z),
                (x * inValue.y) - (y * inValue.x)
            );
        }

        inline float dot(const Vec3& inValue) const { return (x * inValue.x) + (y * inValue.y) + (z * inValue.z); }

        inline float length() const { return std::sqrt(dot(*this)); }

        inline Vec3 normalize() const
        {
            const float length = std::sqrt(dot(*this));

            if (length <= 0.0f)
            {
                return Vec3(0.0f);
            }

            return *this / length;
        }

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