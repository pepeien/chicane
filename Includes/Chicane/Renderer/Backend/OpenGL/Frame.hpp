#pragma once

#include <unordered_map>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLFrame
        {
        public:
            using Objects = std::unordered_map<String, std::uint32_t>;

        public:
            void addObject(const String& inId, std::uint32_t inObject);
            std::uint32_t getObject(const String& inId) const;
            void removeObject(const String& inId);

        public:
            Objects objects;
        };
    }
}
