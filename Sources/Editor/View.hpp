#pragma once

#include "Runtime/Core.hpp"
#include "Runtime/Grid.hpp"

namespace Chicane
{
    namespace Editor
    {
        enum class Menu
        {
            None,
            Grid
        };

        enum class ToolMenu
        {
            None,

            CubemapNew,
            CubemapEdit,

            ModelNew,
            ModelEdit
        };

        enum class FormFieldType
        {
            Text,
            File,
            Folder
        };

        struct FormField
        {
        public:
            std::string id     = "";
            std::string label  = "";
            FormFieldType type = FormFieldType::Text;
            bool isRequired = true;

            std::string value = "";
        
        public:
            bool isValid()
            {
                return isRequired ? !value.empty() : true;
            }

            void clear()
            {
                value = "";
            }
        };

        struct Form
        {
        public:
            std::vector<FormField> fields = {};

        public:
            bool isValid()
            {
                for (FormField field : fields)
                {
                    if (!field.isValid())
                    {
                        return false;
                    }
                }

                return true;
            }

            void clear()
            {
                for (FormField& field : fields)
                {
                    field.clear();
                }
            }

            const FormField& get(const std::string& inId)
            {
                if (fields.empty())
                {
                    return {};
                }

                auto& foundIt = std::find_if(
                    fields.begin(),
                    fields.end(),
                    [inId](FormField _) { return _.id == inId; }
                );

                if (foundIt == fields.end())
                {
                    return {};
                }

                return fields[foundIt - fields.begin()];
            }
        };

        class View : public Grid::View
        {
        public:
            View();

        public:
            void show(
                const Vec2& inResolution,
                const Vec2& inPosition
            ) override;

        private:
            void onMetricsButtonClick();
            void onGridButtonClick();

            // Tool Menus
            void selectCubemapMenu(const std::string& inOption);
            void renderCubemapNewMenu();
    
            void renderModelToolMenu();

        private:
            bool m_isShowingMetrics;

            Menu m_activeMenu;
            ToolMenu m_activeToolMenu;

            // Cubemap
            std::vector<FileSystem::FileFormat> m_cubemapEntryFormats;
            Form m_cubemapForm;
            
            std::vector<std::string> m_cubemapOptions;
            std::string m_cubemapSelectedOption;

            char* m_cubemapName;
            Box::WriteInfo m_cubemapInfo;
        };
    }
}