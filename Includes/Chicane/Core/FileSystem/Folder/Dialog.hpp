#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Dialog.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE FolderDialog : Dialog
        {
        public:
            FolderDialog();

        public:
            void open(const Callback& inCallback) const override;
        };
    }
}