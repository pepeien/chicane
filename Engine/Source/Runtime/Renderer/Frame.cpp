#include "Frame.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Frame
            {
				void Instance::initResources(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice, Scene::Instance& inScene)
                {
                    createCameraData(inLogicalDevice, inPhysicalDevice);
                    createModelData(inLogicalDevice, inPhysicalDevice, inScene);
                }

                void Instance::writeDescriptorSet(vk::Device& inLogicalDevice)
                {
                    vk::WriteDescriptorSet uniformWriteInfo;
                    uniformWriteInfo.dstSet          = descriptorSet;
                    uniformWriteInfo.dstBinding      = 0;
                    uniformWriteInfo.dstArrayElement = 0;
                    uniformWriteInfo.descriptorCount = 1;
                    uniformWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                    uniformWriteInfo.pBufferInfo     = &uniformDescriptorBufferInfo;

                    inLogicalDevice.updateDescriptorSets(uniformWriteInfo, nullptr);

                    vk::WriteDescriptorSet modelWriteInfo;
                    modelWriteInfo.dstSet          = descriptorSet;
                    modelWriteInfo.dstBinding      = 1;
                    modelWriteInfo.dstArrayElement = 0;
                    modelWriteInfo.descriptorCount = 1;
                    modelWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                    modelWriteInfo.pBufferInfo     = &modelDescriptorBufferInfo;

                    inLogicalDevice.updateDescriptorSets(modelWriteInfo, nullptr);
                }

                void Instance::createCameraData(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice)
                {
                    Vertex::BufferCreateInfo cameraBufferCreateInfo;
                    cameraBufferCreateInfo.logicalDevice    = inLogicalDevice;
                    cameraBufferCreateInfo.physicalDevice   = inPhysicalDevice;
                    cameraBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
                    cameraBufferCreateInfo.size             = sizeof(Uniform::BufferObject);
                    cameraBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

                    Vertex::initBuffer(cameraData.buffer, cameraBufferCreateInfo);

                    cameraData.allocationSize = cameraBufferCreateInfo.size;
                    cameraData.writeLocation  = inLogicalDevice.mapMemory(
                        cameraData.buffer.memory,
                        0,
                        cameraData.allocationSize
                    );

                    uniformDescriptorBufferInfo.buffer = cameraData.buffer.instance;
                    uniformDescriptorBufferInfo.offset = 0;
                    uniformDescriptorBufferInfo.range  = cameraData.allocationSize;
                }

                void Instance::createModelData(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice, Scene::Instance& inScene)
                {
                    auto sceneObjects = inScene.getObjects();

                    Vertex::BufferCreateInfo modelBufferCreateInfo;
                    modelBufferCreateInfo.logicalDevice    = inLogicalDevice;
                    modelBufferCreateInfo.physicalDevice   = inPhysicalDevice;
                    modelBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
                    modelBufferCreateInfo.size             = sizeof(glm::mat4) * sceneObjects.size();
                    modelBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;

                    Vertex::initBuffer(modelData.buffer, modelBufferCreateInfo);
    
                    modelData.allocationSize = modelBufferCreateInfo.size;
                    modelData.writeLocation  = inLogicalDevice.mapMemory(
                        modelData.buffer.memory,
                        0,
                        modelData.allocationSize
                    );
                    modelData.transforms.reserve(sceneObjects.size());

                    for (uint32_t i = 0; i < sceneObjects.size(); i++)
                    {
                        modelData.transforms.push_back(glm::mat4(1.0f));
                    }
                    
                    modelDescriptorBufferInfo.buffer = modelData.buffer.instance;
                    modelDescriptorBufferInfo.offset = 0;
                    modelDescriptorBufferInfo.range  = modelData.allocationSize;
                }

                void initBuffer(BufferCreateInfo& outCreateInfo)
                {
                    for (int i = 0; i < outCreateInfo.frames.size(); i++)
                    {
                        std::vector<vk::ImageView> attachments = {
                            outCreateInfo.frames[i].imageView
                        };

                        vk::FramebufferCreateInfo framebufferInfo = {};
                        framebufferInfo.flags           = vk::FramebufferCreateFlags();
                        framebufferInfo.renderPass      = outCreateInfo.renderPass;
                        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                        framebufferInfo.pAttachments    = attachments.data();
                        framebufferInfo.width           = outCreateInfo.swapChainExtent.width;
                        framebufferInfo.height          = outCreateInfo.swapChainExtent.height;
                        framebufferInfo.layers          = 1;

                        outCreateInfo.frames[i].framebuffer = outCreateInfo.logicalDevice.createFramebuffer(framebufferInfo);
                    }
                }
			}
		}
	}
}