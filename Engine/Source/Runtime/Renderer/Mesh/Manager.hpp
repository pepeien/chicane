#pragma once

#include "Base.hpp"

#include "Instance.hpp"
#include "Renderer/Vertex.hpp"

namespace Engine
{
    namespace Mesh
    {
        namespace Manager
        {
            struct CreateInfo
            {
                vk::Device logicalDevice;
                vk::PhysicalDevice physicalDevice;
                vk::Queue queue;
                vk::CommandBuffer commandBuffer;
            };

            class Instance
            {
            public:
                Instance(CreateInfo& inCreateInfo);
                ~Instance();

            public:
                void addMesh(const std::string& inMeshId, const std::vector<Vertex::Base*>& inVertices);
                Mesh::Instance getMesh(const std::string& inMeshId);

                void proccess();

            private:
                void extractAllocationDataFromMeshList(std::vector<Vertex::Base>& outVertices, vk::DeviceSize& outAllocationSize);

            public:
                Vertex::Buffer vertexBuffer;

            private:
                vk::Device logicalDevice;
                vk::PhysicalDevice physicalDevice;
                vk::Queue queue;
                vk::CommandBuffer commandBuffer;

                std::vector<std::string> meshesOrder;
                std::unordered_map<std::string, Mesh::Instance> meshesMap;
            };
        }
    }
}