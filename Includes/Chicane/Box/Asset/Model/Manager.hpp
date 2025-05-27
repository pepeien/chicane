#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"
#include "Chicane/Box/Asset/Model/Data.hpp"
#include "Chicane/Box/Asset/Model/Instance.hpp"

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
                void onAllocation(const std::string& inId, const Parsed& inData) override;
                void onDeallocation(const std::string& inId) override;
                void onActivation(const std::string& inId) override;
                void onDeactivation(const std::string& inId) override;

            public:
                // Setup
                void load(const std::string& inId, const Model::Instance& inModel);

                // Data
                const Extracted& getInstance(const std::string& inId) const;
                const Parsed& getData(const std::string& inId) const;
                const std::vector<Vertex>& getVertices() const;
                const std::vector<std::uint32_t>& getIndices() const;
                std::uint32_t getFirstUse(const std::string& inId) const;

            private:
                std::vector<Vertex>        m_vertices;
                std::vector<std::uint32_t> m_indices;
            };
        }
    }
}