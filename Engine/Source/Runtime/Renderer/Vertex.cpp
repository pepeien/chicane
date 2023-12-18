#include "Vertex.hpp"

namespace Engine
{
    namespace Vertex
    {
        void initBuffer(Buffer& outBuffer, const BufferCreateInfo& inCreateInfo)
        {
            vk::BufferCreateInfo bufferInfo;
            bufferInfo.flags       = vk::BufferCreateFlags();
            bufferInfo.size        = inCreateInfo.size;
            bufferInfo.usage       = inCreateInfo.usage;
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;

            outBuffer.instance = inCreateInfo.logicalDevice.createBuffer(bufferInfo);

            allocateBuffer(outBuffer, inCreateInfo);
        }

        void allocateBuffer(Buffer& outBuffer, const BufferCreateInfo& inCreateInfo)
        {
            vk::MemoryRequirements memoryRequirements = inCreateInfo.logicalDevice.getBufferMemoryRequirements(outBuffer.instance);

            vk::MemoryAllocateInfo memoryAlocateInfo;
            memoryAlocateInfo.allocationSize  = memoryRequirements.size;
            memoryAlocateInfo.memoryTypeIndex = Device::findMemoryTypeIndex(
                inCreateInfo.physicalDevice,
                memoryRequirements.memoryTypeBits,
                inCreateInfo.memoryProperties
            );

            outBuffer.memory = inCreateInfo.logicalDevice.allocateMemory(memoryAlocateInfo);

            inCreateInfo.logicalDevice.bindBufferMemory(outBuffer.instance, outBuffer.memory, 0);
        }

        void copyBuffer(
            const Buffer& inSourceBuffer,
            const Buffer& inDestinationBuffer,
            const vk::DeviceSize& inAllocationSize,
            const vk::Queue& inQueue,
            const vk::CommandBuffer& inCommandBuffer
        )
        {
            Command::Job::start(inCommandBuffer);

            vk::BufferCopy copyRegion;
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size      = inAllocationSize;

            inCommandBuffer.copyBuffer(
                inSourceBuffer.instance,
                inDestinationBuffer.instance,
                1,
                &copyRegion
            );

            Command::Job::end(inCommandBuffer, inQueue, "Copy The Vertex Fuffer");
        }
    }

    vk::VertexInputBindingDescription Vertex2D::getBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding   = 0;
        bindingDescription.stride    = sizeof(Vertex2D);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;
            
        return bindingDescription;
    }
       
    std::vector<vk::VertexInputAttributeDescription> Vertex2D::getAttributeDescriptions()
    {
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

        // Position
        vk::VertexInputAttributeDescription positionDescription;
        positionDescription.binding  = 0;
        positionDescription.location = 0;
        positionDescription.format   = vk::Format::eR32G32Sfloat;
        positionDescription.offset   = offsetof(Vertex2D, position);

        attributeDescriptions.push_back(positionDescription);

        // Color
        vk::VertexInputAttributeDescription colorDescription;
        colorDescription.binding  = 0;
        colorDescription.location = 1;
        colorDescription.format   = vk::Format::eR32G32B32Sfloat;
        colorDescription.offset   = offsetof(Vertex2D, color);

        attributeDescriptions.push_back(colorDescription);

        // Texture Position
        vk::VertexInputAttributeDescription texturePositionDescription;
        texturePositionDescription.binding  = 0;
        texturePositionDescription.location = 2;
        texturePositionDescription.format   = vk::Format::eR32G32Sfloat;
        texturePositionDescription.offset   = offsetof(Vertex2D, texturePosition);

        attributeDescriptions.push_back(texturePositionDescription);
            
        return attributeDescriptions;
    }

    std::vector<Vertex::Base*> Vertex2D::toBaseList(const std::vector<Vertex2D*>& inVertices)
    {
        std::vector<Base*> result;

        for (Vertex2D* vertex : inVertices)
        {
            result.push_back(vertex);
        }

        return result;
    }
}