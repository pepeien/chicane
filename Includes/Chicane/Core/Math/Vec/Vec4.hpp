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
        static const Vec4 Zero;
        static const Vec4 One;

        static const Vec4 Up;
        static const Vec4 Right;
        static const Vec4 Forward;

    public:
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

        template <typename... A>
        constexpr Vec4(A... args)
            : x(0.0f),
              y(0.0f),
              z(0.0f),
              w(0.0f)
        {
            glm::vec4 value(std::forward<A>(args)...);

            x = value.x;
            y = value.y;
            z = value.z;
            w = value.w;
        }

    public:
        // Conversion
        inline operator String() const { return toString(); }

        inline operator glm::vec4() const { return {x, y, z, w}; }

        // Comparassion
        friend inline bool operator==(Vec4 inLeft, Vec4 inRight)
        {
            return glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(
                       inLeft.x,
                       inRight.x
                   ) &&
                   glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(
                       inLeft.y,
                       inRight.y
                   ) &&
                   glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(
                       inLeft.z,
                       inRight.z
                   ) &&
                   glm::detail::compute_equal<float, std::numeric_limits<float>::is_iec559>::call(inLeft.w, inRight.w);
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