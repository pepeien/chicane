#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Script/Bus.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Cursor.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Component/Container.hpp"
#include "Chicane/Grid/Component/View/InputQueue.hpp"

namespace Chicane
{
    namespace Grid
    {
        class ViewScript;

        CH_TYPE(Manual)
        class CHICANE_GRID View : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "View";

            // Attribute
            static constexpr inline const char* PATH_ATTRIBUTE_NAME = "path";

        public:
            CH_CONSTRUCTOR()
            View();

            CH_CONSTRUCTOR()
            View(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle = {});

            ~View() override;

        public:
            void tick(float inDelta) override;

        public:
            // Window
            void post(const WindowEvent& inEvent);
            WindowCursor getPointer() const;
            Component* getHovered() const;
            void clearInteraction(Component* inComponent);

            // Event
            void focusOn(Component* inComponent);
            std::uint64_t subscribe(const String& inName, std::function<void(const String&)> inCallback);
            void unsubscribe(std::uint64_t inToken);
            void send(const String& inName, const String& inData = {});
            void receive(const String& inName, const String& inData);
            void pumpEvents();
            bool callLuaGlobal(const String& inName, const std::vector<String>& inArgs = {});
            void loadViewScript(const FileSystem::Path& inTemplate);

            // Hierarchy
            std::vector<Component*> getChildrenAt(const Vec2& inLocation) const;

            // Styling
            const StyleFile& getStyleFile() const;

            // Draw
            void collectDrawables(std::vector<Component*>& outComponents);

        protected:
            static void sAppendDrawables(Component* inComponent, std::vector<Component*>& outComponents);

            void load(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle = {});
            void handle(const WindowEvent& inEvent);
            void pump();

            WindowCursor resolvePointer() const;
            Component* resolveHit(Component* inHit) const;
            Component* resolveFocus(Component* inHit) const;

            void syncHovered(Component* inComponent);
            void syncFocused(Component* inComponent);
            void syncDragging(Component* inComponent);

        protected:
            // Routing
            String                          m_path;

            // Interaction
            Component*                      m_hovered;
            Component*                      m_focused;
            Component*                      m_dragging;
            std::unique_ptr<ViewInputQueue> m_inputs;
            std::atomic<WindowCursor>       m_pointer;

            std::vector<const Component*>   m_roundedAncestors;

            // Script
            Script::Bus                     m_bus;
            std::unique_ptr<ViewScript>     m_viewScript;
        };
    }
}