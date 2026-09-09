#pragma once

#include <string>

#include <pugixml.hpp>

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Xml
    {
        struct CHICANE_CORE Writer : pugi::xml_writer
        {
            std::string data;

            void write(const void* inData, size_t inSize) override
            {
                data.append(static_cast<const char*>(inData), inSize);
            }
        };
    }
}
