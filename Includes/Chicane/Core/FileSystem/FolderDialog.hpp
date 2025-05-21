#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE FolderDialog
        {
        public:
            using Callback = SDL_DialogFileCallback;

        public:
            FolderDialog();

        public:
            virtual void open(Callback inCallback) const;

        public:
            bool        bCanSelectMany;

            std::string title;
            std::string location;
        };
    }
}