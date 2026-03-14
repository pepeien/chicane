#pragma once

#include <functional>
#include <set>
#include <unordered_map>

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Reflection/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Field/Info.hpp"
#include "Chicane/Core/Reflection/Method/Info.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Primitive.hpp"
#include "Chicane/Grid/Method.hpp"
#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/Style/File.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Component
        {
        public:
            using ClassList  = std::set<String>;
            using Directive  = std::function<void(const String&)>;
            using Directives = std::unordered_map<String, Directive>;
            using Variables  = std::unordered_map<String, ReflectionFieldAccessor>;

        public:
            static constexpr inline const char* IF_DIRECTIVE_KEYWORD  = "dir:if";
            static constexpr inline const char* FOR_DIRECTIVE_KEYWORD = "dir:for";

            static constexpr inline const char* EVENT_KEYWORD = "$event";

            static constexpr inline const char* ON_HOVER_ATTRIBUTE_NAME = "onHover";
            static constexpr inline const char* ON_CLICK_ATTRIBUTE_NAME = "onClick";

        public:
            CH_CONSTRUCTOR()
            Component(const pugi::xml_node& inNode);

            CH_CONSTRUCTOR()
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
            bool isDisplayable() const;
            bool isVisible() const;
            bool isSolid() const;

            bool canAdopt(Component* inComponent) const;

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

            ClassList getClassList() const;
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

            // Directive
            void refreshDirectives();
            void runDirective(const String& inKey, const String& inValue);
            void setDirective(const String& inKey, const Directive& inValue);

            // Attribute
            const String& getAttribute(const String& inName) const;

            // Style
            bool hasStyleFile() const;
            const StyleFile* getStyleFile() const;
            void setStyleFile(StyleFile* inSource);
            void addStyleRuleset(const StyleRuleset::List& inSources);
            void addStyleProperties(const StyleRuleset::Properties& inSource);

            const Style& getStyle() const;

            bool hasLocalSelector(const String& inValue) const;
            bool hasSelector(const String& inValue) const;

            // Reference
            ReflectionFieldAccessor getField(const String& inId) const;
            Method getMethod(const String& inId) const;

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
            float getDepth() const;

            const Vec2& getSize() const;
            void addSize(const Vec2& inValue);
            void addSize(float inWidth, float inHeight);
            void setSize(const Vec2& inValue);
            void setSize(float inWidth, float inHeight);

            const Vec2& getScale() const;
            void setScale(const Vec2& inValue);
            void setScale(float inX, float inY);

            const Vec2& getOffset() const;
            void setOffset(const Vec2& inValue);
            void setOffset(float inX, float inY);

            const Vec2& getPosition() const;
            void addPosition(const Vec2& inValue);
            void addPosition(float inX, float inY);
            void setPosition(const Vec2& inValue);
            void setPosition(float inX, float inY);

            const Vec2& getCursor() const;
            void addCursor(const Vec2& inValue);
            void addCursor(float inX, float inY);
            void setCursor(const Vec2& inValue);
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
            String parseReference(const String& inValue) const;

            bool isMethod(const String& inValue) const;

        protected:
            // Properties
            String                  m_tag;
            String                  m_id;
            String                  m_className;

            // Modifier
            Directives              m_directives;

            // Runtime
            Variables               m_variables;

            // Style
            Style                   m_style;
            StyleFile*              m_styleFile;

            // Hierarchy
            Component*              m_root;
            Component*              m_parent;
            std::vector<Component*> m_children;

            // Position
            Vec2                    m_size;
            Vec2                    m_scale;
            Vec2                    m_offset;
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