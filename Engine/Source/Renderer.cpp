#include "Renderer.hpp"

#include "Core.hpp"
#include "Game.hpp"

namespace Engine
{
    Renderer::Renderer(
        Window* inWindow,
        Level* inLevel
    )
        : m_frameStats({}),
        m_swapChain({}),
        m_maxInFlightImageCount(0),
        m_currentImageIndex(0),
        m_meshVertexBuffer({}),
        m_meshIndexBuffer({}),
        m_cubeMapManager(std::make_unique<CubeMap::Manager::Instance>()),
        m_modelManager(std::make_unique<Model::Manager::Instance>()),
        m_textureManager(std::make_unique<Texture::Manager::Instance>()),
        m_level(inLevel),
        m_camera(std::make_unique<Camera::Instance>()),
        m_window(inWindow)
    {
        // Assets pre load
        includeAssets();

        // Vulkan
        initializeDescriptors();

        buildInstance();
        buildDebugMessenger();
        buildSurface();
        buildDevices();
        buildQueues();
        buildSwapChain();
        buildDescriptorSetLayouts();
        buildGraphicsPipelines();
        buildFramebuffers();
        buildCommandPool();
        buildMainCommandBuffer();
        buildFramesCommandBuffers();
        buildFrameResources();
        buildMaterialResources();

        // Assets building
        buildAssets();
    }

    Renderer::~Renderer()
    {
        // Vulkan
        m_logicalDevice.waitIdle();

        destroyCommandPool();
        destroyGraphicsPipelines();
        destroySwapChain();

        m_camera.reset();

        for (auto [type, instance] : m_frameDescriptors)
        {
            m_logicalDevice.destroyDescriptorSetLayout(
                instance.setLayout
            );
        }

        for (auto [type, instance] : m_materialDescriptors)
        {
            m_logicalDevice.destroyDescriptorSetLayout(
                instance.setLayout
            );
            m_logicalDevice.destroyDescriptorPool(
                instance.pool
            );
        }

        Buffer::destroy(
            m_logicalDevice,
            m_meshVertexBuffer
        );
        Buffer::destroy(
            m_logicalDevice,
            m_meshIndexBuffer
        );

        destroyAssets();
        destroyDevices();
        destroySurface();

        if (IS_DEBUGGING)
        {
            destroyDebugMessenger();
        }

        destroyInstance();
    }

    void Renderer::onEvent(const SDL_Event& inEvent)
    {
        switch (inEvent.type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            onKeyboardEvent(inEvent.key);

            break;
    
        case SDL_MOUSEMOTION:
            onMouseMotionEvent(inEvent.motion);

            break;

        default:
            break;
        }
    }

    void Renderer::render()
    {
        m_logicalDevice.waitIdle();

        Frame::Instance& currentFrame = m_swapChain.images[m_currentImageIndex];

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

        vk::ResultValue<uint32_t> acquireResult = m_logicalDevice.acquireNextImageKHR(
            m_swapChain.instance,
            UINT64_MAX,
            currentFrame.presentSemaphore,
            nullptr
        );

        if (acquireResult.result == vk::Result::eErrorOutOfDateKHR)
        {
            rebuildSwapChain();
    
            return;
        }
    
        if (acquireResult.result != vk::Result::eSuccess && acquireResult.result != vk::Result::eSuboptimalKHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image");
        }
        
        uint32_t imageIndex = acquireResult.value;

        if (
            m_logicalDevice.resetFences(
                1,
                &currentFrame.renderFence
            ) != vk::Result::eSuccess
        )
        {
            LOG_WARNING("Error while resetting the fences");
        }
    
        vk::CommandBuffer& commandBuffer = currentFrame.commandBuffer;

        commandBuffer.reset();

        prepareFrame(m_swapChain.images[imageIndex]);

        vk::CommandBufferBeginInfo beginInfo = {};
        commandBuffer.begin(beginInfo);

        drawSky(commandBuffer,   imageIndex);
        drawScene(commandBuffer, imageIndex);

        commandBuffer.end();

        vk::Semaphore waitSemaphores[]      = { currentFrame.presentSemaphore };
        vk::Semaphore signalSemaphores[]    = { currentFrame.renderSemaphore };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        vk::SubmitInfo submitInfo = {};
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;
        submitInfo.pWaitDstStageMask    = waitStages;

        m_graphicsQueue.submit(
            submitInfo,
            currentFrame.renderFence
        );

        vk::SwapchainKHR swapChains[] = { m_swapChain.instance };

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = swapChains;
        presentInfo.pImageIndices      = &imageIndex;

        vk::Result presentResult = m_presentQueue.presentKHR(presentInfo);

        if (
            presentResult == vk::Result::eErrorOutOfDateKHR ||
            presentResult == vk::Result::eSuboptimalKHR
        )
        {
            rebuildSwapChain();

            return;
        }

        m_currentImageIndex = (m_currentImageIndex + 1) % m_maxInFlightImageCount;
    }

    void Renderer::updateStats()
    {
        m_frameStats.currentTime = SDL_GetTicks64() / 1000;

        double delta = m_frameStats.currentTime - m_frameStats.lastTime;

        if (delta >= 1) {
            m_frameStats.lastTime  = m_frameStats.currentTime;
            m_frameStats.count     = -1;
            m_frameStats.time      = float(1000.0 / std::max(1, int(m_frameStats.count / delta)));
        }

        m_frameStats.count++;
    }

    void Renderer::updateViewport(const vk::CommandBuffer& inCommandBuffer)
    {
        vk::Viewport viewport = GraphicsPipeline::createViewport(m_swapChain.extent);
        inCommandBuffer.setViewport(0, 1, &viewport);

        vk::Rect2D scissor = GraphicsPipeline::createScissor(m_swapChain.extent);
        inCommandBuffer.setScissor(0, 1, &scissor);
    }

    void Renderer::prepareSky(
        const vk::CommandBuffer& inCommandBuffer,
        uint32_t inImageIndex
    )
    {
        updateViewport(inCommandBuffer);

        inCommandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            m_graphicPipelines.at(GraphicsPipeline::Type::SKY)->instance
        );

        inCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_graphicPipelines.at(
                GraphicsPipeline::Type::SKY
            )->layout,
            0,
            m_swapChain.images[inImageIndex].descriptorSets.at(
                GraphicsPipeline::Type::SKY
            ),
            nullptr
        );

        m_cubeMapManager->bind(
            "Sky",
            inCommandBuffer,
            m_graphicPipelines.at(
                GraphicsPipeline::Type::SKY
            )->layout
        );
    }

    void Renderer::drawSky(
        const vk::CommandBuffer& inCommandBuffer,
        uint32_t inImageIndex
    )
    {
        vk::ClearValue clearColor;
        clearColor.color = vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f);

        std::vector<vk::ClearValue> clearValues = {{ clearColor }};

        vk::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass          = m_graphicPipelines.at(GraphicsPipeline::Type::SKY)->renderPass;
        renderPassBeginInfo.framebuffer         = m_swapChain.images[inImageIndex].framebuffers.at(GraphicsPipeline::Type::SKY);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = m_swapChain.extent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues        = clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );

        prepareSky(inCommandBuffer, inImageIndex);

        m_cubeMapManager->draw(
            "Sky",
            inCommandBuffer
        );

        inCommandBuffer.endRenderPass();
    }

    void Renderer::prepareScene(
        const vk::CommandBuffer& inCommandBuffer,
        uint32_t inImageIndex
    )
    {
        updateViewport(inCommandBuffer);

        inCommandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->instance
        );

        inCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_graphicPipelines.at(
                GraphicsPipeline::Type::SCENE
            )->layout,
            0,
            m_swapChain.images[inImageIndex].descriptorSets.at(
                GraphicsPipeline::Type::SCENE
            ),
            nullptr
        );

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

        // TODO Change to bind all used
        m_textureManager->bind(
            "grid",
            inCommandBuffer,
            m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->layout
        );
    }

    void Renderer::drawScene(
        const vk::CommandBuffer& inCommandBuffer,
        uint32_t inImageIndex
    )
    {
        vk::ClearValue clearColor;
        clearColor.color = vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f);

        vk::ClearValue clearDepth;
        clearDepth.depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

        std::vector<vk::ClearValue> clearValues = {{ clearColor, clearDepth }};

        vk::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass          = m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->renderPass;
        renderPassBeginInfo.framebuffer         = m_swapChain.images[inImageIndex].framebuffers.at(GraphicsPipeline::Type::SCENE);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = m_swapChain.extent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues        = clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );

        prepareScene(inCommandBuffer, inImageIndex);

        m_modelManager->draw(
            "floor",
            inCommandBuffer,
            1,
            0
        );

        inCommandBuffer.endRenderPass();
    }

    void Renderer::onKeyboardEvent(const SDL_KeyboardEvent& inEvent)
    {
        if (!m_window->isFocused())
        {
            return;
        }

        float cameraStepSize = 12.25;

        glm::vec3 nextPosition = m_camera->getPosition();

        switch (inEvent.keysym.scancode)
        {
        case SDL_SCANCODE_W:
            nextPosition.x += cameraStepSize;

            break;

        case SDL_SCANCODE_A:
            nextPosition.y += cameraStepSize;

            break;

        case SDL_SCANCODE_S:
            nextPosition.x -= cameraStepSize;   

            break;    

        case SDL_SCANCODE_D:
            nextPosition.y -= cameraStepSize;

            break;

        default:
            return;
        }

        m_camera->setPosition(nextPosition);
    }

    void Renderer::onMouseMotionEvent(const SDL_MouseMotionEvent& inEvent)
    {
        if (!m_window->isFocused())
        {
            return;
        }

        float castedMidWidth = (float) m_swapChain.midPoints.width;
        float yawDiff = castedMidWidth - ((float) inEvent.x);
        yawDiff = std::clamp(
            yawDiff,
            -castedMidWidth,
            castedMidWidth
        );
        float yaw = yawDiff / castedMidWidth;

        float castedMidHeight = (float) m_swapChain.midPoints.height;
        float pitchDiff = castedMidHeight - ((float) inEvent.y);
        pitchDiff = std::clamp(
            pitchDiff,
            -castedMidHeight,
            castedMidHeight
        );
        float pitch = pitchDiff / castedMidHeight;

        m_camera->addYaw(yaw);
        m_camera->addPitch(pitch);
    }

    void Renderer::buildInstance()
    {
        Instance::init(
            m_instance,
            m_dldi,
            m_window->instance
        );
    }

    void Renderer::destroyInstance()
    {
        m_instance.destroy();
    }

    void Renderer::buildDebugMessenger()
    {
        if (IS_DEBUGGING == false)
        {
            return;
        }

        Debug::initMessenger(
            m_debugMessenger,
            m_instance,
            m_dldi
        );
    }

    void Renderer::destroyDebugMessenger()
    {
        m_instance.destroyDebugUtilsMessengerEXT(
            m_debugMessenger,
            nullptr,
            m_dldi
        );
    }

    void Renderer::buildSurface()
    {
        Surface::init(
            m_surface,
            m_instance,
            m_window->instance
        );
    }

    void Renderer::destroySurface()
    {
        m_instance.destroySurfaceKHR(m_surface);
    }

    void Renderer::buildQueues()
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

    void Renderer::buildDevices()
    {
        Device::pickPhysicalDevice(
            m_physicalDevice,
            m_instance
        );
        Device::initLogicalDevice(
            m_logicalDevice,
            m_physicalDevice,
            m_surface
        );
    }
                
    void Renderer::destroyDevices()
    {
        m_logicalDevice.destroy();
    }

    void Renderer::buildSwapChain()
    {
        SwapChain::init(
            m_swapChain,
            m_physicalDevice,
            m_logicalDevice,
            m_surface,
            m_window->getResolution()
        );

        m_camera->setResolution(
            m_swapChain.extent.width,
            m_swapChain.extent.height
        );

        m_maxInFlightImageCount = static_cast<int>(m_swapChain.images.size());
    }

    void Renderer::rebuildSwapChain()
    {
        if (m_window->isMinimized())
        {
            return;
        }

        m_logicalDevice.waitIdle();

        destroySwapChain();
        buildSwapChain();

        buildFramebuffers();
        buildFrameResources();
        buildFramesCommandBuffers();
    }

    void Renderer::destroySwapChain()
    {
        for (Frame::Instance& frame : m_swapChain.images)
        {
            frame.destroy();
        }

        m_logicalDevice.destroySwapchainKHR(m_swapChain.instance);

        m_swapChain.images.clear();

        for (auto& [type, instance] : m_frameDescriptors)
        {
            m_logicalDevice.destroyDescriptorPool(
                instance.pool
            );
        }
    }

    void Renderer::initializeDescriptors()
    {
        Descriptor::Bundle defaultDescriptor {};

        // Sky
        m_frameDescriptors.insert(
            std::make_pair(
                GraphicsPipeline::Type::SKY,
                defaultDescriptor
            )
        );
        m_materialDescriptors.insert(
            std::make_pair(
                GraphicsPipeline::Type::SKY,
                defaultDescriptor
            )
        );

        // Scene
        m_frameDescriptors.insert(
            std::make_pair(
                GraphicsPipeline::Type::SCENE,
                defaultDescriptor
            )
        );
        m_materialDescriptors.insert(
            std::make_pair(
                GraphicsPipeline::Type::SCENE,
                defaultDescriptor
            )
        );
    }

    void Renderer::buildSkyDescriptorSetLayouts()
    {
        // Frame
        Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 1;

        /// Camera
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Descriptor::initSetLayout(
            m_frameDescriptors.at(GraphicsPipeline::Type::SKY).setLayout,
            m_logicalDevice,
            frameLayoutBidings
        );

        // Material
        Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;

        /// Texture
        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(1);
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            m_materialDescriptors.at(GraphicsPipeline::Type::SKY).setLayout,
            m_logicalDevice,
            materialLayoutBidings
        );
    }

    void Renderer::buildSceneDescriptorSetLayouts()
    {
        // Frame
        Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 2;

        /// Camera
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        /// Model
        frameLayoutBidings.indices.push_back(1);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eStorageBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Descriptor::initSetLayout(
            m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
            m_logicalDevice,
            frameLayoutBidings
        );

        // Material
        Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;

        /// Texture
        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(1);
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
            m_logicalDevice,
            materialLayoutBidings
        );
    }

    void Renderer::buildDescriptorSetLayouts()
    {
        buildSkyDescriptorSetLayouts();
        buildSceneDescriptorSetLayouts();
    }

    void Renderer::buildSkyGraphicsPipeline()
    {
        GraphicsPipeline::CreateInfo createInfo = {};
        createInfo.canOverwrite         = false;
        createInfo.hasVertices          = false;
        createInfo.hasDepth             = false;
        createInfo.logicalDevice        = m_logicalDevice;
        createInfo.vertexShaderPath     = FileSystem::Paths::contentDir() + "Shaders/sky.vert.spv";
        createInfo.fragmentShaderPath   = FileSystem::Paths::contentDir() + "Shaders/sky.frag.spv";
        createInfo.swapChainExtent      = m_swapChain.extent;
        createInfo.swapChainImageFormat = m_swapChain.format;
        createInfo.descriptorSetLayouts = {
            m_frameDescriptors.at(GraphicsPipeline::Type::SKY).setLayout,
            m_materialDescriptors.at(GraphicsPipeline::Type::SKY).setLayout
        };

        m_graphicPipelines.insert(
            std::make_pair(
                GraphicsPipeline::Type::SKY,
                std::make_unique<GraphicsPipeline::Instance>(createInfo)
            )
        );
    }

    void Renderer::buildSceneGraphicsPipeline()
    {
        GraphicsPipeline::CreateInfo createInfo = {};
        createInfo.canOverwrite          = true;
        createInfo.hasVertices           = true;
        createInfo.hasDepth              = true;
        createInfo.logicalDevice         = m_logicalDevice;
        createInfo.vertexShaderPath      = FileSystem::Paths::contentDir() + "Shaders/triangle.vert.spv";
        createInfo.fragmentShaderPath    = FileSystem::Paths::contentDir() + "Shaders/triangle.frag.spv";
        createInfo.bindingDescription    = Vertex::getBindingDescription();
        createInfo.attributeDescriptions = Vertex::getAttributeDescriptions();
        createInfo.swapChainExtent       = m_swapChain.extent;
        createInfo.swapChainImageFormat  = m_swapChain.format;
        createInfo.depthFormat           = m_swapChain.images[0].depthFormat;
        createInfo.descriptorSetLayouts  = {
            m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
            m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout
        };

        m_graphicPipelines.insert(
            std::make_pair(
                GraphicsPipeline::Type::SCENE,
                std::make_unique<GraphicsPipeline::Instance>(createInfo)
            )
        );
    }

    void Renderer::buildGraphicsPipelines()
    {
        buildSkyGraphicsPipeline();
        buildSceneGraphicsPipeline();
    }

    void Renderer::destroyGraphicsPipelines()
    {
        for (auto& [type, instance] : m_graphicPipelines)
        {
            instance.reset();
        }
    }

    void Renderer::buildFramebuffers()
    {
        std::unordered_map<GraphicsPipeline::Type, vk::RenderPass> renderPasses;
        renderPasses.insert(
            std::make_pair(
                GraphicsPipeline::Type::SKY,
                m_graphicPipelines.at(GraphicsPipeline::Type::SKY)->renderPass
            )
        );
        renderPasses.insert(
            std::make_pair(
                GraphicsPipeline::Type::SCENE,
                m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->renderPass
            )
        );

        Frame::Buffer::CreateInfo createInfo = {
            m_logicalDevice,
            renderPasses,
            m_swapChain.extent
        };

        Frame::Buffer::init(m_swapChain.images, createInfo);
    }

    void Renderer::buildCommandPool()
    {
        CommandBuffer::Pool::init(
            m_mainCommandPool,
            m_logicalDevice,
            m_physicalDevice,
            m_surface
        );
    }

    void Renderer::destroyCommandPool()
    {
        m_logicalDevice.destroyCommandPool(m_mainCommandPool);
    }

    void Renderer::buildMainCommandBuffer()
    {
        CommandBuffer::Instance::CreateInfo createInfo = {
            m_logicalDevice,
            m_mainCommandPool
        };

        CommandBuffer::Instance::init(
            m_mainCommandBuffer,
            createInfo
        );
    }

    void Renderer::buildFramesCommandBuffers()
    {
        CommandBuffer::Instance::CreateInfo createInfo = {
            m_logicalDevice,
            m_mainCommandPool
        };

        Frame::Buffer::initCommand(
            m_swapChain.images,
            createInfo
        );
    }

    void Renderer::prepareCamera(Frame::Instance& outFrame)
    {
        outFrame.cameraVectorUBO.instance = m_camera->getVectorUBO();
        memcpy(
            outFrame.cameraVectorUBO.writeLocation,
            &outFrame.cameraVectorUBO.instance,
            outFrame.cameraVectorUBO.allocationSize
        );

        outFrame.cameraMatrixUBO.instance = m_camera->getMatrixUBO();
        memcpy(
            outFrame.cameraMatrixUBO.writeLocation,
            &outFrame.cameraMatrixUBO.instance,
            outFrame.cameraMatrixUBO.allocationSize
        );
    }

    void Renderer::prepareActors(Frame::Instance& outFrame)
    {
        memcpy(
            outFrame.modelData.writeLocation,
            outFrame.modelData.transforms.data(),
            outFrame.modelData.allocationSize
        );
    }

    void Renderer::prepareFrame(Frame::Instance& outFrame)
    {
        prepareCamera(outFrame);
        prepareActors(outFrame);

        outFrame.updateDescriptorSets();
    }

    void Renderer::buildFrameResources()
    {
        // Sky
        Descriptor::PoolCreateInfo skyPoolCreateInfo;
        skyPoolCreateInfo.count = 2;
        skyPoolCreateInfo.size  = static_cast<uint32_t>(m_swapChain.images.size());
        skyPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        skyPoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Descriptor::initPool(
            m_frameDescriptors.at(GraphicsPipeline::Type::SKY).pool,
            m_logicalDevice,
            skyPoolCreateInfo
        );

        // Scene
        Descriptor::PoolCreateInfo scenePoolCreateInfo;
        scenePoolCreateInfo.count = 2;
        scenePoolCreateInfo.size  = static_cast<uint32_t>(m_swapChain.images.size());
        scenePoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        scenePoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Descriptor::initPool(
            m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).pool,
            m_logicalDevice,
            scenePoolCreateInfo
        );

        for (Frame::Instance& frame : m_swapChain.images)
        {
            // Sync
            frame.setupSync();

            // Data
            frame.setupCameraVectorUBO();
            frame.setupCameraMatrixUBO();
            frame.setupModelData(m_level->getActors());

            // Sets
            frame.addDescriptorSet(
                GraphicsPipeline::Type::SKY,
                m_frameDescriptors.at(GraphicsPipeline::Type::SKY).setLayout,
                m_frameDescriptors.at(GraphicsPipeline::Type::SKY).pool
            );
            frame.addDescriptorSet(
                GraphicsPipeline::Type::SCENE,
                m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
                m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).pool
            );
            frame.setupDescriptorSets();
        }
    }

    void Renderer::buildMaterialResources()
    {
        // Sky
        Descriptor::PoolCreateInfo skyPoolCreateInfo;
        skyPoolCreateInfo.count = 1;
        skyPoolCreateInfo.size  = 1;
        skyPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_materialDescriptors.at(GraphicsPipeline::Type::SKY).pool,
            m_logicalDevice,
            skyPoolCreateInfo
        );

        // Scene
        Descriptor::PoolCreateInfo scenePoolCreateInfo;
        scenePoolCreateInfo.count = 1;
        scenePoolCreateInfo.size  = m_textureManager->getCount();
        scenePoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).pool,
            m_logicalDevice,
            scenePoolCreateInfo
        );
    }

    void Renderer::includeCubeMaps()
    {
        CubeMap::Data skyCubeMapData;
        skyCubeMapData.filepaths = {
            FileSystem::Paths::contentDir() + "Textures/SkyBox/Sunset/front.tga",
            FileSystem::Paths::contentDir() + "Textures/SkyBox/Sunset/back.tga",
            FileSystem::Paths::contentDir() + "Textures/SkyBox/Sunset/left.tga",
            FileSystem::Paths::contentDir() + "Textures/SkyBox/Sunset/right.tga",
            FileSystem::Paths::contentDir() + "Textures/SkyBox/Sunset/up.tga",
            FileSystem::Paths::contentDir() + "Textures/SkyBox/Sunset/down.tga"
        };

        m_cubeMapManager->add(
            "Sky",
            skyCubeMapData
        );
    }

    void Renderer::buildCubeMaps()
    {
        m_cubeMapManager->build(
            m_logicalDevice,
            m_physicalDevice,
            m_mainCommandBuffer,
            m_graphicsQueue,
            m_materialDescriptors.at(
                GraphicsPipeline::Type::SKY
            ).setLayout,
            m_materialDescriptors.at(
                GraphicsPipeline::Type::SKY
            ).pool
        );
    }

    void Renderer::includeModels()
    {
        m_modelManager->add(
            "floor",
            FileSystem::Paths::contentDir() + "Models/floor.obj",
            Model::Vendor::Wavefront
        );
    }

    void Renderer::buildModels()
    {
        m_modelManager->build(
            m_meshVertexBuffer,
            m_meshIndexBuffer,
            m_logicalDevice,
            m_physicalDevice,
            m_graphicsQueue,
            m_mainCommandBuffer
        );
    }

    void Renderer::includeTextures()
    {
        Texture::Data gridTextureData;
        gridTextureData.filepath = FileSystem::Paths::contentDir() + "Textures/Base/grid.png";

        m_textureManager->add("grid", gridTextureData);
    }

    void Renderer::buildTextures()
    {
        m_textureManager->build(
            m_logicalDevice,
            m_physicalDevice,
            m_mainCommandBuffer,
            m_graphicsQueue,
            m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
            m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).pool
        );
    }

    void Renderer::includeAssets()
    {
        includeCubeMaps();
        includeModels();
        includeTextures();
    }

    void Renderer::buildAssets()
    {
        buildCubeMaps();
        buildModels();
        buildTextures();
    }

    void Renderer::destroyAssets()
    {
        m_cubeMapManager.reset();
        m_modelManager.reset();
        m_textureManager.reset();
    }
}