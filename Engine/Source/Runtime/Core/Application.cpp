#include "Application.hpp"

namespace Chicane
{
    Application::Application(const std::string& inWindowTitle, const Level::Instance& inLevel)
    : m_window({ nullptr, inWindowTitle, 0, 0 }),
      m_level(inLevel),
      m_currentImageIndex(0),
      m_meshManager(std::make_unique<Mesh::Manager::Instance>()),
      m_textureManager(std::make_unique<Texture::Manager::Instance>()),
      m_frameStats({ 0, 0.0f, 0.0f, 0.0f })
    {
        // Assets pre load
        loadAssets();

        // GLFW
        glfwInit();
        buildWindow();

        // Vulkan
        buildInstance();
        buildDebugMessenger();
        buildSurface();
        buildDevices();
        buildQueues();
        buildSwapChain();
        buildFrameDescriptorSetLayout();
        buildMaterialDescriptorSetLayout();
        buildGraphicsPipeline();
        buildFramebuffers();
        buildCommandPool();
        buildMainCommandBuffer();
        buildFramesCommandBuffers();
        buildFrameResources();
        buildMaterialResources();

        // Assets building
        buildAssets();
    }

    Application::~Application()
    {
        // Vulkan
        m_logicalDevice.waitIdle();

        destroyCommandPool();
        destroyGraphicsPipeline();
        destroySwapChain();

        m_logicalDevice.destroyDescriptorSetLayout(m_frameDescriptors.setLayout);

        m_logicalDevice.destroyDescriptorSetLayout(m_materialDescriptors.setLayout);
        m_logicalDevice.destroyDescriptorPool(m_materialDescriptors.pool);

        Buffer::destroy(m_logicalDevice, m_meshVertexBuffer);
        Buffer::destroy(m_logicalDevice, m_meshIndexBuffer);

        destroyAssets();
        destroyDevices();
        destroySurface();

        if (IS_DEBUGGING)
        {
            destroyDebugMessenger();
        }

        destroyInstance();

        // GLFW
        glfwTerminate();
    }

    void Application::run()
    {
        while (glfwWindowShouldClose(m_window.instance) == false)
        {
            glfwPollEvents();
            render();
            calculateFrameRate();
        }
    }

    void Application::draw(const vk::CommandBuffer& inCommandBuffer, uint32_t inImageIndex)
    {
        vk::CommandBufferBeginInfo beginInfo = {};

        inCommandBuffer.begin(beginInfo);

        vk::ClearValue clearColor;
        clearColor.color = vk::ClearColorValue(0.051f, 0.051f, 0.051f, 1.0f);

        vk::ClearValue clearDepth;
        clearDepth.depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

        std::vector<vk::ClearValue> clearValues = {{ clearColor, clearDepth }};

        vk::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline.renderPass;
        renderPassBeginInfo.framebuffer         = m_swapChain.frames[inImageIndex].framebuffer;
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = m_swapChain.extent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues        = clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );

        inCommandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            m_graphicsPipeline.instance
        );
    
        inCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_graphicsPipeline.layout,
            0,
            m_swapChain.frames[inImageIndex].descriptorSet,
            nullptr
        );

        prepareLevel(inCommandBuffer);

        drawLevel(inCommandBuffer);

        inCommandBuffer.endRenderPass();

        inCommandBuffer.end();
    }

    void Application::render()
    {
        Frame::Instance& currentFrame = m_swapChain.frames[m_currentImageIndex];

        if (
            m_logicalDevice.waitForFences(
                1,
                &currentFrame.renderFence,
                VK_TRUE,
                UINT64_MAX
            ) != vk::Result::eSuccess
        )
        {
            LOG_WARNING("Error while waiting the fences");
        }

        if (
            m_logicalDevice.resetFences(
                1,
                &currentFrame.renderFence
            ) != vk::Result::eSuccess
        )
        {
            LOG_WARNING("Error while resetting the fences");
        }

        uint32_t imageIndex;

        try
        {
            imageIndex = m_logicalDevice.acquireNextImageKHR(
                m_swapChain.instance,
                UINT64_MAX,
                currentFrame.presentSemaphore,
                nullptr
            ).value;
        }
        catch(vk::OutOfDateKHRError e)
        {
            rebuildSwapChain();

            return;
        }
        catch(vk::IncompatibleDisplayKHRError e)
        {
            rebuildSwapChain();

            return;
        }

        vk::CommandBuffer commandBuffer = m_swapChain.frames[imageIndex].commandBuffer;

        commandBuffer.reset();

        prepareFrame(imageIndex);

        draw(commandBuffer, imageIndex);

        vk::Semaphore waitSemaphores[]      = { currentFrame.presentSemaphore };
        vk::Semaphore signalSemaphores[]    = { currentFrame.renderSemaphore };

        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        vk::SubmitInfo submitInfo = {};
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.pWaitDstStageMask    = waitStages;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;

        m_graphicsQueue.submit(submitInfo, currentFrame.renderFence);

        vk::SwapchainKHR swapChains[] = { m_swapChain.instance };

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = swapChains;
        presentInfo.pImageIndices      = &imageIndex;

        vk::Result present;

        try
        {
            present = m_presentQueue.presentKHR(presentInfo);
        }
        catch(vk::OutOfDateKHRError e)
        {
            present = vk::Result::eErrorOutOfDateKHR;
        }

        if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR)
        {
            rebuildSwapChain();

            return;
        }

        m_currentImageIndex = (m_currentImageIndex + 1) % m_maxInFlightFramesCount;
    }

    void Application::calculateFrameRate()
    {
        m_frameStats.currentTime = glfwGetTime();
        double delta = m_frameStats.currentTime - m_frameStats.lastTime;

        if (delta >= 1) {
            int framerate{ std::max(1, int(m_frameStats.count / delta)) };

            std::stringstream title;
            title << m_window.title << " - " << framerate << " FPS";

            glfwSetWindowTitle(
                m_window.instance,
                title.str().c_str()
            );

            m_frameStats.lastTime  = m_frameStats.currentTime;
            m_frameStats.count     = -1;
            m_frameStats.time      = float(1000.0 / framerate);
        }

        m_frameStats.count++;
    }

    void Application::buildWindow()
    {
        Window::init(m_window);
    }

    void Application::buildInstance()
    {
        Instance::init(m_instance, m_dldi);
    }

    void Application::destroyInstance()
    {
        m_instance.destroy();
    }

    void Application::buildDebugMessenger()
    {
        if (IS_DEBUGGING == false)
        {
            return;
        }

        Debug::initMessenger(m_debugMessenger, m_instance, m_dldi);
    }

    void Application::destroyDebugMessenger()
    {
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger, nullptr, m_dldi);
    }

    void Application::buildSurface()
    {
        Surface::init(m_surface, m_instance, m_window.instance);
    }

    void Application::destroySurface()
    {
        m_instance.destroySurfaceKHR(m_surface);
    }

    void Application::buildQueues()
    {
        Queue::initGraphicsQueue(
            m_graphicsQueue,
            m_physicalDevice,
            m_logicalDevice,
            m_surface
        );

        Queue::initPresentQueue(
            m_presentQueue,
            m_physicalDevice,
            m_logicalDevice,
            m_surface
        );
    }

    void Application::buildDevices()
    {
        Device::pickPhysicalDevice(m_physicalDevice, m_instance);
        Device::initLogicalDevice(
            m_logicalDevice,
            m_physicalDevice,
            m_surface
        );
    }
                
    void Application::destroyDevices()
    {
        m_logicalDevice.destroy();
    }

    void Application::buildSwapChain()
    {
        SwapChain::init(
            m_swapChain,
            m_physicalDevice,
            m_logicalDevice,
            m_surface,
            m_window.width,
            m_window.height
        );

        m_maxInFlightFramesCount = static_cast<int>(m_swapChain.frames.size());

        for (Frame::Instance& frame : m_swapChain.frames)
        {
            frame.physicalDevice = m_physicalDevice;
            frame.logicalDevice  = m_logicalDevice;
            frame.width          = m_swapChain.extent.width;
            frame.height         = m_swapChain.extent.height;

            frame.setupDepthBuffering();
        }
    }

    void Application::rebuildSwapChain()
    {
        do
        {
            glfwGetFramebufferSize(
                m_window.instance,
                &m_window.width,
                &m_window.height
            );

            glfwWaitEvents();
        } while (m_window.width == 0 || m_window.height == 0);

        m_logicalDevice.waitIdle();

        destroySwapChain();
        buildSwapChain();

        buildFramebuffers();
        buildFrameResources();
        buildFramesCommandBuffers();
    }

    void Application::destroySwapChain()
    {
        for (Frame::Instance& frame : m_swapChain.frames)
        {
            frame.destroy();
        }

        m_logicalDevice.destroySwapchainKHR(m_swapChain.instance);

        m_logicalDevice.destroyDescriptorPool(m_frameDescriptors.pool);
    }

    void Application::buildFrameDescriptorSetLayout()
    {
        Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 2;

        // Camera
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        // Model
        frameLayoutBidings.indices.push_back(1);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eStorageBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Descriptor::initSetLayout(
            m_frameDescriptors.setLayout,
            m_logicalDevice,
            frameLayoutBidings
        );
    }

    void Application::buildMaterialDescriptorSetLayout()
    {
        Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;

        // Texture
        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(1);
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            m_materialDescriptors.setLayout,
            m_logicalDevice,
            materialLayoutBidings
        );
    }

    void Application::buildGraphicsPipeline()
    {
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.push_back(m_frameDescriptors.setLayout);
        descriptorSetLayouts.push_back(m_materialDescriptors.setLayout);
    
        GraphicsPipeline::CreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.logicalDevice        = m_logicalDevice;
        graphicsPipelineCreateInfo.vertexShaderName     = "triangle.vert.spv";
        graphicsPipelineCreateInfo.fragmentShaderName   = "triangle.frag.spv";
        graphicsPipelineCreateInfo.swapChainExtent      = m_swapChain.extent;
        graphicsPipelineCreateInfo.swapChainImageFormat = m_swapChain.format;
        graphicsPipelineCreateInfo.depthFormat          = m_swapChain.frames[0].depthFormat;
        graphicsPipelineCreateInfo.descriptorSetLayouts = descriptorSetLayouts;

        GraphicsPipeline::init(
            m_graphicsPipeline,
            graphicsPipelineCreateInfo
        );    
    }

    void Application::destroyGraphicsPipeline()
    {
        m_logicalDevice.destroyPipeline(m_graphicsPipeline.instance);
        m_logicalDevice.destroyPipelineLayout(m_graphicsPipeline.layout);
        m_logicalDevice.destroyRenderPass(m_graphicsPipeline.renderPass);
    }

    void Application::buildFramebuffers()
    {
        Frame::Buffer::CreateInfo createInfo = {
            m_logicalDevice,
            m_graphicsPipeline.renderPass,
            m_swapChain.extent
        };

        Frame::Buffer::init(m_swapChain.frames, createInfo);
    }

    void Application::buildCommandPool()
    {
        CommandBuffer::Pool::init(
            m_mainCommandPool,
            m_logicalDevice,
            m_physicalDevice,
            m_surface
        );
    }

    void Application::destroyCommandPool()
    {
        m_logicalDevice.destroyCommandPool(m_mainCommandPool);
    }

    void Application::buildMainCommandBuffer()
    {
        CommandBuffer::Instance::CreateInfo createInfo = {
            m_logicalDevice,
            m_mainCommandPool
        };

        CommandBuffer::Instance::init(m_mainCommandBuffer, createInfo);
    }

    void Application::buildFramesCommandBuffers()
    {
        CommandBuffer::Instance::CreateInfo createInfo = {
            m_logicalDevice,
            m_mainCommandPool
        };

        Frame::Buffer::initCommand(m_swapChain.frames, createInfo);
    }

    void Application::buildFrameResources()
    {
        Descriptor::PoolCreateInfo poolCreateInfo;
        poolCreateInfo.count = 2;
        poolCreateInfo.size  = static_cast<uint32_t>(m_swapChain.frames.size());
        poolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        poolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Descriptor::initPool(
            m_frameDescriptors.pool,
            m_logicalDevice,
            poolCreateInfo
        );

        for (Frame::Instance& frame : m_swapChain.frames)
        {
            frame.setupSync();
            frame.setupCamera();
            frame.setupModelData(m_level.getActors());
            frame.setupDescriptors(
                m_frameDescriptors.setLayout,
                m_frameDescriptors.pool
            );
        }
    }

    void Application::buildMaterialResources()
    {
        Descriptor::PoolCreateInfo poolCreateInfo;
        poolCreateInfo.count = 1;
        poolCreateInfo.size  = m_textureManager->getCount();
        poolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_materialDescriptors.pool,
            m_logicalDevice,
            poolCreateInfo
        );
    }

    void Application::loadMeshes()
    {
        m_meshManager->importMesh(
            "Cube",
            "cube.obj",
            Mesh::Type::Wavefront
        );

        m_meshManager->importMesh(
            "AirCraft",
            "air_craft.obj",
            Mesh::Type::Wavefront
        );
    }

    void Application::buildMeshes()
    {
        m_meshManager->initBuffers(
            m_meshVertexBuffer,
            m_meshIndexBuffer,
            m_logicalDevice,
            m_physicalDevice,
            m_graphicsQueue,
            m_mainCommandBuffer
        );
    }

    void Application::loadTextures()
    {
        Texture::Data grayTextureData;
        grayTextureData.width    = 512;
        grayTextureData.height   = 512;
        grayTextureData.filepath = "Base/gray.png";

        m_textureManager->addTexture("Gray", grayTextureData);

        Texture::Data gridTextureData;
        gridTextureData.width    = 512;
        gridTextureData.height   = 512;
        gridTextureData.filepath = "Base/grid.png";

        m_textureManager->addTexture("Grid", gridTextureData);

        Texture::Data uvTextureData;
        uvTextureData.width    = 512;
        uvTextureData.height   = 512;
        uvTextureData.filepath = "Base/uv.png";

        m_textureManager->addTexture("UV", uvTextureData);
    }

    void Application::buildTextures()
    {
        m_textureManager->buildTexturesInstances(
            m_logicalDevice,
            m_physicalDevice,
            m_mainCommandBuffer,
            m_graphicsQueue,
            m_materialDescriptors.setLayout,
            m_materialDescriptors.pool
        );
    }

    void Application::destroyTextures()
    {
        m_textureManager->destroyTexturesInstances();
    }

    void Application::loadAssets()
    {
        loadMeshes();
        loadTextures();
    }

    void Application::buildAssets()
    {
        buildMeshes();
        buildTextures();
    }

    void Application::destroyAssets()
    {
        m_meshManager.reset();
        m_textureManager.reset();
    }

    void Application::prepareLevel(const vk::CommandBuffer& inCommandBuffer)
    {
        vk::Buffer vertexBuffers[] = { m_meshVertexBuffer.instance };
        vk::DeviceSize offsets[]   = { 0 };

        inCommandBuffer.bindVertexBuffers(
            0,
            1,
            vertexBuffers,
            offsets
        );

        inCommandBuffer.bindIndexBuffer(
            m_meshIndexBuffer.instance,
            0,
            vk::IndexType::eUint32
        );
    }

    void Application::prepareCamera(Frame::Instance& outFrame)
    {
        glm::vec3 eyes   = { 1.0f, 0.0f, -1.0f };
        glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        glm::vec3 up     = { 0.0f, 0.0f, -1.0f };
        glm::mat4 view   = glm::lookAt(eyes, center, up);

        glm::mat4 projection = glm::perspective(
            glm::radians(25.0f),
            static_cast<float>(m_swapChain.extent.width) / static_cast<float>(m_swapChain.extent.height),
            0.1f,
            10.0f
        );

        // Normalize OpenGL's to coordinate system Vulkan
        projection[1][1] *= -1;

        outFrame.cameraData.object.view           = view;
        outFrame.cameraData.object.projection     = projection;
        outFrame.cameraData.object.viewProjection = projection * view;
    }

    void Application::prepareLevelActors(Frame::Instance& outFrame)
    {
        outFrame.updateModelTransforms(m_level.getActors());
    }

    void Application::prepareFrame(uint32_t inImageIndex)
    {
        Frame::Instance& frame = m_swapChain.frames[inImageIndex];

        prepareCamera(frame);
        memcpy(
            frame.cameraData.writeLocation,
            &frame.cameraData.object,
            frame.cameraData.allocationSize
        );

        prepareLevelActors(frame);
        memcpy(
            frame.modelData.writeLocation,
            frame.modelData.transforms.data(),
            frame.modelData.allocationSize
        );

        frame.setupDescriptorSet();
    }

    void Application::drawLevel(const vk::CommandBuffer& inCommandBuffer)
    {
        std::unordered_map<std::string, uint32_t> usedMeshes;

        for (Level::Actor::Instance actor : m_level.actors)
        {
            if (usedMeshes.find(actor.mesh.id) != usedMeshes.end())
            {
                usedMeshes[actor.mesh.id]++;

                continue;
            }

            usedMeshes.insert(std::make_pair(actor.mesh.id, 1));
        }

        std::vector<std::string> drawnMeshes;
        drawnMeshes.reserve(usedMeshes.size());

        for (Level::Actor::Instance actor : m_level.actors)
        {
            m_textureManager->bindTexture(
                actor.texture.id,
                inCommandBuffer,
                m_graphicsPipeline.layout
            );

            if (std::find(drawnMeshes.begin(), drawnMeshes.end(), actor.mesh.id) != drawnMeshes.end())
            {
                continue;
            }

            m_meshManager->drawMesh(
                actor.mesh.id,
                usedMeshes[actor.mesh.id],
                inCommandBuffer
            );

            drawnMeshes.push_back(actor.mesh.id);
        }
    }
}