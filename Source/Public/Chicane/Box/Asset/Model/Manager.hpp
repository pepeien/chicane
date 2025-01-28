#pragma once

#include "Chicane/Base.hpp"

#include "Chicane/Box/Asset/Model/Data.hpp"
#include "Chicane/Box/Asset/Model/Instance.hpp"
#include "Chicane/Box/Manager.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            class Manager : public Box::Manager::Instance<RawData, CompiledData>
            {
            public:
                Manager();

            protected:
                void onAllocation(const std::string& inId, const CompiledData& inData) override;
                void onDeallocation(const std::string& inId) override;
                void onActivation(const std::string& inId) override;
                void onDeactivation(const std::string& inId) override;

            public:
                // Setup
                void load(const std::string& inId, const Model::Instance* inModel);

                // Data
                const RawData& getInstance(const std::string& inId) const;
                const CompiledData& getData(const std::string& inId) const;
                const std::vector<Vertex::Instance>& getVertices() const;
                const std::vector<std::uint32_t>& getIndices() const;
                std::uint32_t getFirstUse(const std::string& inId) const;

            private:
                std::vector<Vertex::Instance> m_vertices;
                std::vector<std::uint32_t>    m_indices;
            };
        }
    }
}