#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Component/BaseProps.hpp"
#include "Chicane/Grid/Component/Button.hpp"
#include "Chicane/Grid/Component/Child.hpp"
#include "Chicane/Grid/Component/Compiler.hpp"
#include "Chicane/Grid/Component/Container.hpp"
#include "Chicane/Grid/Component/Function.hpp"
#include "Chicane/Grid/Component/List.hpp"
#include "Chicane/Grid/Component/Popup.hpp"
#include "Chicane/Grid/Component/ProgressBar.hpp"
#include "Chicane/Grid/Component/Text.hpp"
#include "Chicane/Grid/Component/TextInput.hpp"
#include "Chicane/Grid/Component/Variable.hpp"

namespace Chicane
{
    namespace Grid
    {
        bool hasComponent(const std::string& inId);
        void addComponent(const std::string& inId, Component::Compiler::Function inCompiler);
        Component::Compiler::Function getComponent(const std::string& inId);
    }
}