#include "Application.hpp"

vk::ClearValue s_clearColor = { std::array<float, 4>{ 0.04f, 0.89f, 0.84f, 1.0f } };

namespace Chicane
{
    Application::Application(const std::string& inWindowTitle, const Scene::Instance& inScene)
    : m_window({ nullptr, inWindowTitle, 0, 0 }),
      m_scene(inScene),
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

    void Application::draw(const vk::CommandBuffer& inCommandBuffer, const uint32_t& inImageIndex)
    {
        vk::CommandBufferBeginInfo beginInfo = {};

        inCommandBuffer.begin(beginInfo);

        vk::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline.renderPass;
        renderPassBeginInfo.framebuffer         = m_swapChain.frames[inImageIndex].framebuffer;
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = m_swapChain.extent;
        renderPassBeginInfo.clearValueCount     = 1;
        renderPassBeginInfo.pClearValues        = &s_clearColor;

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

        prepareScene(inCommandBuffer);

        drawObjects(inCommandBuffer);

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
            m_logicalDevice.destroyImageView(frame.imageView);
            m_logicalDevice.destroyFramebuffer(frame.framebuffer);

            m_logicalDevice.destroyFence(frame.renderFence);
            m_logicalDevice.destroySemaphore(frame.presentSemaphore);
            m_logicalDevice.destroySemaphore(frame.renderSemaphore);

            m_logicalDevice.unmapMemory(frame.cameraData.buffer.memory);
            Buffer::destroy(m_logicalDevice, frame.cameraData.buffer);

            m_logicalDevice.unmapMemory(frame.modelData.buffer.memory);
            Buffer::destroy(m_logicalDevice, frame.modelData.buffer);
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
        Command::Pool::init(
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
        Command::Buffer::CreateInfo createInfo = {
            m_logicalDevice,
            m_mainCommandPool
        };

        Command::Buffer::init(m_mainCommandBuffer, createInfo);
    }

    void Application::buildFramesCommandBuffers()
    {
        Command::Buffer::CreateInfo createInfo = {
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
            Sync::initSempahore(frame.presentSemaphore, m_logicalDevice);
            Sync::initSempahore(frame.renderSemaphore, m_logicalDevice);
            Sync::initFence(frame.renderFence, m_logicalDevice);
           
            frame.initResources(
                m_logicalDevice,
                m_physicalDevice,
                m_scene
            );

            Descriptor::initSet(
                frame.descriptorSet,
                m_logicalDevice,
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
        std::vector<Vertex::Instance> triangleVertices;
        triangleVertices.resize(3);

        triangleVertices[0].position        = glm::vec2(0.0f, -1.0f);
        triangleVertices[0].color           = glm::vec3(1.0f, 0.0f, 0.0f);
        triangleVertices[0].texturePosition = glm::vec2(0.5f, 0.0f);

        triangleVertices[1].position        = glm::vec2(1.0f, 1.0f);
        triangleVertices[1].color           = glm::vec3(1.0f, 0.0f, 0.0f);
        triangleVertices[1].texturePosition = glm::vec2(1.0f, 1.0f);

        triangleVertices[2].position        = glm::vec2(-1.0f, 1.0f);
        triangleVertices[2].color           = glm::vec3(1.0f, 0.0f, 0.0f);
        triangleVertices[2].texturePosition = glm::vec2(0.0f, 1.0f);

        m_meshManager->addMesh(
            "Triangle",
            triangleVertices
        );

        std::vector<Vertex::Instance> squareVertices;
        squareVertices.resize(6);

        squareVertices[0].position        = glm::vec2(-0.5f, 1.0f);
        squareVertices[0].color           = glm::vec3(0.0f, 0.0f, 1.0f);
        squareVertices[0].texturePosition = glm::vec2(0.0f, 1.0f);

        squareVertices[1].position        = glm::vec2(-0.5f, -1.0f);
        squareVertices[1].color           = glm::vec3(0.0f, 0.0f, 1.0f);
        squareVertices[1].texturePosition = glm::vec2(0.0f, 0.0f);

        squareVertices[2].position        = glm::vec2(0.5f, -1.0f);
        squareVertices[2].color           = glm::vec3(0.0f, 0.0f, 1.0f);
        squareVertices[2].texturePosition = glm::vec2(1.0f, 0.0f);

        squareVertices[3].position        = glm::vec2(0.5f, -1.0f);
        squareVertices[3].color           = glm::vec3(0.0f, 0.0f, 1.0f);
        squareVertices[3].texturePosition = glm::vec2(1.0f, 0.0f);

        squareVertices[4].position        = glm::vec2(0.5f, 1.0f);
        squareVertices[4].color           = glm::vec3(0.0f, 0.0f, 1.0f);
        squareVertices[4].texturePosition = glm::vec2(1.0f, 1.0f);

        squareVertices[5].position        = glm::vec2(-0.5f, 1.0f);
        squareVertices[5].color           = glm::vec3(0.0f, 0.0f, 1.0f);
        squareVertices[5].texturePosition = glm::vec2(0.0f, 1.0f);

        m_meshManager->addMesh(
            "Square",
            squareVertices
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
        grayTextureData.filename = "gray.png";

        m_textureManager->addTexture("Gray", grayTextureData);

        Texture::Data gridTextureData;
        gridTextureData.width    = 512;
        gridTextureData.height   = 512;
        gridTextureData.filename = "grid.png";

        m_textureManager->addTexture("Grid", gridTextureData);

        Texture::Data uvTextureData;
        uvTextureData.width    = 512;
        uvTextureData.height   = 512;
        uvTextureData.filename = "uv.png";

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

    void Application::prepareScene(const vk::CommandBuffer& inCommandBuffer)
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
            glm::radians(45.0f),
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

    void Application::prepareSceneObjects(Frame::Instance& outFrame)
    {
        std::vector<Scene::Object::Instance> sceneObjects = m_scene.getObjects();
            
        for (uint32_t i = 0; i < sceneObjects.size(); i++)
        {
            Scene::Object::Instance& sceneObject = sceneObjects[i];

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), sceneObject.transform.translation);
            transform           = glm::scale(transform, sceneObject.transform.scale);

            outFrame.modelData.transforms[i] = transform;
        }
    }

    void Application::prepareFrame(const uint32_t& inImageIndex)
    {
        Frame::Instance& frame = m_swapChain.frames[inImageIndex];

        prepareCamera(frame);
        memcpy(
            frame.cameraData.writeLocation,
            &frame.cameraData.object,
            frame.cameraData.allocationSize
        );

        prepareSceneObjects(frame);
        memcpy(
            frame.modelData.writeLocation,
            frame.modelData.transforms.data(),
            frame.modelData.allocationSize
        );

        frame.writeDescriptorSet(m_logicalDevice);
    }

    void Application::drawObjects(const vk::CommandBuffer& inCommandBuffer)
    {
        std::unordered_map<std::string, uint32_t> usedMeshes;

        for (Scene::Object::Instance sceneObject : m_scene.objects)
        {
            if (usedMeshes.find(sceneObject.mesh.id) != usedMeshes.end())
            {
                usedMeshes[sceneObject.mesh.id]++;

                continue;
            }

            usedMeshes.insert(std::make_pair(sceneObject.mesh.id, 1));
        }

        std::vector<std::string> drawnMeshes;
        drawnMeshes.reserve(usedMeshes.size());

        for (Scene::Object::Instance sceneObject : m_scene.objects)
        {
            m_textureManager->bindTexture(
                sceneObject.texture.id,
                inCommandBuffer,
                m_graphicsPipeline.layout
            );

            if (std::find(drawnMeshes.begin(), drawnMeshes.end(), sceneObject.mesh.id) != drawnMeshes.end())
            {
                continue;
            }

            m_meshManager->drawMesh(
                sceneObject.mesh.id,
                usedMeshes[sceneObject.mesh.id],
                inCommandBuffer
            );

            drawnMeshes.push_back(sceneObject.mesh.id);
        }
    }
}