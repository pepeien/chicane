#include "Chicane/Renderer/Texture/Manager.hpp"

namespace Chicane
{
    namespace Texture
    {
        Manager::Manager()
            : m_observable(std::make_unique<Observable<void*>>())
        {}

        bool Manager::isEmpty() const
        {
            return m_registeredIds.empty();
        }

        bool Manager::canBind() const
        {
            return !isEmpty() && !m_instanceMap.empty();
        }

        bool Manager::contains(const std::string& inId) const
        {
            return m_dataMap.find(inId) != m_dataMap.end();
        }

        std::uint32_t Manager::getCount() const
        {
            return m_registeredIds.size();
        }

        std::uint32_t Manager::getIndex(const std::string& inId) const
        {
            auto interator = std::find(
                m_registeredIds.begin(),
                m_registeredIds.end(),
                inId
            );

            if (interator == m_registeredIds.end())
            {
                return 0;
            }

            return static_cast<std::uint32_t>(interator - m_registeredIds.begin());
        }

        void Manager::add(const std::string& inId, const Box::Texture* inAsset)
        {
            if (contains(inId))
            {
                return;
            }

            m_dataMap.insert(
                std::make_pair(
                    inId,
                    inAsset->getData()
                )
            );

            m_registeredIds.push_back(inId);

            m_observable->next(nullptr);
        }

        void Manager::bindAll(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::PipelineLayout& inPipelineLayout,
            Descriptor::Bundle& inDescriptor
        )
        {
            for (std::string& id : m_registeredIds)
            {
                bind(
                    id,
                    inCommandBuffer,
                    inPipelineLayout,
                    inDescriptor
                );
            }
        }

        void Manager::bind(
            const std::string& inId,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::PipelineLayout& inPipelineLayout,
            Descriptor::Bundle& inDescriptor
        )
        {
            if (!canBind())
            {
                return;
            }

            auto foundPair = m_instanceMap.find(inId);

            if (m_instanceMap.find(inId) == m_instanceMap.end())
            {
                throw std::runtime_error(
                    "The Texture [" + inId + "] does not exists"
                );
            }

            Texture::Instance* texture = m_instanceMap.at(inId).get();

            if (texture == nullptr)
            {
                throw std::runtime_error(
                    "The Texture [" + inId + "] has not been initialized"
                );
            }

            inCommandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                inPipelineLayout,
                1,
                inDescriptor.set,
                nullptr
            );
        }

        void Manager::build(
            const vk::Device& inLogicalDevice,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            Descriptor::Bundle& inDescriptor
        )
        {
            if (isEmpty())
            {
                return;
            }

            Texture::CreateInfo textureCreateInfo;
            textureCreateInfo.logicalDevice  = inLogicalDevice;
            textureCreateInfo.physicalDevice = inPhysicalDevice;
            textureCreateInfo.commandBuffer  = inCommandBuffer;
            textureCreateInfo.queue          = inQueue;

            std::vector<vk::DescriptorImageInfo> imageInfos {};

            for (std::string& id : m_registeredIds)
            {
                if (m_dataMap.find(id) == m_dataMap.end())
                {
                    throw std::runtime_error("The Texture [" + id + "] does not exist");
                }

                textureCreateInfo.data = m_dataMap.at(id);

                m_instanceMap.insert(
                    std::make_pair(
                        id,
                        std::make_unique<Texture::Instance>(textureCreateInfo)
                    )
                );

                Image::Bundle image = m_instanceMap.at(id)->getImage();

                vk::DescriptorImageInfo imageDescriptorInfo;
                imageDescriptorInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                imageDescriptorInfo.imageView   = image.view;
                imageDescriptorInfo.sampler     = image.sampler;

                imageInfos.push_back(imageDescriptorInfo);
            }

            Descriptor::allocalteSet(
                inDescriptor.set,
                inLogicalDevice,
                inDescriptor.setLayout,
                inDescriptor.pool
            );

            vk::WriteDescriptorSet imageWriteDescriptorSet;
            imageWriteDescriptorSet.dstSet          = inDescriptor.set;
            imageWriteDescriptorSet.dstBinding      = 0;
            imageWriteDescriptorSet.dstArrayElement = 0;
            imageWriteDescriptorSet.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            imageWriteDescriptorSet.descriptorCount = imageInfos.size();
            imageWriteDescriptorSet.pImageInfo      = imageInfos.data();

            inLogicalDevice.updateDescriptorSets(
                imageWriteDescriptorSet,
                nullptr
            );
        }

        void Manager::watchChanges(
            std::function<void (void*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback,
            std::function<void ()> inCompleteCallback
        )
        {
            m_observable->subscribe(
                inNextCallback,
                inErrorCallback,
                inCompleteCallback
            );
        }
    }
}