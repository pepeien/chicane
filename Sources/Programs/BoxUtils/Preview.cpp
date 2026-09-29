#include "Preview.hpp"

#include <memory>

#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Core/Math/Vec.hpp>
#include <Chicane/Core/Window.hpp>
#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>
#include <Chicane/Renderer/Feature.hpp>
#include <Chicane/Renderer/Instance.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Instance/CreateInfo.hpp>

#include "Preview/Scene.hpp"

bool bakePreviewsWithRuntime(const std::vector<Chicane::FileSystem::Path>& inAssets)
{
    if (inAssets.empty())
    {
        return true;
    }

    static constexpr int PREVIEW_SIZE = Chicane::Box::AssetPreview::SIZE;

    std::shared_ptr<PreviewScene> scene = std::make_shared<PreviewScene>(inAssets);

    Chicane::InstanceCreateInfo createInfo;
    createInfo.window.title     = "Chicane BoxUtils";
    createInfo.window.size      = Chicane::Vec<2, std::uint32_t>(PREVIEW_SIZE, PREVIEW_SIZE);
    createInfo.window.display   = 0;
    createInfo.window.type      = Chicane::WindowType::Windowed;
    createInfo.window.bIsHidden = true;
#if CHICANE_VULKAN
    createInfo.window.backend = Chicane::WindowBackend::Vulkan;
#elif CHICANE_OPENGL
    createInfo.window.backend = Chicane::WindowBackend::OpenGL;
#endif
    createInfo.renderer.resolution = createInfo.window.size;

    createInfo.onSetup = [scene]()
    {
        scene->load();
        Chicane::Instance::sInstance().setScene(scene);
        if (Chicane::Renderer::Instance* renderer = Chicane::Instance::sInstance().getRenderer())
        {
            renderer->disableFeature(Chicane::Renderer::RendererFeature::Bloom);
            renderer->disableFeature(Chicane::Renderer::RendererFeature::HDR);
            renderer->disableFeature(Chicane::Renderer::RendererFeature::Outline);
        }
        if (Chicane::Window* window = Chicane::Window::sGetCurrent())
        {
            window->disableResizing();
            window->setSize(PREVIEW_SIZE, PREVIEW_SIZE);
            window->setPosition(-32000, -32000);
            if (window->isMinimized())
            {
                window->restore();
            }
        }
    };
    createInfo.onFrame = [scene]() { scene->onFrame(); };

    Chicane::Instance::sInstance().run(createInfo);

    return true;
}
