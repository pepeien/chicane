#pragma once

#include "Base.hpp"

#include "Device.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Vertex
            {
                struct BufferCreateInfo
				{
					size_t size;
					vk::BufferUsageFlags usage;
					vk::Device logicalDevice;
					vk::PhysicalDevice physicalDevice;
				};

				struct Buffer
				{
					vk::Buffer instance;
					vk::DeviceMemory memory;
				};

                struct V2
                {
                public:
                    glm::vec2 position;
                    glm::vec3 color;
                };  

                template<typename T>
                vk::VertexInputBindingDescription getBindingDescription()
                {
                    vk::VertexInputBindingDescription bindingDescription{};
                    bindingDescription.binding   = 0;
                    bindingDescription.stride    = sizeof(T);
                    bindingDescription.inputRate = vk::VertexInputRate::eVertex;

                    return bindingDescription;
                }

                template<typename T>
                std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
                {
                    std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

                    // Position
                    attributeDescriptions[0].binding  = 0;
                    attributeDescriptions[0].location = 0;
                    attributeDescriptions[0].format   = vk::Format::eR32G32Sfloat;
                    attributeDescriptions[0].offset   = offsetof(T, position);

                    // Color
                    attributeDescriptions[1].binding  = 0;
                    attributeDescriptions[1].location = 1;
                    attributeDescriptions[1].format   = vk::Format::eR32G32B32Sfloat;
                    attributeDescriptions[1].offset   = offsetof(T, color);

                    return attributeDescriptions;
                }

                void initBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo);
			    void allocateBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo);
            };
        }
    }
}