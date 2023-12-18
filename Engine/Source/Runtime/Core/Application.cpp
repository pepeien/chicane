#include "Application.hpp"

vk::ClearValue clearColor = { std::array<float, 4>{ 0.04f, 0.89f, 0.84f, 1.0f } };

namespace Engine
{
    Application::Application(const std::string& inWindowTitle, const Scene::Instance& inScene)
    {
        windowTitle = inWindowTitle;
        scene       = inScene;

        // Stats
        numFrames   = 0;
        frameTime   = 0.0f;
        lastTime    = 0.0;
        currentTime = 0.0;

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
        buildDescriptorSetLayouts();
        buildGraphicsPipeline();
        buildFramebuffers();
        buildCommandPool();
        buildCommandBuffers();
        buildFrameResources();
        buildMeshResources();
        buildAssets();
    }

    Application::~Application()
    {
        // Vulkan
        logicalDevice.waitIdle();

        destroyDescriptorSetLayouts();
        destroyGraphicsPipeline();
        destroySwapChain();
        destroyCommandPool();
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
        while (glfwWindowShouldClose(window) == false)
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
        renderPassBeginInfo.renderPass          = graphicsPipeline.renderPass;
        renderPassBeginInfo.framebuffer         = swapChain.frames[inImageIndex].framebuffer;
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = swapChain.extent;
        renderPassBeginInfo.clearValueCount     = 1;
        renderPassBeginInfo.pClearValues        = &clearColor;

        inCommandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

        inCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            graphicsPipeline.layout,
            0,
            swapChain.frames[inImageIndex].descriptorSet,
            nullptr
        );

        inCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.instance);

        prepareScene(inCommandBuffer);

        //TODO Implement way to add textures for each object
        for (int i = 0; i < scene.objects.size(); i++)
        {
            scene.objects[i].texture->use(inCommandBuffer, graphicsPipeline.layout);
        }

        std::vector<Mesh::AllocationInfo> allocatorInfoList = meshManager->getAllocationInfoList();

        for (Mesh::AllocationInfo& allocationInfo : allocatorInfoList)
        {
            inCommandBuffer.draw(
                allocationInfo.vertexCount,
                allocationInfo.instanceCount,
                allocationInfo.firstVertex,
                allocationInfo.firstInstance
            );
        }

        inCommandBuffer.endRenderPass();

        inCommandBuffer.end();
    }

    void Application::render()
    {
        Frame::Instance& currentFrame = swapChain.frames[currentImageIndex];

        if (logicalDevice.waitForFences(1, &currentFrame.renderFence, VK_TRUE, UINT64_MAX) != vk::Result::eSuccess)
        {
            Log::warning("Error while waiting the fences");
        }

        if (logicalDevice.resetFences(1, &currentFrame.renderFence) != vk::Result::eSuccess)
        {
            Log::warning("Error while resetting the fences");
        }

        uint32_t imageIndex;

        try
        {
            imageIndex = logicalDevice.acquireNextImageKHR(
                swapChain.instance,
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

        vk::CommandBuffer commandBuffer = swapChain.frames[imageIndex].commandBuffer;

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

        graphicsQueue.submit(submitInfo, currentFrame.renderFence);

        vk::SwapchainKHR swapChains[] = { swapChain.instance };

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = swapChains;
        presentInfo.pImageIndices      = &imageIndex;

        vk::Result present;

        try
        {
            present = presentQueue.presentKHR(presentInfo);
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

        currentImageIndex = (currentImageIndex + 1) % maxInFlightFramesCount;
    }

    void Application::calculateFrameRate()
    {
        currentTime = glfwGetTime();
        double delta = currentTime - lastTime;

        if (delta >= 1) {
            int framerate{ std::max(1, int(numFrames / delta)) };

            std::stringstream title;
            title << windowTitle << " - " << framerate << " FPS";

            glfwSetWindowTitle(window, title.str().c_str());

            lastTime  = currentTime;
            numFrames = -1;
            frameTime = float(1000.0 / framerate);
        }

        ++numFrames;
    }

    void Application::buildWindow()
    {
        window = Window::init(
            windowWidth,
            windowHeight,
            windowTitle.c_str()
        );
    }

    void Application::buildInstance()
    {
        Instance::init(instance, dldi);
    }

    void Application::destroyInstance()
    {
        instance.destroy();
    }

    void Application::buildDebugMessenger()
    {
        if (IS_DEBUGGING == false)
        {
            return;
        }

        Debug::initMessenger(debugMessenger, instance, dldi);
    }

    void Application::destroyDebugMessenger()
    {
        instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
    }

    void Application::buildSurface()
    {
        Surface::init(surface, instance, window);
    }
                
    void Application::destroySurface()
    {
        instance.destroySurfaceKHR(surface);
    }

    void Application::buildQueues()
    {
        Queue::initGraphicsQueue(
            graphicsQueue,
            physicalDevice,
            logicalDevice,
            surface
        );

        Queue::initPresentQueue(
            presentQueue,
            physicalDevice,
            logicalDevice,
            surface
        );
    }

    void Application::buildDevices()
    {
        Device::pickPhysicalDevice(physicalDevice, instance);
        Device::initLogicalDevice(
            logicalDevice,
            physicalDevice,
            surface
        );
    }
                
    void Application::destroyDevices()
    {
        logicalDevice.destroy();
    }

    void Application::buildSwapChain()
    {
        SwapChain::init(
            swapChain,
            physicalDevice,
            logicalDevice,
            surface,
            windowWidth,
            windowHeight
        );

        maxInFlightFramesCount = static_cast<int>(swapChain.frames.size());
        currentImageIndex      = 0;
    }

    void Application::rebuildSwapChain()
    {
        windowWidth  = 0;
        windowHeight = 0;

        while (windowWidth == 0 || windowHeight == 0) {
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            glfwWaitEvents();
        }

        logicalDevice.waitIdle();

        destroySwapChain();
        buildSwapChain();

        destroyGraphicsPipeline();
        buildGraphicsPipeline();

        buildFramebuffers();

        buildFrameResources();
        buildMeshResources();

        buildCommandBuffers();
    }

    void Application::destroySwapChain()
    {
        for (Frame::Instance& frame : swapChain.frames)
        {
            logicalDevice.destroyImageView(frame.imageView);
            logicalDevice.destroyFramebuffer(frame.framebuffer);

            logicalDevice.destroyFence(frame.renderFence);
            logicalDevice.destroySemaphore(frame.presentSemaphore);
            logicalDevice.destroySemaphore(frame.renderSemaphore);

            logicalDevice.unmapMemory(frame.cameraData.buffer.memory);
            logicalDevice.freeMemory(frame.cameraData.buffer.memory);
            logicalDevice.destroyBuffer(frame.cameraData.buffer.instance);

            logicalDevice.unmapMemory(frame.modelData.buffer.memory);
            logicalDevice.freeMemory(frame.modelData.buffer.memory);
            logicalDevice.destroyBuffer(frame.modelData.buffer.instance);
        }

        logicalDevice.destroySwapchainKHR(swapChain.instance);

        destroyDescriptorPools();
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
            frameDescriptorSetLayout,
            logicalDevice,
            frameLayoutBidings
        );
    }

    void Application::buildMeshDescriptorSetLayout()
    {
        Descriptor::SetLayoutBidingsCreateInfo meshLayoutBidings;
        meshLayoutBidings.count = 1;

        // Texture
        meshLayoutBidings.indices.push_back(0);
        meshLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        meshLayoutBidings.counts.push_back(1);
        meshLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            meshDescriptorSetLayout,
            logicalDevice,
            meshLayoutBidings
        );
    }

    void Application::buildDescriptorSetLayouts()
    {
        buildFrameDescriptorSetLayout();
        buildMeshDescriptorSetLayout();
    }

    void Application::destroyDescriptorSetLayouts()
    {
        logicalDevice.destroyDescriptorSetLayout(frameDescriptorSetLayout);
        logicalDevice.destroyDescriptorSetLayout(meshDescriptorSetLayout);
    }

    void Application::destroyDescriptorPools()
    {
        logicalDevice.destroyDescriptorPool(frameDescriptorPool);
        logicalDevice.destroyDescriptorPool(meshDescriptorPool);
    }

    void Application::buildGraphicsPipeline()
    {
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.push_back(frameDescriptorSetLayout);
        descriptorSetLayouts.push_back(meshDescriptorSetLayout);
    
        GraphicsPipeline::CreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.logicalDevice        = logicalDevice;
        graphicsPipelineCreateInfo.vertexShaderName     = "triangle.vert.spv";
        graphicsPipelineCreateInfo.fragmentShaderName   = "triangle.frag.spv";
        graphicsPipelineCreateInfo.swapChainExtent      = swapChain.extent;
        graphicsPipelineCreateInfo.swapChainImageFormat = swapChain.format;
        graphicsPipelineCreateInfo.descriptorSetLayouts = descriptorSetLayouts;

        GraphicsPipeline::init(graphicsPipeline, graphicsPipelineCreateInfo);    
    }

    void Application::destroyGraphicsPipeline()
    {
        logicalDevice.destroyPipeline(graphicsPipeline.instance);
        logicalDevice.destroyPipelineLayout(graphicsPipeline.layout);
        logicalDevice.destroyRenderPass(graphicsPipeline.renderPass);
    }

    void Application::buildFramebuffers()
    {
        Frame::BufferCreateInfo framebufferCreateInfo = {
            logicalDevice,
            graphicsPipeline.renderPass,
            swapChain.extent,
            swapChain.frames
        };

        Frame::initBuffer(framebufferCreateInfo);
    }

    void Application::buildCommandPool()
    {
        Command::initPool(
            commandPool,
            logicalDevice,
            physicalDevice,
            surface
        );
    }

    void Application::destroyCommandPool()
    {
        logicalDevice.destroyCommandPool(commandPool);
    }

    void Application::buildCommandBuffers()
    {
        Command::BufferCreateInfo commandBufferInfo = {
            logicalDevice,
            commandPool,
            swapChain.frames
        };

        Command::initBuffers(mainCommandBuffer, commandBufferInfo);
    }

    void Application::buildFrameResources()
    {
        Descriptor::PoolCreateInfo poolCreateInfo;
        poolCreateInfo.count = 2;
        poolCreateInfo.size  = static_cast<uint32_t>(swapChain.frames.size());
        poolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        poolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Descriptor::initPool(
            frameDescriptorPool,
            logicalDevice,
            poolCreateInfo
        );

        for (Frame::Instance& frame : swapChain.frames)
        {
            Sync::initFence(frame.renderFence, logicalDevice);
            Sync::initSempahore(frame.presentSemaphore, logicalDevice);
            Sync::initSempahore(frame.renderSemaphore, logicalDevice);
           
            frame.initResources(logicalDevice, physicalDevice, scene);

            Descriptor::initSet(
                frame.descriptorSet,
                logicalDevice,
                frameDescriptorSetLayout,
                frameDescriptorPool
            );
        }
    }

    void Application::buildMeshResources()
    {
        Descriptor::PoolCreateInfo poolCreateInfo;
        poolCreateInfo.count = 1;
        poolCreateInfo.size  = 1; // Due to the usage of one default texture, should be texture.count
        poolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            meshDescriptorPool,
            logicalDevice,
            poolCreateInfo
        );
    }

    void Application::buildAssets()
    {
        Mesh::ManagerCreateInfo meshCreationInfo;
        meshCreationInfo.logicalDevice  = logicalDevice;
        meshCreationInfo.physicalDevice = physicalDevice;
        meshCreationInfo.queue          = graphicsQueue;
        meshCreationInfo.commandBuffer  = mainCommandBuffer;

        meshManager = new Mesh::Manager(meshCreationInfo);

        for (auto sceneObject : scene.getObjects())
        {
            meshManager->addMesh(sceneObject.vertices);
        }

        meshManager->proccess();

        //TODO Implement way to add textures for each object
        Texture::CreateInfo createInfo;
        createInfo.width               = 512;
        createInfo.height              = 512;
        createInfo.filename            = "gray.png";
        createInfo.logicalDevice       = logicalDevice;
        createInfo.physicalDevice      = physicalDevice;
        createInfo.commandBuffer       = mainCommandBuffer;
        createInfo.queue               = graphicsQueue;
        createInfo.descriptorSetLayout = meshDescriptorSetLayout;
        createInfo.descriptorPool      = meshDescriptorPool;

        defaultTexture = new Texture::Instance(createInfo);

        for (int i = 0; i < scene.objects.size(); i++)
        {
            scene.objects[i].texture = defaultTexture;
        }
    }

    void Application::destroyAssets()
    {
        delete meshManager;

        //TODO Implement way to add textures for each object
        delete defaultTexture;
    }

    void Application::prepareScene(const vk::CommandBuffer& inCommandBuffer)
    {
        vk::Buffer vertexBuffers[] = { meshManager->vertexBuffer.instance };
        vk::DeviceSize offsets[]   = { 0 };

        inCommandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    }

    void Application::prepareCamera(Frame::Instance& outFrame)
    {
        glm::vec3 eyes   = { 1.0f, 0.0f, -1.0f };
        glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        glm::vec3 up     = { 0.0f, 0.0f, -1.0f };
        glm::mat4 view   = glm::lookAt(eyes, center, up);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(swapChain.extent.width) / static_cast<float>(swapChain.extent.height),
            0.1f,
            10.0f
        );
        // Normalize OpenGL's to coordinate system Vulkan
        projection[1][1] *= -1;

        outFrame.cameraData.object.view           = view;
        outFrame.cameraData.object.projection     = projection;
        outFrame.cameraData.object.viewProjection = projection * view;
    }

    void Application::prepareModel(Frame::Instance& outFrame)
    {
        auto sceneObjects = scene.getObjects();
            
        for (uint32_t i = 0; i < sceneObjects.size(); i++)
        {
            auto& sceneObject = sceneObjects[i];

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), sceneObject.transform.translation);
            transform           = glm::scale(transform, sceneObject.transform.scale);

            outFrame.modelData.transforms[i] = transform;
        }
    }

    void Application::prepareFrame(const uint32_t& inImageIndex)
    {
        Frame::Instance& frame = swapChain.frames[inImageIndex];

        prepareCamera(frame);
        memcpy(
            frame.cameraData.writeLocation,
            &frame.cameraData.object,
            frame.cameraData.allocationSize
        );

        prepareModel(frame);
        memcpy(
            frame.modelData.writeLocation,
            frame.modelData.transforms.data(),
            frame.modelData.allocationSize
        );

        frame.writeDescriptorSet(logicalDevice);
    }
}