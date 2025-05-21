#include "Chicane/Core/FileSystem/FolderDialog.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        FolderDialog::FolderDialog()
            : title(""),
            location(""),
            bCanSelectMany(false)
        {}

        void FolderDialog::open(Callback inCallback) const
        {

        }
    }
}