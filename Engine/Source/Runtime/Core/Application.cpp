#include "Application.hpp"

vk::ClearValue clearColor = { std::array<float, 4>{ 0.04f, 0.89f, 0.84f, 1.0f } };

namespace Chicane
{
    Application::Application(const std::string& inWindowTitle, const Scene::Instance& inScene)
    {
        windowTitle = inWindowTitle;
        scene       = inScene;
        LOG_INFO("huh");
        // Stats
        numFrames   = 0;
        frameTime   = 0.0f;
        lastTime    = 0.0;
        currentTime = 0.0;

        meshManager    = std::make_unique<Mesh::Manager::Instance>();
        textureManager = std::make_unique<Texture::Manager::Instance>();

        prepareMeshes();
        prepareTextures();

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
        buildAssets();
    }

    Application::~Application()
    {
        // Vulkan
        logicalDevice.waitIdle();

        destroyCommandPool();
        destroyGraphicsPipeline();
        destroySwapChain();

        logicalDevice.destroyDescriptorSetLayout(frameDescriptorSetLayout);

        logicalDevice.destroyDescriptorSetLayout(materialDescriptorSetLayout);
        logicalDevice.destroyDescriptorPool(materialDescriptorPool);

        Buffer::destroy(logicalDevice, meshVertexBuffer);

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

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );

        inCommandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            graphicsPipeline.instance
        );
    
        inCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            graphicsPipeline.layout,
            0,
            swapChain.frames[inImageIndex].descriptorSet,
            nullptr
        );

        prepareScene(inCommandBuffer);

        drawObjects(inCommandBuffer);

        inCommandBuffer.endRenderPass();

        inCommandBuffer.end();
    }

    void Application::render()
    {
        Frame::Instance& currentFrame = swapChain.frames[currentImageIndex];

        if (
            logicalDevice.waitForFences(
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
            logicalDevice.resetFences(
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
        do
        {
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            glfwWaitEvents();
        } while (windowWidth == 0 || windowHeight == 0);

        logicalDevice.waitIdle();

        destroySwapChain();
        buildSwapChain();

        buildFramebuffers();
        buildFrameResources();
        buildFramesCommandBuffers();
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
            Buffer::destroy(logicalDevice, frame.cameraData.buffer);

            logicalDevice.unmapMemory(frame.modelData.buffer.memory);
            Buffer::destroy(logicalDevice, frame.modelData.buffer);
        }

        logicalDevice.destroySwapchainKHR(swapChain.instance);

        logicalDevice.destroyDescriptorPool(frameDescriptorPool);
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
            materialDescriptorSetLayout,
            logicalDevice,
            materialLayoutBidings
        );
    }

    void Application::buildGraphicsPipeline()
    {
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.push_back(frameDescriptorSetLayout);
        descriptorSetLayouts.push_back(materialDescriptorSetLayout);
    
        GraphicsPipeline::CreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.logicalDevice        = logicalDevice;
        graphicsPipelineCreateInfo.vertexShaderName     = "triangle.vert.spv";
        graphicsPipelineCreateInfo.fragmentShaderName   = "triangle.frag.spv";
        graphicsPipelineCreateInfo.swapChainExtent      = swapChain.extent;
        graphicsPipelineCreateInfo.swapChainImageFormat = swapChain.format;
        graphicsPipelineCreateInfo.descriptorSetLayouts = descriptorSetLayouts;

        GraphicsPipeline::init(
            graphicsPipeline,
            graphicsPipelineCreateInfo
        );    
    }

    void Application::destroyGraphicsPipeline()
    {
        logicalDevice.destroyPipeline(graphicsPipeline.instance);
        logicalDevice.destroyPipelineLayout(graphicsPipeline.layout);
        logicalDevice.destroyRenderPass(graphicsPipeline.renderPass);
    }

    void Application::buildFramebuffers()
    {
        Frame::Buffer::CreateInfo createInfo = {
            logicalDevice,
            graphicsPipeline.renderPass,
            swapChain.extent
        };

        Frame::Buffer::init(swapChain.frames, createInfo);
    }

    void Application::buildCommandPool()
    {
        Command::Pool::init(
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

    void Application::buildMainCommandBuffer()
    {
        Command::Buffer::CreateInfo createInfo = {
            logicalDevice,
            commandPool
        };

        Command::Buffer::init(mainCommandBuffer, createInfo);
    }

    void Application::buildFramesCommandBuffers()
    {
        Command::Buffer::CreateInfo createInfo = {
            logicalDevice,
            commandPool
        };

        Frame::Buffer::initCommand(swapChain.frames, createInfo);
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
           
            frame.initResources(
                logicalDevice,
                physicalDevice,
                scene
            );

            Descriptor::initSet(
                frame.descriptorSet,
                logicalDevice,
                frameDescriptorSetLayout,
                frameDescriptorPool
            );
        }
    }

    void Application::buildMaterialResources()
    {
        Descriptor::PoolCreateInfo poolCreateInfo;
        poolCreateInfo.count = 1;
        poolCreateInfo.size  = textureManager->getCount();
        poolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            materialDescriptorPool,
            logicalDevice,
            poolCreateInfo
        );
    }

    void Application::prepareMeshes()
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

        meshManager->addMesh(
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

        meshManager->addMesh(
            "Square",
            squareVertices
        );
    }

    void Application::buildMeshes()
    {
        meshManager->initVertexBuffer(
            meshVertexBuffer,
            logicalDevice,
            physicalDevice,
            graphicsQueue,
            mainCommandBuffer
        );
    }

    void Application::prepareTextures()
    {
        Texture::Data grayTextureData;
        grayTextureData.width    = 512;
        grayTextureData.height   = 512;
        grayTextureData.filename = "gray.png";

        textureManager->addTexture("Gray", grayTextureData);

        Texture::Data gridTextureData;
        gridTextureData.width    = 512;
        gridTextureData.height   = 512;
        gridTextureData.filename = "grid.png";

        textureManager->addTexture("Grid", gridTextureData);

        Texture::Data uvTextureData;
        uvTextureData.width    = 512;
        uvTextureData.height   = 512;
        uvTextureData.filename = "uv.png";

        textureManager->addTexture("UV", uvTextureData);
    }

    void Application::buildTextures()
    {
        textureManager->buildTexturesInstances(
            logicalDevice,
            physicalDevice,
            mainCommandBuffer,
            graphicsQueue,
            materialDescriptorSetLayout,
            materialDescriptorPool
        );
    }

    void Application::destroyTextures()
    {
        textureManager->destroyTexturesInstances();
    }

    void Application::buildAssets()
    {
        buildMeshes();
        buildTextures();
    }

    void Application::destroyAssets()
    {
        meshManager.reset();
        textureManager.reset();
    }

    void Application::prepareScene(const vk::CommandBuffer& inCommandBuffer)
    {
        vk::Buffer vertexBuffers[] = { meshVertexBuffer.instance };
        vk::DeviceSize offsets[]   = { 0 };

        inCommandBuffer.bindVertexBuffers(
            0,
            1,
            vertexBuffers,
            offsets
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

    void Application::prepareSceneObjects(Frame::Instance& outFrame)
    {
        std::vector<Scene::Object::Instance> sceneObjects = scene.getObjects();
            
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
        Frame::Instance& frame = swapChain.frames[inImageIndex];

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

        frame.writeDescriptorSet(logicalDevice);
    }

    void Application::drawObjects(const vk::CommandBuffer& inCommandBuffer)
    {
        for (Scene::Object::Instance sceneObject : scene.objects)
        {
            textureManager->bindTexture(
                sceneObject.texture.id,
                inCommandBuffer,
                graphicsPipeline.layout
            );

            meshManager->drawMesh(
                sceneObject.mesh.id,
                inCommandBuffer
            );
        }
    }
}