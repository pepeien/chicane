#include "Chicane/Box/Asset/Model/Manager.hpp"

#include "Chicane/Box/Asset/Model/Wavefront.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            static const Extracted   EMPTY_INSTANCE = {};
            static const Parsed EMPTY_DATA     = {};

            Manager::Manager()
                : Super()
            {}

            void Manager::onAllocation(const std::string& inId, const Parsed& inData)
            {
                const Extracted& instance = getInstance(inId);

                m_vertices.insert(
                    m_vertices.end(),
                    instance.vertices.begin(),
                    instance.vertices.end()
                );

                for (std::uint32_t index : instance.indices)
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
                const Extracted& instance = getInstance(inId);

                Parsed data = {};
                data.vertexCount = static_cast<std::uint32_t>(instance.vertices.size());
                data.firstVertex = static_cast<std::uint32_t>(m_vertices.size());
                data.indexCount  = static_cast<std::uint32_t>(instance.indices.size());
                data.firstIndex  = static_cast<std::uint32_t>(m_indices.size());

                Super::allocate(inId, data);
            }

            void Manager::onDeactivation(const std::string& inId)
            {
                const Parsed& data = m_datum.at(inId);

                auto verticesStart = m_vertices.begin() + data.firstVertex;
                auto verticesEnd   = verticesStart + data.vertexCount;
                m_vertices.erase(verticesStart, verticesEnd);

                auto indicesStart = m_indices.begin() + data.firstIndex;
                auto indicesEnd   = indicesStart + data.indexCount;
                m_indices.erase(indicesStart, indicesEnd);

                Super::deallocate(inId);
            }

            void Manager::load(const std::string& inId, const Model::Instance& inModel)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                switch (inModel.getVendor())
                {
                case Vendor::Wavefront:
                    Super::load(inId, Wavefront::parse(inModel.getData()));

                    break;

                default:
                    throw std::runtime_error("Failed to import Model due to invalid type");
                }
            }

            const Extracted& Manager::getInstance(const std::string& inId) const
            {
                if (!isLoaded(inId))
                {
                    return EMPTY_INSTANCE;
                }

                return m_instances.at(inId);
            }

            const Parsed& Manager::getData(const std::string& inId) const
            {
                if (!isAllocated(inId))
                {
                    return EMPTY_DATA;
                }

                return m_datum.at(inId);
            }

            const std::vector<Vertex>& Manager::getVertices() const
            {
                return m_vertices;
            }

            const std::vector<std::uint32_t>& Manager::getIndices() const
            {
                return m_indices;
            }

            std::uint32_t Manager::getFirstUse(const std::string& inId) const
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
}