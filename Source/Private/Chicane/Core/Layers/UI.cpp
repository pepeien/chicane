#include "Chicane/Core/Layers/UI.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    UILayer::UILayer(Window* inWindow)
        : Layer("UI"),
        m_window(inWindow)
    {
        m_isInitialized = Grid::hasViews();
        m_internals = inWindow->getRendererInternals();

        if (!m_isInitialized)
        {
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.IniFilename = nullptr;

        ImVec2 padding = ImVec2(0.0f, 0.0f);
        ImVec2 minSize = ImVec2(1.0f, 1.0f);

        ImGuiStyle& style = ImGui::GetStyle();
        style.DisplayWindowPadding   = padding;
        style.DisplaySafeAreaPadding = padding;

        style.ChildBorderSize = 0.0f;

        style.WindowPadding    = padding;
        style.WindowBorderSize = 0.0f;
        style.WindowRounding   = 0.0f;
        style.WindowMinSize    = minSize;

        style.FramePadding    = padding;
        style.FrameBorderSize = 0.0f;

        style.SeparatorTextPadding    = padding;
        style.SeparatorTextBorderSize = 0.0f;

        style.CellPadding = padding;
    
        style.ButtonTextAlign = padding;

        style.TabBorderSize = 0.0f;

        style.GrabMinSize = 0.0f;

        style.ItemInnerSpacing = padding;
        style.ItemSpacing      = padding;

        ImGui_ImplSDL3_InitForVulkan(m_window->instance);
    }

    UILayer::~UILayer()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_internals.logicalDevice.waitIdle();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        m_internals.logicalDevice.destroyRenderPass(m_renderPass);
        m_internals.logicalDevice.destroyDescriptorPool(m_descriptorPool);
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

        ImGui_ImplSDL3_ProcessEvent(&inEvent);
    }

    void UILayer::setup(Chicane::Frame::Instance& outFrame)
    {
        if (!m_isInitialized)
        {
            return;
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        Grid::getActiveView()->show(
            m_window->getDrawableSize(),
            m_window->getPosition()
        );

        ImGui::Render();

    	ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags)
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
            m_internals.logicalDevice,
            createInfo
        );
    }

    void UILayer::initRenderpass()
    {
        vk::AttachmentDescription attachment = GraphicsPipeline::createColorAttachment(
            m_internals.swapchain->format,
            vk::AttachmentLoadOp::eLoad,
            vk::ImageLayout::ePresentSrcKHR
        );

        m_renderPass = GraphicsPipeline::createRendepass(
            false,
            { attachment },
            m_internals.logicalDevice
        );
    }

    void UILayer::initFramebuffers()
    {
        Frame::Buffer::CreateInfo framebufferCreateInfo {};
        framebufferCreateInfo.id              = m_id;
        framebufferCreateInfo.logicalDevice   = m_internals.logicalDevice;
        framebufferCreateInfo.renderPass      = m_renderPass;
        framebufferCreateInfo.swapChainExtent = m_internals.swapchain->extent;

        for (Frame::Instance& frame : m_internals.swapchain->images)
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
            m_internals.physicalDevice,
            m_internals.sufrace
        );

        ImGui_ImplVulkan_InitInfo imguiInitInfo {};
        imguiInitInfo.Instance        = m_internals.instance;
        imguiInitInfo.PhysicalDevice  = m_internals.physicalDevice;
        imguiInitInfo.Device          = m_internals.logicalDevice;
        imguiInitInfo.QueueFamily     = familyIndices.graphicsFamily.value();
        imguiInitInfo.Queue           = m_internals.graphicsQueue;
        imguiInitInfo.PipelineCache   = VK_NULL_HANDLE;
        imguiInitInfo.DescriptorPool  = m_descriptorPool;
        imguiInitInfo.Allocator       = nullptr;
        imguiInitInfo.MinImageCount   = 3;
        imguiInitInfo.ImageCount      = m_internals.imageCount;
        imguiInitInfo.RenderPass      = m_renderPass;

        ImGui_ImplVulkan_Init(&imguiInitInfo);
    }
}