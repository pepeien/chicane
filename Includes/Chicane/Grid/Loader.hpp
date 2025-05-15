#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        CHICANE_GRID Component* createComponent(const pugi::xml_node& inNode);
        CHICANE_GRID void registerComponent(const std::string& inTag, Component::Compiler inCompiler);

        CHICANE_GRID bool hasActiveView();
        CHICANE_GRID View* getActiveView();
        CHICANE_GRID void activateView(const std::string& inPath);
        CHICANE_GRID Subscription<View*>* watchActiveView(
            std::function<void (View*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );
        CHICANE_GRID bool hasView(const std::string& inPath);
        CHICANE_GRID View* getView(const std::string& inPath);
        CHICANE_GRID void registerView(View* inView);
    }
}