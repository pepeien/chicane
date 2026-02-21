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
            using Children = std::vector<String>;
            using Groups   = std::unordered_map<String, Children>;

        public:
            ModelManager();

        protected:
            void onLoad(const String& inId, const Model& inData) override;

        public:
            Children getChildren(const String& inId) const;

        private:
            Groups m_groups;
        };
    }
}