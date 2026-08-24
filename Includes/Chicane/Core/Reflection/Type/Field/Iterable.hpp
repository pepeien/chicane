#pragma once

#include <any>
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
        using TypeIndex        = std::optional<std::type_index>;
        using SizeFunction     = std::function<std::size_t(const void*)>;
        using AtFunction       = std::function<const void*(const void*, std::size_t)>;
        using SnapshotFunction = std::function<const void*(const void*, std::any&)>;

    public:
        ReflectionFieldIterable(
            String           inElementTypeName,
            TypeIndex        inElementIndex,
            std::size_t      inElementSize,
            SizeFunction     inSizeFunction,
            AtFunction       inAtFunction,
            SnapshotFunction inSnapshotFunction = {}
        );
        ReflectionFieldIterable();

    public:
        String           elementTypeName;
        TypeIndex        elementIndex;
        std::size_t      elementSize;
        SizeFunction     sizeFunction;
        AtFunction       atFunction;
        SnapshotFunction snapshotFunction;
    };
}
