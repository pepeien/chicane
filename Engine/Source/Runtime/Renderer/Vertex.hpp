#pragma once

#include "Base.hpp"

#include "CommandJob.hpp"
#include "Device.hpp"

namespace Engine
{
    namespace Vertex
    {
        struct BufferCreateInfo
        {
            vk::DeviceSize size;
            vk::BufferUsageFlags usage;
            vk::Device logicalDevice;
            vk::PhysicalDevice physicalDevice;
            vk::MemoryPropertyFlags memoryProperties;
        };

        struct Buffer
        {
            vk::Buffer instance;
            vk::DeviceMemory memory;
        };

        class Base
        {
        public:
            glm::vec2 position;
            glm::vec3 color;
            glm::vec2 texturePosition;
        };

        void initBuffer(Vertex::Buffer& outBuffer, const Vertex::BufferCreateInfo& inCreateInfo);
        void allocateBuffer(Vertex::Buffer& outBuffer, const Vertex::BufferCreateInfo& inCreateInfo);
        void copyBuffer(
            const Vertex::Buffer& inSourceBuffer,
            const Vertex::Buffer& inDestinationBuffer,
            const vk::DeviceSize& inAllocationSize,
            const vk::Queue& inQueue,
            const vk::CommandBuffer& inCommandBuffer
        );
    }

    class Vertex2D : public Vertex::Base
    {
    public:
        static vk::VertexInputBindingDescription getBindingDescription();
        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
        static std::vector<Vertex::Base*> toBaseList(const std::vector<Vertex2D*>& inVertices);
    }; 
}