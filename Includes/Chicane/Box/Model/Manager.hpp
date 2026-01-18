#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Model/Extracted.hpp"
#include "Chicane/Box/Model/Parsed.hpp"
#include "Chicane/Box/Manager.hpp"

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX ModelManager : public Box::Manager<ModelExtracted, ModelParsed>
        {
        public:
            ModelManager();

        protected:
            void onAllocation(const String& inId, const ModelParsed& inData) override;
            void onDeallocation(const String& inId) override;
            void onActivation(const String& inId) override;
            void onDeactivation(const String& inId) override;

        public:
            // Setup
            void load(const String& inId, const Model& inModel);

            // Data
            const ModelExtracted& getInstance(const String& inId) const;
            const ModelParsed& getData(const String& inId) const;
            const Vertex::List& getVertices() const;
            const Vertex::Indices& getIndices() const;
            std::uint32_t getFirstUse(const String& inId) const;

        private:
            Vertex::List    m_vertices;
            Vertex::Indices m_indices;
        };
    }
}