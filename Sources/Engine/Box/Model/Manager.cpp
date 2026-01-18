#include "Chicane/Box/Model/Manager.hpp"

#include "Chicane/Box/Model/Wavefront.hpp"

namespace Chicane
{
    namespace Box
    {
        static const ModelExtracted EMPTY_INSTANCE = {};
        static const ModelParsed    EMPTY_DATA     = {};

        ModelManager::ModelManager()
            : Manager()
        {}

        void ModelManager::onAllocation(const String& inId, const ModelParsed& inData)
        {
            const ModelExtracted& instance = getInstance(inId);

            m_vertices.insert(m_vertices.end(), instance.vertices.begin(), instance.vertices.end());

            for (std::uint32_t index : instance.indices)
            {
                m_indices.push_back(index + inData.firstVertex);
            }
        }

        void ModelManager::onDeallocation(const String& inId)
        {
            std::uint32_t firstVertex = 0;
            std::uint32_t firstIndex  = 0;

            for (auto& [id, data] : m_datum)
            {
                data.firstVertex = firstVertex;
                data.firstIndex  = firstIndex;

                firstVertex += data.vertexCount;
                firstIndex += data.indexCount;
            }
        }

        void ModelManager::onActivation(const String& inId)
        {
            const ModelExtracted& instance = getInstance(inId);

            ModelParsed data = {};
            data.vertexCount = static_cast<std::uint32_t>(instance.vertices.size());
            data.firstVertex = static_cast<std::uint32_t>(m_vertices.size());
            data.indexCount  = static_cast<std::uint32_t>(instance.indices.size());
            data.firstIndex  = static_cast<std::uint32_t>(m_indices.size());

            Manager::allocate(inId, data);
        }

        void ModelManager::onDeactivation(const String& inId)
        {
            const ModelParsed& data = m_datum.at(inId);

            auto verticesStart = m_vertices.begin() + data.firstVertex;
            auto verticesEnd   = verticesStart + data.vertexCount;
            m_vertices.erase(verticesStart, verticesEnd);

            auto indicesStart = m_indices.begin() + data.firstIndex;
            auto indicesEnd   = indicesStart + data.indexCount;
            m_indices.erase(indicesStart, indicesEnd);

            Manager::deallocate(inId);
        }

        void ModelManager::load(const String& inId, const Model& inModel)
        {
            if (isLoaded(inId))
            {
                return;
            }

            switch (inModel.getVendor())
            {
            case ModelVendor::Wavefront:
                Manager::load(inId, ModelWavefront::parse(inModel.getData()));

                break;

            default:
                throw std::runtime_error("Failed to import Model due to invalid type");
            }
        }

        const ModelExtracted& ModelManager::getInstance(const String& inId) const
        {
            if (!isLoaded(inId))
            {
                return EMPTY_INSTANCE;
            }

            return m_instances.at(inId);
        }

        const ModelParsed& ModelManager::getData(const String& inId) const
        {
            if (!isAllocated(inId))
            {
                return EMPTY_DATA;
            }

            return m_datum.at(inId);
        }

        const Vertex::List& ModelManager::getVertices() const
        {
            return m_vertices;
        }

        const Vertex::Indices& ModelManager::getIndices() const
        {
            return m_indices;
        }

        std::uint32_t ModelManager::getFirstUse(const String& inId) const
        {
            if (!isLoaded(inId))
            {
                throw std::runtime_error("The Model [" + inId + "] does not exist");
            }

            return static_cast<std::uint32_t>(
                std::find(m_usedIds.begin(), m_usedIds.end(), inId) - m_usedIds.begin()
            );
        }
    }
}