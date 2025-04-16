#pragma once

#include "Chicane/Core.hpp"
#include "Component/BaseProps.hpp"
#include "Component/Button.hpp"
#include "Component/Child.hpp"
#include "Component/Compiler.hpp"
#include "Component/Container.hpp"
#include "Component/Function.hpp"
#include "Component/List.hpp"
#include "Component/Popup.hpp"
#include "Component/ProgressBar.hpp"
#include "Component/Text.hpp"
#include "Component/TextInput.hpp"
#include "Component/Variable.hpp"

namespace Chicane
{
    namespace Grid
    {
        CHICANE bool hasComponent(const std::string& inId);
        CHICANE void addComponent(const std::string& inId, Component::Compiler::Function inCompiler);
        CHICANE Component::Compiler::Function getComponent(const std::string& inId);
    }
}