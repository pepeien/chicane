#include "Chicane/Core/Reflection/Type/Field/Iterable.hpp"

namespace Chicane
{
    ReflectionFieldIterable::ReflectionFieldIterable(
        String           inElementTypeName,
        TypeIndex        inElementIndex,
        std::size_t      inElementSize,
        SizeFunction     inSizeFunction,
        AtFunction       inAtFunction,
        SnapshotFunction inSnapshotFunction
    )
        : elementTypeName(std::move(inElementTypeName)),
          elementIndex(std::move(inElementIndex)),
          elementSize(inElementSize),
          sizeFunction(std::move(inSizeFunction)),
          atFunction(std::move(inAtFunction)),
          snapshotFunction(std::move(inSnapshotFunction))
    {}

    ReflectionFieldIterable::ReflectionFieldIterable()
        : elementTypeName(""),
          elementIndex(std::nullopt),
          elementSize(0),
          sizeFunction(nullptr),
          atFunction(nullptr),
          snapshotFunction(nullptr)
    {}
}
