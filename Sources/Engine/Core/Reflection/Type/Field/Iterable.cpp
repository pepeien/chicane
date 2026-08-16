#include "Chicane/Core/Reflection/Type/Field/Iterable.hpp"

namespace Chicane
{
    ReflectionFieldIterable::ReflectionFieldIterable(
        String       inElementTypeName,
        TypeIndex    inElementIndex,
        std::size_t  inElementSize,
        SizeFunction inSizeFunction,
        AtFunction   inAtFunction
    )
        : elementTypeName(std::move(inElementTypeName)),
          elementIndex(std::move(inElementIndex)),
          elementSize(inElementSize),
          sizeFunction(std::move(inSizeFunction)),
          atFunction(std::move(inAtFunction))
    {}

    ReflectionFieldIterable::ReflectionFieldIterable()
        : elementTypeName(""),
          elementIndex(std::nullopt),
          elementSize(0),
          sizeFunction(nullptr),
          atFunction(nullptr)
    {}
}
