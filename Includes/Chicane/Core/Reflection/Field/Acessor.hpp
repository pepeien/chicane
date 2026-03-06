#pragma once

#include <cstddef>
#include <optional>
#include <typeindex>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionFieldAccessor
    {
    public:
        template <typename T>
        bool isType() const
        {
            if (!typeIndex.has_value())
            {
                return false;
            }

            return typeIndex.value() == std::type_index(typeid(T));
        }

        template <typename T>
        const T* getValue(const void* inInstance) const
        {
            if (!isType<T>())
            {
                return nullptr;
            }

            const char* addr = address(inInstance);

            return addr ? reinterpret_cast<const T*>(addr) : nullptr;
        }

        template <typename T>
        T* getValue(void* inInstance) const
        {
            if (!isType<T>())
            {
                return nullptr;
            }

            char* addr = address(inInstance);

            return addr ? reinterpret_cast<T*>(addr) : nullptr;
        }

        template <typename T>
        const T& get(const void* inInstance) const
        {
            assertSize<T>();

            const char* addr = address(inInstance);
            if (!addr)
            {
                throw std::runtime_error("ReflectionFieldAccessor::get — null pointer on field [" + name + "]");
            }

            return *reinterpret_cast<const T*>(addr);
        }

        template <typename T>
        void set(void* inInstance, const T& inValue) const
        {
            assertSize<T>();

            char* addr = address(inInstance);
            if (!addr)
            {
                throw std::runtime_error("ReflectionFieldAccessor::set — null pointer on field [" + name + "]");
            }

            *reinterpret_cast<T*>(addr) = inValue;
        }

        const char* address(const void* inInstance) const;
        char* address(void* inInstance) const;

        void* ptr(void* inInstance) const;
        const void* ptr(const void* inInstance) const;

        String toString(const void* inInstance) const;

    private:
        template <typename T>
        void assertSize() const
        {
            if (sizeof(T) != size)
            {
                throw std::runtime_error(
                    "FieldAccessor — size mismatch on field [" + name + "]: field is [" + size +
                    "] bytes, requested type is [" + sizeof(T) + "] bytes"
                );
            }
        }

    public:
        std::size_t                    offset      = 0;
        std::size_t                    ptrOffset   = 0;
        std::size_t                    size        = 0;
        String                         name        = "";
        String                         typeName    = "";
        std::optional<std::type_index> typeIndex   = std::nullopt;
        bool                           bNeedsDeref = false;
    };
}