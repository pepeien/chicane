#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    CH_TYPE(Automatic)
    struct CHICANE_CORE Vec4
    {
    public:
        inline static constexpr Vec4 sZero() { return Vec4(0.0f); }

        inline static constexpr Vec4 sOne() { return Vec4(1.0f); }

        inline static constexpr Vec4 sRight() { return Vec4(Vec3::sRight(), 0.0f); }

        inline static constexpr Vec4 sForward() { return Vec4(Vec3::sForward(), 0.0f); }

        inline static constexpr Vec4 sUp() { return Vec4(Vec3::sUp(), 0.0f); }

        inline static constexpr Vec4 sSentinel() { return Vec4(-1.0e9f, -1.0e9f, 1.0e9f, 1.0e9f); }

    public:
        constexpr Vec4()
            : x(0.0f),
              y(0.0f),
              z(0.0f),
              w(0.0f)
        {}

        constexpr Vec4(float inValue)
            : x(inValue),
              y(inValue),
              z(inValue),
              w(inValue)
        {}

        constexpr Vec4(float inX, float inY, float inZ, float inW)
            : x(inX),
              y(inY),
              z(inZ),
              w(inW)
        {}

        constexpr Vec4(const Vec2& inValue)
            : x(inValue.x),
              y(inValue.y),
              z(0.0f),
              w(0.0f)
        {}

        constexpr Vec4(const Vec3& inValue, float inW = 0.0f)
            : x(inValue.x),
              y(inValue.y),
              z(inValue.z),
              w(inW)
        {}

        template <typename T, glm::qualifier Q>
        constexpr Vec4(const glm::vec<4, T, Q>& inValue)
            : x(static_cast<float>(inValue.x)),
              y(static_cast<float>(inValue.y)),
              z(static_cast<float>(inValue.z)),
              w(static_cast<float>(inValue.w))
        {}

    public:
        // Conversion
        inline operator String() const { return toString(); }

        inline operator glm::vec4() const { return {x, y, z, w}; }

        // Comparassion
        friend inline bool operator==(Vec4 inLeft, Vec4 inRight)
        {
            return inLeft.x == inRight.x && inLeft.y == inRight.y && inLeft.z == inRight.z && inLeft.w == inRight.w;
        }

        // Addition
        inline Vec4& operator+=(Vec4 inValue)
        {
            x += static_cast<float>(inValue.x);
            y += static_cast<float>(inValue.y);
            z += static_cast<float>(inValue.z);
            w += static_cast<float>(inValue.w);

            return *this;
        }

        template <typename T>
        inline Vec4 operator+(T inScalar)
        {
            x += static_cast<float>(inScalar);
            y += static_cast<float>(inScalar);
            z += static_cast<float>(inScalar);
            w += static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec4 operator+(Vec4 inLeft, Vec4 inRight)
        {
            return Vec4(inLeft.x + inRight.x, inLeft.y + inRight.y, inLeft.z + inRight.z, inLeft.w + inRight.w);
        }

        template <typename T>
        friend inline Vec4 operator+(Vec4 inValue, T inScalar)
        {
            return Vec4(inValue.x + inScalar, inValue.y + inScalar, inValue.z + inScalar, inValue.w + inScalar);
        }

        template <typename T>
        friend inline Vec4 operator+(T inScalar, Vec4 inValue)
        {
            return Vec4(inValue.x + inScalar, inValue.y + inScalar, inValue.z + inScalar, inValue.w + inScalar);
        }

        // Substraction
        inline Vec4& operator-=(Vec4 inValue)
        {
            x -= static_cast<float>(inValue.x);
            y -= static_cast<float>(inValue.y);
            z -= static_cast<float>(inValue.z);
            w -= static_cast<float>(inValue.w);

            return *this;
        }

        template <typename T>
        inline Vec4 operator-(T inScalar)
        {
            x -= static_cast<float>(inScalar);
            y -= static_cast<float>(inScalar);
            z -= static_cast<float>(inScalar);
            w -= static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec4 operator-(Vec4 inLeft, const Vec4 inRight)
        {
            return Vec4(inLeft.x - inRight.x, inLeft.y - inRight.y, inLeft.z - inRight.z, inLeft.w - inRight.w);
        }

        template <typename T>
        friend inline Vec4 operator-(Vec4 inValue, T inScalar)
        {
            return Vec4(inValue.x - inScalar, inValue.y - inScalar, inValue.z - inScalar, inValue.w - inScalar);
        }

        template <typename T>
        friend inline Vec4 operator-(T inScalar, Vec4 inValue)
        {
            return Vec4(inValue.x - inScalar, inValue.y - inScalar, inValue.z - inScalar, inValue.w - inScalar);
        }

        // Multiplication
        inline Vec4& operator*=(Vec4 inValue)
        {
            x *= static_cast<float>(inValue.x);
            y *= static_cast<float>(inValue.y);
            z *= static_cast<float>(inValue.z);
            w *= static_cast<float>(inValue.w);

            return *this;
        }

        template <typename T>
        inline Vec4& operator*=(T inScalar)
        {
            x *= static_cast<float>(inScalar);
            y *= static_cast<float>(inScalar);
            z *= static_cast<float>(inScalar);
            w *= static_cast<float>(inScalar);

            return *this;
        }

        template <typename T>
        inline Vec4 operator*(T inScalar)
        {
            x *= static_cast<float>(inScalar);
            y *= static_cast<float>(inScalar);
            z *= static_cast<float>(inScalar);
            w *= static_cast<float>(inScalar);

            return *this;
        }

        friend inline Vec4 operator*(Vec4 inLeft, Vec4 inRight)
        {
            return Vec4(inLeft.x * inRight.x, inLeft.y * inRight.y, inLeft.z * inRight.z, inLeft.w * inRight.w);
        }

        template <typename T>
        friend inline Vec4 operator*(Vec4 inValue, T inScalar)
        {
            return Vec4(inValue.x * inScalar, inValue.y * inScalar, inValue.z * inScalar, inValue.w * inScalar);
        }

        template <typename T>
        friend inline Vec4 operator*(T inScalar, Vec4 inValue)
        {
            return Vec4(inValue.x * inScalar, inValue.y * inScalar, inValue.z * inScalar, inValue.w * inScalar);
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
        union
        {
            float z, b, p;
        };
        union
        {
            float w, a, q;
        };
    };
}