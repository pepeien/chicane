#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <typeindex>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionFieldIterable
    {
    public:
        using TypeIndex    = std::optional<std::type_index>;
        using SizeFunction = std::function<std::size_t(const void*)>;
        using AtFunction   = std::function<const void*(const void*, std::size_t)>;

    public:
        ReflectionFieldIterable(
            String       inElementTypeName,
            TypeIndex    inElementIndex,
            std::size_t  inElementSize,
            SizeFunction inSizeFunction,
            AtFunction   inAtFunction
        );
        ReflectionFieldIterable();

    public:
        String       elementTypeName;
        TypeIndex    elementIndex;
        std::size_t  elementSize;
        SizeFunction sizeFunction;
        AtFunction   atFunction;
    };
}
