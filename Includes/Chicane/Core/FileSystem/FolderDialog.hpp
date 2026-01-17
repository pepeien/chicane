#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE FolderDialog
        {
        public:
            using Callback = std::function<void()>;

        public:
            FolderDialog();

        public:
            virtual void open(Callback inCallback) const;

        public:
            bool   bCanSelectMany;

            String title;
            String location;
        };
    }
}