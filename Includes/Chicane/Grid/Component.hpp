#pragma once

#include "Chicane.hpp"
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
        bool hasComponent(const std::string& inId);
        void addComponent(const std::string& inId, Component::Compiler::Function inCompiler);
        Component::Compiler::Function getComponent(const std::string& inId);
    }
}