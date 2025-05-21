#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem/FolderDialog.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE FileDialog : FolderDialog
        {
        public:
            FileDialog();

        public:
            void open(Callback inCallback) const override;
        };
    }
}