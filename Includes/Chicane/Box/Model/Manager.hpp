#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Model/Parsed.hpp"
#include "Chicane/Box/Manager.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX ModelManager : public Box::Manager<Model, ModelParsed>
        {
        public:
            ModelManager();
            ~ModelManager() = default;

        protected:
            void onLoad(const String& inId, const Model& inData) override;
        };
    }
}