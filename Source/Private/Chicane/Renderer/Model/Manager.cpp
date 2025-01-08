#include "Chicane/Renderer/Model/Manager.hpp"

static const Chicane::Model::Instance EMPTY_INSTANCE = {};
static const Chicane::Model::Data     EMPTY_DATA     = {};

namespace Chicane
{
    namespace Model
    {
        Manager::Manager()
            : Super()
        {}

        void Manager::onAllocation(const std::string& inId, const Data& inData)
        {
            const Instance& instance = getInstance(inId);

            m_vertices.insert(
                m_vertices.end(),
                instance.vertexInstances.begin(),
                instance.vertexInstances.end()
            );

            for (std::uint32_t index : instance.vertexIndices)
            {
                m_indices.push_back(index + inData.firstVertex);
            }
        }

        void Manager::onDeallocation(const std::string& inId)
        {
            std::uint32_t firstVertex = 0;
            std::uint32_t firstIndex  = 0;

            for (auto& [id, data] : m_datum)
            {
                data.firstVertex = firstVertex;
                data.firstIndex  = firstIndex;

                firstVertex += data.vertexCount;
                firstIndex  += data.indexCount;
            }
        }

        void Manager::onActivation(const std::string& inId)
        {
            const Instance& instance = getInstance(inId);

            Data data {};
            data.vertexCount = static_cast<std::uint32_t>(instance.vertexInstances.size());
            data.firstVertex = static_cast<std::uint32_t>(m_vertices.size());
            data.indexCount  = static_cast<std::uint32_t>(instance.vertexIndices.size());
            data.firstIndex  = static_cast<std::uint32_t>(m_indices.size());

            Super::allocate(inId, data);
        }

        void Manager::onDeactivation(const std::string& inId)
        {
            const Data& data = m_datum.at(inId);

            auto verticesStart = m_vertices.begin() + data.firstVertex;
            auto verticesEnd   = verticesStart + data.vertexCount;
            m_vertices.erase(verticesStart, verticesEnd);

            auto indicesStart = m_indices.begin() + data.firstIndex;
            auto indicesEnd   = indicesStart + data.indexCount;
            m_indices.erase(indicesStart, indicesEnd);

            Super::deallocate(inId);
        }

        void Manager::load(const std::string& inId, const Box::Model* inModel)
        {
            if (isLoaded(inId))
            {
                return;
            }

            Parse::Result result {};

            switch (inModel->getVendor())
            {
            case Vendor::Type::Wavefront:
                Vendor::Wavefront::parse(result, inModel->getData());

                break;

            default:
                throw std::runtime_error("Failed to import Model due to invalid type");
            }

            Instance instance {};
            instance.bounds          = Bounds(result.vertices);
            instance.vertexInstances = result.vertices;
            instance.vertexIndices   = result.indexes;

            Super::load(inId, instance);
        }

        const Model::Instance& Manager::getInstance(const std::string& inId) const
        {
            if (!isLoaded(inId))
            {
                return EMPTY_INSTANCE;
            }

            return m_instances.at(inId);
        }

        const Model::Data& Manager::getData(const std::string& inId) const
        {
            if (!isAllocated(inId))
            {
                return EMPTY_DATA;
            }

            return m_datum.at(inId);
        }
        
        const std::vector<Vertex::Instance>& Manager::getVertices() const
        {
            return m_vertices;
        }

        const std::vector<std::uint32_t>& Manager::getIndices() const
        {
            return m_indices;
        }

        std::uint32_t Manager::getFirstInstance(const std::string& inId) const
        {
            if (!isLoaded(inId))
            {
                throw std::runtime_error("The Model [" + inId + "] does not exist");
            }

            std::uint32_t result = static_cast<std::uint32_t>(
                std::find(
                    m_usedIds.begin(),
                    m_usedIds.end(),
                    inId
                ) - m_usedIds.begin()
            );

            return result;
        }
    }
}