#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"
#include "Chicane/Box/Asset/Model/Extracted.hpp"
#include "Chicane/Box/Asset/Model/Instance.hpp"
#include "Chicane/Box/Asset/Model/Parsed.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            class CHICANE_BOX Manager : public Box::Manager::Instance<Extracted, Parsed>
            {
            public:
                Manager();

            protected:
                void onAllocation(const String& inId, const Parsed& inData) override;
                void onDeallocation(const String& inId) override;
                void onActivation(const String& inId) override;
                void onDeactivation(const String& inId) override;

            public:
                // Setup
                void load(const String& inId, const Model::Instance& inModel);

                // Data
                const Extracted& getInstance(const String& inId) const;
                const Parsed& getData(const String& inId) const;
                const std::vector<Vertex>& getVertices() const;
                const std::vector<std::uint32_t>& getIndices() const;
                std::uint32_t getFirstUse(const String& inId) const;

            private:
                std::vector<Vertex>        m_vertices;
                std::vector<std::uint32_t> m_indices;
            };
        }
    }
}