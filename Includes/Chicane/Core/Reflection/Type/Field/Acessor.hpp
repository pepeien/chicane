#pragma once

#include <cstddef>
#include <optional>
#include <typeindex>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Type/Field/Info.hpp"
#include "Chicane/Core/Reflection/Type/Field/Iterable.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionFieldAccessor
    {
    public:
        ReflectionFieldAccessor(
            std::size_t                       inOffset,
            std::size_t                       inPtrOffset,
            std::size_t                       inSize,
            const ReflectionFieldInfo::Names& inNames,
            const String&                     inTypeName,
            ReflectionFieldInfo::TypeIndex    inTypeIndex,
            bool                              bInNeedsDeref,
            bool                              bInIsIterable,
            ReflectionFieldInfo::TypeIndex    inElementIndex,
            ReflectionFieldIterable           inIterable,
            const void*                       inBoundInstance
        );
        ReflectionFieldAccessor();

    public:
        bool isValid() const;

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
                throw std::runtime_error("ReflectionFieldAccessor::get — null pointer on field [" + getName() + "]");
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
                throw std::runtime_error("ReflectionFieldAccessor::set — null pointer on field [" + getName() + "]");
            }

            *reinterpret_cast<T*>(addr) = inValue;
        }

        const char* address(const void* inInstance) const;
        char* address(void* inInstance) const;

        void* ptr(void* inInstance) const;
        const void* ptr(const void* inInstance) const;

        String toString(const void* inInstance) const;

        const String& getName() const;

        std::size_t getSize(const void* inInstance) const;
        ReflectionFieldAccessor getElement(const void* inInstance, std::size_t inIndex) const;
        ReflectionFieldAccessor bind(const void* inInstance) const;

    private:
        template <typename T>
        void assertSize() const
        {
            if (sizeof(T) != size)
            {
                throw std::runtime_error(
                    "FieldAccessor — size mismatch on field [" + getName() + "]: field is [" + size +
                    "] bytes, requested type is [" + sizeof(T) + "] bytes"
                );
            }
        }

        const void* containerPtr(const void* inInstance) const;

    public:
        std::size_t                    offset;
        std::size_t                    ptrOffset;
        std::size_t                    size;
        ReflectionFieldInfo::Names     names;
        String                         typeName;
        ReflectionFieldInfo::TypeIndex typeIndex;
        ReflectionFieldInfo::TypeIndex elementIndex;
        bool                           bNeedsDeref;
        bool                           bIsIterable;
        ReflectionFieldIterable        iterable;
        const void*                    boundInstance;
    };
}
