#pragma once

#include <any>
#include <functional>
#include <memory>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Type/Field/Info.hpp"
#include "Chicane/Core/Reflection/Type/Method.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Animatable.hpp"
#include "Chicane/Grid/Primitive.hpp"
#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/Style/File.hpp"
#include "Chicane/Grid/Style/Keyframe.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Component : public Animatable
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
            static constexpr inline const char* ON_LEAVE_ATTRIBUTE_NAME = "onLeave";
            static constexpr inline const char* ON_CLICK_ATTRIBUTE_NAME = "onClick";
            static constexpr inline const char* ON_FOCUS_ATTRIBUTE_NAME = "onFocus";
            static constexpr inline const char* ON_BLUR_ATTRIBUTE_NAME  = "onBlur";

        public:
            static Component* create(const pugi::xml_node& inNode);

        public:
            CH_CONSTRUCTOR()
            Component(const pugi::xml_node& inNode);

            CH_CONSTRUCTOR()
            Component(const String& inTag);

            virtual ~Component();

        protected:
            bool canPlayAnimation() const override;

        public:
            // Status
            virtual bool isDrawable() const;

            // Events
            virtual bool onEvent(const WindowEvent& inEvent);

        protected:
            // Lifescycle Events
            virtual void onTick(float inDeltaTime) { return; }
            virtual void onRefresh() { return; }
            virtual void onAdoption(Component* inChild) { return; }
            virtual void onAdopted(Component* inParent) { return; }

            // Mouse Events
            virtual void onHover() { return; }
            virtual void onLeave() { return; }
            virtual void onClick() { return; }
            virtual void onFocus() { return; }
            virtual void onBlur() { return; }

            // Refresh Events
            virtual void refreshPrimitive() { return; }
            virtual void refreshStyleRuleset();
            virtual void refreshSize();

        public:
            // Checkers
            bool isRoot() const;
            bool isDisplayable() const;
            bool isVisible() const;
            bool isSolid() const;
            bool isHovered() const;
            bool isFocused() const;

            bool canAdopt(Component* inComponent) const;

            // Mouse Events
            void hover();
            void leave();
            void click();
            void focus();
            void blur();

            void setHovered(bool inValue);
            void setFocused(bool inValue);

            // Lifecycle Events
            virtual void tick(float inDelta);
            virtual void refresh();

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
            void importStyleFile(const FileSystem::Path& inValue);
            void addStyleRuleset(const StyleRuleset::List& inSources);
            void addStyleProperties(const StyleRuleset::Properties& inSource);

            const Style& getStyle() const;
            float getOpacity() const;
            float getFilterBlur() const;
            const String& getStyleVariable(const String& inName) const;
            void refreshStyleSubtree();

            bool hasLocalSelector(const String& inValue) const;
            bool hasSelector(const String& inValue) const;

            // Reference
            ReflectionFieldAccessor getField(const String& inId) const;
            ReflectionTypeMethod getMethod(const String& inId) const;

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
            virtual std::vector<Component*> getChildrenFlat() const;
            Component* getHitAt(const Vec2& inLocation) const;
            bool containsPoint(const Vec2& inLocation) const;
            bool broadcastEvent(const WindowEvent& inEvent);
            bool bubbleEvent(const WindowEvent& inEvent, const Vec2& inLocation);
            void addChildren(const pugi::xml_node& inNode);
            void addChild(Component* inComponent, std::size_t inIndex = SIZE_MAX);
            void removeChild(Component* inComponent);

            Vec2 getChildrenContentSizeBlock() const;
            Vec2 getChildrenContentSizeFlex() const;
            Vec2 getChildrenContentSize() const;

            // Positioning
            virtual float getDepth() const;

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

            virtual Vec2 getDrawPosition() const;

            const Vec2& getCursor() const;
            void addCursor(const Vec2& inValue);
            void addCursor(float inX, float inY);
            void setCursor(const Vec2& inValue);
            void setCursor(float inX, float inY);

            // Collision
            const Bounds2D& getBounds() const;
            Bounds2D getDrawBounds() const;
            Bounds2D getOverflowClip() const;
            void getOverflowRoundClips(
                Vec4& outFirst,
                Vec4& outFirstRadiusX,
                Vec4& outFirstRadiusY,
                Vec4& outSecond,
                Vec4& outSecondRadiusX,
                Vec4& outSecondRadiusY
            ) const;

            // Draw
            bool hasPrimitive() const;
            const Primitive& getPrimitive() const;
            void clearPrimitive();
            void setPrimitive(const Primitive& inPrimitive);

            // Parsing
            String parseText(const String& inValue) const;

        protected:
            template <typename T>
            inline void import()
            {
                static_assert(std::is_base_of<Component, T>::value, "import requires a Grid::Component subclass");

                const ReflectionTypeInfo* type = ReflectionTypeRegistry::getInstance().find(typeid(T));
                if (!type)
                {
                    throw std::runtime_error("Grid component is not reflected");
                }

                String            selector = type->name;
                const std::size_t split    = selector.lastOf(':');
                if (split != String::npos)
                {
                    selector = selector.substr(split + 1);
                }

                m_imports.insert_or_assign(selector, type);
            }

            void load(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle = {});

            void refreshClassName();
            void refreshStyle();
            void refreshPosition();
            void refreshBounds();

            const StyleKeyframe::List* findKeyframes(const String& inName) const override;
            Drift::Clip makeAnimationClip(const StyleKeyframe::List& inKeyframes) const override;

            bool isReference(const String& inValue) const;
            String parseReference(const String& inValue) const;
            String parseMethod(const String& inValue) const;

            void addVariable(const String& inId, const ReflectionFieldAccessor& inValue);

            Component* cloneTemplate() const;
            const ReflectionTypeInfo* findImported(const String& inSelector) const;
            void syncForLoop(
                const String& inVariableId, const ReflectionFieldAccessor& inAccessor, const void* inInstance
            );

            bool isMethod(const String& inValue) const;
            ReflectionTypeMethod getMethod(const String& inId, const Component* inParamContext) const;
            void populateMethodParams(ReflectionTypeMethod& outMethod, const String& inSignature) const;
            std::vector<String> splitMethodParams(const String& inValue) const;

        protected:
            // Properties
            String                                                m_tag;
            String                                                m_id;
            String                                                m_className;

            // Modifier
            Directives                                            m_directives;

            // Runtime
            Variables                                             m_variables;

            // Style
            Style                                                 m_style;
            Style                                                 m_styleBase;
            bool                                                  m_bHasStyleBase;
            StyleFile::Variables                                  m_styleVariables;
            StyleFile*                                            m_styleFile;
            std::unique_ptr<StyleFile>                            m_styles;
            bool                                                  m_bOwnsStyle;

            // Imports
            std::unordered_map<String, const ReflectionTypeInfo*> m_imports;
            Component*                                            m_importOwner;

            // Hierarchy
            Component*                                            m_root;
            Component*                                            m_parent;
            std::vector<Component*>                               m_children;

            // Position
            Vec2                                                  m_size;
            Vec2                                                  m_scale;
            Vec2                                                  m_offset;
            Vec2                                                  m_cursor;
            float                                                 m_scratch;

            // Draw
            Primitive                                             m_primitive;

            // XML
            Xml::Attributes                                       m_attributes;
            pugi::xml_document                                    m_sourceDocument;
            pugi::xml_node                                        m_sourceNode;

            // For-loop
            std::vector<Component*>                               m_forInstances;
            String                                                m_forVariable;
            std::any                                              m_forSource;
            bool                                                  m_bSkipForDirective;
            bool                                                  m_bHovered;
            bool                                                  m_bFocused;
        };
    }
}