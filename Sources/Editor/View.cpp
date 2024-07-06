#include "Editor/View.hpp"

#include "Runtime/Core.hpp"
#include "Runtime/Game/State.hpp"

#include <windows.h>
#include <tchar.h>

namespace Chicane
{
    namespace Editor
    {
        View::View()
            : Grid::View("home"),
            m_isShowingMetrics(true),
            m_activeMenu(Menu::None),
            m_activeToolMenu(ToolMenu::None)
        {
            m_cubemapEntryFormats = {
                {
                    "Texture",
                    "tga"
                }
            };
            m_cubemapAxes = {
                "Select Postive X",
                "Select Negative X",
                "Select Postive Y",
                "Select Negative Y",
                "Select Postive Z",
                "Select Negative Z",
            };

            m_cubemapName = new char(256);

            m_cubemapInfo = {};
            m_cubemapInfo.type = (uint8_t) Box::Type::CubeMap;
            m_cubemapInfo.entries.resize(6);
        }

        void View::show(
            const Vec2& inResolution,
            const Vec2& inPosition
        )
        {
            ImGuiWindowFlags viewFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoNav |
                                         ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration |
                                         ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
                                         ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;

            ImGui::Begin(
                m_id.c_str(),
                nullptr,
                viewFlags
            );
                ImGui::SetWindowSize(
                    ImVec2(
                        static_cast<float>(inResolution.x),
                        static_cast<float>(inResolution.y)
                    )
                );
                ImGui::SetWindowPos(
                    ImVec2(
                        static_cast<float>(inPosition.x),
                        static_cast<float>(inPosition.y)
                    )
                );

                ImGui::BeginMenuBar();
                    if (ImGui::Button("Metrics"))
                    {
                        onMetricsButtonClick();
                    }

                    if (ImGui::Button("Box"))
                    {
                        onGridButtonClick();
                    }
                ImGui::EndMenuBar();

                if (m_isShowingMetrics)
                {
                    Chicane::Telemetry telemetry = Chicane::State::getTelemetry();

                    ImGui::Text("FPS: %d",          telemetry.framerate);
                    ImGui::Text("Frametime: %.2f ms", telemetry.time);
                }

                if (m_activeMenu == Menu::Grid)
                {
                    ImGuiWindow* currentWindow = ImGui::GetCurrentWindow();
 
                    float width  = Grid::calculateSizeFromViewportWidth(30.0f);
                    float height = Grid::calculateSizeFromViewportHeight(100.0f);

                    ImGui::SetNextWindowPos(
                        ImVec2(
                            currentWindow->InnerRect.Max.x - width,
                            currentWindow->InnerRect.Min.y
                        )
                    );
                    ImGui::SetNextWindowSize(
                        ImVec2(
                            width,
                            height - 19.0f
                        )
                    );
                    ImGui::Begin(
                        "Tool",
                        nullptr,
                        ImGuiWindowFlags_::ImGuiWindowFlags_NoNav |
                        ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration |
                        ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar
                    );
                        ImGui::BeginMenuBar();
                            if (ImGui::Button("Cubemap"))
                            {
                                m_activeToolMenu = ToolMenu::Cubemap;
                            }

                            if (ImGui::Button("Model"))
                            {
                                m_activeToolMenu = ToolMenu::Model;
                            }
                        ImGui::EndMenuBar();

                        if (m_activeToolMenu == ToolMenu::Cubemap)
                        {
                            renderCubemapToolMenu();
                        }

                        if (m_activeToolMenu == ToolMenu::Model)
                        {
                            renderModelToolMenu();
                        }
                    ImGui::End();
                }

            ImGui::End();
        }

        void View::onMetricsButtonClick()
        {
            m_isShowingMetrics = !m_isShowingMetrics;
        }

        void View::onGridButtonClick()
        {
            m_activeMenu = Menu::Grid;
        }

        void View::renderCubemapToolMenu()
        {
            ImGui::InputText("Name", m_cubemapName, 256);

            m_cubemapInfo.name = m_cubemapName;

            for (int i = 0; i < m_cubemapAxes.size(); i++)
            {
                const std::string& axis = m_cubemapAxes[i];

                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

                if (ImGui::Button(axis.c_str()))
                {
                    FileSystem::FileResult selectedFile = FileSystem::openFileDialog(
                        axis.c_str(),
                        m_cubemapEntryFormats
                    );

                    if (!selectedFile.path.empty())
                    {
                        Box::WriteEntry entry = {};
                        entry.filePath = selectedFile.path;
                        entry.type     = (uint8_t) Box::Type::CubeMap;

                        m_cubemapInfo.entries[i] = entry;
                    }
                }

                ImGui::Text(
                    m_cubemapInfo.entries[i].filePath.empty() ?
                    "Not Selected" : m_cubemapInfo.entries[i].filePath.c_str()
                );
            }

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

            if (ImGui::Button("Select Output Location"))
            {
                FileSystem::DirectoryResult outputLocation = FileSystem::openDirectoryDialog();

                if (!outputLocation.path.empty())
                {
                    m_cubemapInfo.outputPath = outputLocation.path;
                }
            }

            ImGui::Text(
                m_cubemapInfo.outputPath.empty() ?
                "Not Selected" : m_cubemapInfo.outputPath.c_str()
            );

            ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - 20);

            bool isDisabled = m_cubemapInfo.name.empty() || m_cubemapInfo.outputPath.empty();

            for (Box::WriteEntry entry : m_cubemapInfo.entries)
            {
                if (entry.filePath.empty())
                {
                    isDisabled = true;

                    break;
                }
            }

            if (isDisabled)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth(), 20)))
            {
                if (!isDisabled)
                {
                    Box::write(m_cubemapInfo);
                }
            }

            if (isDisabled)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
        }

        void View::renderModelToolMenu()
        {}
    }
}