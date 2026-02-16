#pragma once

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Function.hpp"
#include "Chicane/Grid/Function/Data.hpp"
#include "Chicane/Grid/Primitive.hpp"
#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/Style/File.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Component
        {
        public:
            using Compiler = std::function<Component*(const pugi::xml_node& inNode)>;

        public:
            static constexpr inline const char* EVENT_KEYWORD = "$event";

            static constexpr inline const char* ON_HOVER_ATTRIBUTE_NAME = "onHover";
            static constexpr inline const char* ON_CLICK_ATTRIBUTE_NAME = "onClick";

        public:
            Component(const pugi::xml_node& inNode);
            Component(const String& inTag);

            virtual ~Component();

        public:
            // Status
            virtual bool isDrawable() const;

        protected:
            // Lifescycle Events
            virtual void onEvent(const WindowEvent& inEvent) { return; }
            virtual void onTick(float inDeltaTime) { return; }
            virtual void onRefresh() { return; }
            virtual void onAdoption(Component* inChild) { return; }
            virtual void onAdopted(Component* inParent) { return; }

            // Mouse Events
            virtual void onHover() { return; }
            virtual void onClick() { return; }

            virtual void refreshPrimitive() { return; }

        public:
            // Checkers
            bool isRoot() const;
            bool isVisible() const;
            bool isValidChild(Component* inComponent) const;

            // Mouse Events
            void hover();
            void click();

            // Lifecycle Events
            void tick(float inDelta);
            void refresh();

            // Properties
            const String& getTag() const;

            String getId() const;
            void setId(const String& inValue);

            std::vector<String> getClassList() const;
            const String& getClassName() const;
            void setClassName(const String& inValue);
            template <typename... Args>
            inline void addClassName(Args... inClasses)
            {
                String className = m_className;

                (
                    [&]()
                    {
                        className.append(inClasses);
                        className.append(' ');
                    }(),
                    ...
                );

                setClassName(className.trim());
            }

            String getAttribute(const String& inName) const;

            // Style
            bool hasStyleFile() const;
            const StyleFile* getStyleFile() const;
            void setStyleFile(StyleFile* inSource);
            void addStyleRuleset(const StyleRuleset::List& inSources);
            void addStyleProperties(const StyleRuleset::Properties& inSource);

            const Style& getStyle() const;

            // Reference
            bool hasReference(const String& inId, bool isLocalOnly = false) const;
            Reference* getReference(const String& inId) const;
            void addReference(const Reference::Map& inReference);
            void addReference(const String& inId, Reference* inReference);
            void removeReference(const String& inId);

            bool hasFunction(const String& inId, bool isLocalOnly = false) const;
            const Function getFunction(const String& inId, bool isLocalOnly = false) const;
            void addFunction(const Functions& inFunctions);
            void addFunction(const String& inId, Function inFunction);
            void removeFunction(const String& inId);

            // Hierarchy
            bool hasRoot() const;
            Component* getRoot() const;
            void setRoot(Component* inComponent);

            bool hasParent() const;
            Component* getParent() const;
            void setParent(Component* inComponent);

            bool hasNeighbours() const;
            Component* getNeighbour(int inJumps) const;

            bool hasChildren() const;
            const std::vector<Component*>& getChildren() const;
            std::vector<Component*> getChildrenFlat() const;
            void addChildren(const pugi::xml_node& inNode);
            void addChild(Component* inComponent);

            Vec2 getChildrenContentSizeBlock() const;
            Vec2 getChildrenContentSizeFlex() const;
            Vec2 getChildrenContentSize() const;

            // Positioning
            Vec2 getAvailableSize() const;
            const Vec2& getSize() const;
            void setSize(const Vec2& inSize);
            void setSize(float inWidth, float inHeight);

            const Vec2& getPosition() const;
            void addPosition(const Vec2& inPosition);
            void addPosition(float inX, float inY);
            void setPosition(const Vec2& inPosition);
            void setPosition(float inX, float inY);

            const Vec2& getCursor() const;
            void addCursor(const Vec2& inCursor);
            void addCursor(float inX, float inY);
            void setCursor(const Vec2& inCursor);
            void setCursor(float inX, float inY);

            // Collision
            const Bounds2D& getBounds() const;

            // Draw
            bool hasPrimitive() const;
            const Primitive& getPrimitive() const;
            void clearPrimitive();
            void setPrimitive(const Primitive& inPrimitive);

            // Parsing
            String parseText(const String& inValue) const;

        protected:
            void refreshStyle();
            void refreshStyleRuleset();
            void refreshSize();
            void refreshPosition();
            void refreshBounds();

            bool isReference(const String& inValue) const;
            Reference parseReference(const String& inValue) const;

            bool isFunction(const String& inValue) const;
            FunctionData parseFunction(const String& inRefValue) const;

        protected:
            // Identification
            String                  m_tag;
            String                  m_id;
            String                  m_className;

            // Style
            Style                   m_style;
            StyleFile*              m_styleFile;

            // References
            Reference::Map          m_references;
            Functions               m_functions;

            // Hierarchy
            Component*              m_root;
            Component*              m_parent;
            std::vector<Component*> m_children;

            // Position
            Vec2                    m_size;
            Vec2                    m_position;
            Vec2                    m_cursor;

            // Collision
            Bounds2D                m_bounds;

            // Draw
            Primitive               m_primitive;

            // XML
            Xml::Attributes         m_attributes;
        };
    }
}