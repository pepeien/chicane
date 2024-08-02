#include "Chicane/Core/Layers/UI.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    UILayer::UILayer(Window* inWindow)
        : Layer("UI"),
        m_window(inWindow),
        m_renderer(inWindow->getRenderer())
    {
        m_isInitialized = Grid::hasViews();

        if (!m_isInitialized)
        {
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.IniFilename = nullptr;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplSDL2_InitForVulkan(m_window->instance);
    }

    UILayer::~UILayer()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_renderer->m_logicalDevice.waitIdle();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        m_renderer->m_logicalDevice.destroyRenderPass(m_renderPass);
        m_renderer->m_logicalDevice.destroyDescriptorPool(m_descriptorPool);
    }

    void UILayer::build()
    {
        if (!m_isInitialized)
        {
            return;
        }

        initDescriptorPool();
        initRenderpass();
        initFramebuffers();
        initImgui();
    }

    void UILayer::rebuild()
    {
        if (!m_isInitialized)
        {
            return;
        }

        initFramebuffers();

        Grid::getActiveView()->rebuild();
    }

    void UILayer::onEvent(const SDL_Event& inEvent)
    {
        if (!m_isInitialized)
        {
            return;
        }

        ImGui_ImplSDL2_ProcessEvent(&inEvent);
    }

    void UILayer::setup(Chicane::Frame::Instance& outFrame)
    {
        if (!m_isInitialized)
        {
            return;
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        Grid::getActiveView()->show(
            m_window->getResolution(),
            m_window->getPosition()
        );

    	ImGuiIO &io = ImGui::GetIO();

        ImGui::Render();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
    }

    void UILayer::render(
        Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        if (!m_isInitialized)
        {
            return;
        }

        // Renderpass
        std::vector<vk::ClearValue> clearValues;
        clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

        vk::RenderPassBeginInfo renderPassBeginInfo {};
        renderPassBeginInfo.renderPass          = m_renderPass;
        renderPassBeginInfo.framebuffer         = outFrame.getFramebuffer(m_id);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = inSwapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues        = clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), inCommandBuffer);

        inCommandBuffer.endRenderPass();

        ImGui::EndFrame();
    }

    void UILayer::initDescriptorPool()
    {
        Descriptor::PoolCreateInfo createInfo;
        createInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        createInfo.size  = 1000;
        createInfo.types.push_back(vk::DescriptorType::eSampler);
        createInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        createInfo.types.push_back(vk::DescriptorType::eSampledImage);
        createInfo.types.push_back(vk::DescriptorType::eStorageImage);
        createInfo.types.push_back(vk::DescriptorType::eUniformTexelBuffer);
        createInfo.types.push_back(vk::DescriptorType::eStorageTexelBuffer);
        createInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        createInfo.types.push_back(vk::DescriptorType::eStorageBuffer);
        createInfo.types.push_back(vk::DescriptorType::eUniformBufferDynamic);
        createInfo.types.push_back(vk::DescriptorType::eStorageBufferDynamic);
        createInfo.types.push_back(vk::DescriptorType::eInputAttachment);

        Descriptor::initPool(
            m_descriptorPool,
            m_renderer->m_logicalDevice,
            createInfo
        );
    }

    void UILayer::initRenderpass()
    {
        vk::AttachmentDescription attachment = GraphicsPipeline::createColorAttachment(
            m_renderer->m_swapChain.format,
            vk::AttachmentLoadOp::eLoad,
            vk::ImageLayout::ePresentSrcKHR
        );

        m_renderPass = GraphicsPipeline::createRendepass(
            false,
            { attachment },
            m_renderer->m_logicalDevice
        );
    }

    void UILayer::initFramebuffers()
    {
        Frame::Buffer::CreateInfo framebufferCreateInfo {};
        framebufferCreateInfo.id              = m_id;
        framebufferCreateInfo.logicalDevice   = m_renderer->m_logicalDevice;
        framebufferCreateInfo.renderPass      = m_renderPass;
        framebufferCreateInfo.swapChainExtent = m_renderer->m_swapChain.extent;

        for (Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            framebufferCreateInfo.attachments.clear();
            framebufferCreateInfo.attachments.push_back(frame.imageView);

            Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void UILayer::initImgui()
    {
        Queue::FamilyIndices familyIndices;
        Queue::findFamilyInidices(
            familyIndices,
            m_renderer->m_physicalDevice,
            m_renderer->m_surface
        );

        ImGui_ImplVulkan_InitInfo imguiInitInfo {};
        imguiInitInfo.Instance        = m_renderer->m_instance;
        imguiInitInfo.PhysicalDevice  = m_renderer->m_physicalDevice;
        imguiInitInfo.Device          = m_renderer->m_logicalDevice;
        imguiInitInfo.QueueFamily     = familyIndices.graphicsFamily.value();
        imguiInitInfo.Queue           = m_renderer->m_graphicsQueue;
        imguiInitInfo.PipelineCache   = VK_NULL_HANDLE;
        imguiInitInfo.DescriptorPool  = m_descriptorPool;
        imguiInitInfo.Allocator       = nullptr;
        imguiInitInfo.MinImageCount   = 3;
        imguiInitInfo.ImageCount      = m_renderer->m_imageCount;
        imguiInitInfo.RenderPass      = m_renderPass;

        ImGui_ImplVulkan_Init(&imguiInitInfo);
    }
}