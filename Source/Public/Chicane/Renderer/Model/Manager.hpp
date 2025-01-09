#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/Model.hpp"
#include "Chicane/Renderer/Manager.hpp"
#include "Chicane/Renderer/Model/Data.hpp"
#include "Chicane/Renderer/Model/Instance.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Model
    {
        class Manager : public Chicane::Manager::Instance<Model::Instance, Model::Data>
        {
        public:
            Manager();

        protected:
            void onAllocation(const std::string& inId, const Data& inData) override;
            void onDeallocation(const std::string& inId) override;
            void onActivation(const std::string& inId) override;
            void onDeactivation(const std::string& inId) override;

        public:
            // Setup
            void load(const std::string& inId, const Box::Model* inModel);

            // Data
            const Model::Instance& getInstance(const std::string& inId) const;
            const Model::Data& getData(const std::string& inId) const;
            const std::vector<Vertex::Instance>& getVertices() const;
            const std::vector<std::uint32_t>& getIndices() const;
            std::uint32_t getFirstInstance(const std::string& inId) const;

        private:
            std::vector<Vertex::Instance> m_vertices;
            std::vector<std::uint32_t>    m_indices;
        };
    }
}