#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Item.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE Dialog
        {
        public:
            using Callback = std::function<void(const Item::List&)>;

        public:
            virtual void open(const Callback& inCallback) const = 0;

        public:
            bool   bCanSelectMany = false;

            String title    = "";
            String location = "";
        };
    }
}