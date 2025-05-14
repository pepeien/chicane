#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        CHICANE Component* createComponent(const pugi::xml_node& inNode);
        CHICANE void registerComponent(const std::string& inTag, Component::Compiler inCompiler);

        CHICANE bool hasActiveView();
        CHICANE View* getActiveView();
        CHICANE void activateView(const std::string& inPath);
        CHICANE Subscription<View*>* watchActiveView(
            std::function<void (View*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );
        CHICANE bool hasView(const std::string& inPath);
        CHICANE View* getView(const std::string& inPath);
        CHICANE void registerView(View* inView);
    }
}