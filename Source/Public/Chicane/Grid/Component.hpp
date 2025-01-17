#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Component/BaseProps.hpp"
#include "Chicane/Grid/Component/ButtonComponent.hpp"
#include "Chicane/Grid/Component/Child.hpp"
#include "Chicane/Grid/Component/Compiler.hpp"
#include "Chicane/Grid/Component/ContainerComponent.hpp"
#include "Chicane/Grid/Component/Function.hpp"
#include "Chicane/Grid/Component/GridComponent.hpp"
#include "Chicane/Grid/Component/PopupComponent.hpp"
#include "Chicane/Grid/Component/ProgressBarComponent.hpp"
#include "Chicane/Grid/Component/TextComponent.hpp"
#include "Chicane/Grid/Component/TextInputComponent.hpp"
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