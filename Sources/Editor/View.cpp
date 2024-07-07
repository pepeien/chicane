#include "Editor/View.hpp"

#include <windows.h>
#include <tchar.h>

#include "Runtime/Core.hpp"
#include "Runtime/Game/State.hpp"

#include "imgui/misc/cpp/imgui_stdlib.h"

namespace Chicane
{
    namespace Editor
    {
        View::View()
            : Grid::View("home"),
            m_isShowingMetrics(true),
            m_activeMenu(Menu::Grid),
            m_activeToolMenu(ToolMenu::None)
        {
            // Cubemap
            m_cubemapEntryFormats = {
                {
                    "Texture",
                    "jpg"
                },
                {
                    "Texture",
                    "tga"
                }
            };

            m_cubemapForm = {
                {
                    { "name",      "Name",              FormFieldType::Text },
                    { "positiveX", "Select Positive X", FormFieldType::File },
                    { "negativeX", "Select Negative X", FormFieldType::File },
                    { "positiveY", "Select Positive Y", FormFieldType::File },
                    { "negativeY", "Select Negative Y", FormFieldType::File },
                    { "positiveZ", "Select Positive Z", FormFieldType::File },
                    { "negativeZ", "Select Negative Z", FormFieldType::File },
                    { "outputPath", "Select Output",    FormFieldType::Folder }
                }
            };

            m_cubemapOptions        = { "Create", "Edit" };
            m_cubemapSelectedOption = "";

            m_cubemapInfo = {};
            m_cubemapInfo.type = (uint8_t) Box::Type::CubeMap;
            m_cubemapInfo.entries.resize(6);

            for (Box::WriteEntry& entry : m_cubemapInfo.entries)
            {
                entry.type = (uint8_t) Box::EntryType::Texture;
            }
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

                    if (ImGui::Button("Grid"))
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
                            if (ImGui::BeginMenu("Cubemap"))
                            {
                                for (const std::string& option : m_cubemapOptions)
                                {
                                    bool isSelected = m_cubemapSelectedOption == option;

                                    if (ImGui::MenuItem(option.c_str(), "", isSelected))
                                    {
                                        selectCubemapMenu(option);
                                    }
                                }

                                ImGui::EndMenu();
                            }
                        ImGui::EndMenuBar();

                        if (m_activeToolMenu == ToolMenu::CubemapNew)
                        {
                            renderCubemapNewMenu();
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

        void View::selectCubemapMenu(const std::string& inOption)
        {
            m_cubemapSelectedOption = inOption;

            if (inOption == "Create")
            {
                m_activeToolMenu = ToolMenu::CubemapNew;

                return;
            }

            if (inOption == "Edit")
            {
                m_activeToolMenu = ToolMenu::CubemapEdit;

                return;
            }
        }
        
        void View::renderCubemapNewMenu()
        {
            for (FormField& formField : m_cubemapForm.fields)
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

                switch (formField.type)
                {
                case FormFieldType::Text:
                    ImGui::InputText(
                        formField.label.c_str(),
                        &formField.value
                    );

                    continue;
                case FormFieldType::File:
                    if (ImGui::Button(formField.label.c_str()))
                    {
                        FileSystem::FileResult selectedFile = FileSystem::openFileDialog(
                            formField.label.c_str(),
                            m_cubemapEntryFormats
                        );

                        if (!selectedFile.path.empty())
                        {
                            formField.value = selectedFile.path;
                        }
                    }

                    ImGui::Text(
                        formField.value.empty() ?
                        "Not Selected" : formField.value.c_str()
                    );

                    continue;
                case FormFieldType::Folder:
                    if (ImGui::Button(formField.label.c_str()))
                    {
                        FileSystem::DirectoryResult outputLocation = FileSystem::openDirectoryDialog();

                        if (!outputLocation.path.empty())
                        {
                            formField.value = outputLocation.path;
                        }
                    }

                    ImGui::Text(
                        formField.value.empty() ?
                        "Not Selected" : formField.value.c_str()
                    );

                    continue;

                default:
                    break;
                }
            }

            ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - 20);

            bool isDisabled = !m_cubemapForm.isValid();

            if (isDisabled)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth(), 20)))
            {
                if (!isDisabled)
                {
                    m_cubemapInfo.name                = m_cubemapForm.get("name").value;
                    m_cubemapInfo.outputPath          = m_cubemapForm.get("outputPath").value;
                    m_cubemapInfo.entries[0].filePath = m_cubemapForm.get("positiveX").value;
                    m_cubemapInfo.entries[1].filePath = m_cubemapForm.get("negativeX").value;
                    m_cubemapInfo.entries[2].filePath = m_cubemapForm.get("positiveY").value;
                    m_cubemapInfo.entries[3].filePath = m_cubemapForm.get("negative").value;
                    m_cubemapInfo.entries[4].filePath = m_cubemapForm.get("positiveZ").value;
                    m_cubemapInfo.entries[5].filePath = m_cubemapForm.get("negativeZ").value;

                    m_cubemapForm.clear();

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