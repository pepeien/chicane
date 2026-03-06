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
        std::size_t                    offset    = 0;
        std::size_t                    size      = 0;
        String                         name      = "";
        String                         typeName  = "";
        std::optional<std::type_index> typeIndex = std::nullopt;

        // ── Type check ───────────────────────────────────────────
        template <typename T>
        bool isType() const
        {
            if (!typeIndex.has_value())
            {
                return false;
            }
            return typeIndex.value() == std::type_index(typeid(T));
        }

        // ── Read typed pointer (returns nullptr if wrong type) ───
        template <typename T>
        const T* getValue(const void* instance) const
        {
            if (!isType<T>())
            {
                return nullptr;
            }
            return reinterpret_cast<const T*>(static_cast<const char*>(instance) + offset);
        }

        template <typename T>
        T* getValue(void* instance) const
        {
            if (!isType<T>())
            {
                return nullptr;
            }
            return reinterpret_cast<T*>(static_cast<char*>(instance) + offset);
        }

        // ── Read (throws on size mismatch) ───────────────────────
        template <typename T>
        const T& get(const void* instance) const
        {
            assertSize<T>();
            return *reinterpret_cast<const T*>(static_cast<const char*>(instance) + offset);
        }

        // ── Write (throws on size mismatch) ──────────────────────
        template <typename T>
        void set(void* instance, const T& value) const
        {
            assertSize<T>();
            *reinterpret_cast<T*>(static_cast<char*>(instance) + offset) = value;
        }

        // ── Raw pointer ──────────────────────────────────────────
        void* ptr(void* instance) const { return static_cast<char*>(instance) + offset; }

        const void* ptr(const void* instance) const { return static_cast<const char*>(instance) + offset; }

        // ── toString — converts primitive field value to string ──
        String toString(const void* instance) const
        {
            if (isType<String>())
            {
                const String* v = getValue<String>(instance);

                return v ? *v : "";
            }

            if (isType<std::string>())
            {
                const std::string* v = getValue<std::string>(instance);

                return v ? *v : "";
            }

            if (isType<char>())
            {
                const char* v = getValue<char>(instance);

                return v ? String(1, *v) : "";
            }

            if (isType<bool>())
            {
                const bool* v = getValue<bool>(instance);

                return v ? (*v ? "true" : "false") : "";
            }

            if (isType<int>())
            {
                const int* v = getValue<int>(instance);

                return v ? std::to_string(*v) : "";
            }

            if (isType<long>())
            {
                const long* v = getValue<long>(instance);

                return v ? std::to_string(*v) : "";
            }

            if (isType<float>())
            {
                const float* v = getValue<float>(instance);

                return v ? std::to_string(*v) : "";
            }

            if (isType<double>())
            {
                const double* v = getValue<double>(instance);

                return v ? std::to_string(*v) : "";
            }

            if (isType<std::uint64_t>())
            {
                const std::uint64_t* v = getValue<std::uint64_t>(instance);

                return v ? std::to_string(*v) : "";
            }

            if (isType<std::uint32_t>())
            {
                const std::uint32_t* v = getValue<std::uint32_t>(instance);

                return v ? std::to_string(*v) : "";
            }

            if (isType<std::uint16_t>())
            {
                const std::uint16_t* v = getValue<std::uint16_t>(instance);

                return v ? std::to_string(*v) : "";
            }

            if (isType<std::uint8_t>())
            {
                const std::uint8_t* v = getValue<std::uint8_t>(instance);

                return v ? std::to_string(*v) : "";
            }

            return "<" + typeName + ">";
        }

    private:
        template <typename T>
        void assertSize() const
        {
            if (sizeof(T) != size)
            {
                std::ostringstream oss;
                oss << "FieldAccessor — size mismatch on field '" << name << "': field is " << size
                    << " bytes, requested type is " << sizeof(T) << " bytes";
                throw std::runtime_error(oss.str());
            }
        }
    };
}