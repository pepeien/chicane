#pragma once

#include <map>
#include <concepts>

#include "Base.hpp"

#include "Vertex.hpp"
#include "Vertex/V2.hpp"
#include "Vertex/V3.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Mesh
            {
                template<typename T>
                class Manager
                {
                public:
                    Manager(
                        vk::Device& inLogicalDevice,
                        vk::PhysicalDevice& inPhysicalDevice
                    )
                    {
                        static_assert(std::derived_from<T, Vertex::Base> == true);

                        logicalDevice  = inLogicalDevice;
                        physicalDevice = inPhysicalDevice;
                    }

                    ~Manager()
                    {
                        logicalDevice.destroyBuffer(vertexBuffer.instance);
                        logicalDevice.freeMemory(vertexBuffer.memory);
                    }

                    void add(std::string inMeshID, std::vector<T> inVertices)
                    {
                        if (vertexMap.find(inMeshID) != vertexMap.end())
                        {
                            throw std::runtime_error("Mesh already added");
                        }

                        vertexMap.insert(std::make_pair(inMeshID, inVertices));
                    }

                    void copyToGPU()
                    {
                        size_t allocationSize = 0;

                        std::vector<T> allVertices;

                        for (auto entry : vertexMap)
                        {
                            allocationSize =+ sizeof(entry.second[0]) * entry.second.size();

                            allVertices.insert(allVertices.end(), entry.second.begin(), entry.second.end());
                        }

                        Vertex::BufferCreateInfo bufferCreateInfo;
                        bufferCreateInfo.physicalDevice = physicalDevice;
                        bufferCreateInfo.logicalDevice  = logicalDevice;
                        bufferCreateInfo.size           = allocationSize;
                        bufferCreateInfo.usage          = vk::BufferUsageFlagBits::eVertexBuffer;

                        Vertex::initBuffer(vertexBuffer, bufferCreateInfo);

                        void* memoryLocation = logicalDevice.mapMemory(vertexBuffer.memory, 0, bufferCreateInfo.size);
                        memcpy(memoryLocation, allVertices.data(), bufferCreateInfo.size);

                        logicalDevice.unmapMemory(vertexBuffer.memory);
                    }

                public:
                    Vertex::Buffer vertexBuffer;

                private:
                    vk::Device logicalDevice;
                    vk::PhysicalDevice physicalDevice;

                    std::map<std::string, std::vector<T>> vertexMap;
                };
            }
        }
    }
}