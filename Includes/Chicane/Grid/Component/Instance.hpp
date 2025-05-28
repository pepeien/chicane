#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Function.hpp"
#include "Chicane/Grid/Primitive.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Component : public Changeable
        {
        public:
            using Compiler = std::function<Component* (const pugi::xml_node& inNode)>;

            using ChildrenObservable   = Observable<Component*>;
            using ChildrenSubscription = Subscription<Component*>;

        protected:
            using Super = Component;

        public:
            Component(const pugi::xml_node& inNode);
            Component(const std::string& inTag);

            virtual ~Component();

        public:
            // Status
            virtual bool isDrawable() const;

        protected:
            // Events
            virtual void onEvent(const Window::Event& inEvent) { return; }
            virtual void onTick(float inDelta) { return; }
            virtual void onRefresh() { return; }
            virtual void onAdoption(Component* inChild) { return; }
            virtual void onAdopted(Component* inParent) { return; }

            virtual void refreshPrimitive() { return; }

        public:
            // Checkers
            bool isRoot() const;
            bool isVisible() const;
            bool isValidChild(Component* inComponent) const;

            // Lifecycle
            void tick(float inDelta);
            void refresh();

            // Properties
            const std::string& getTag() const;
            void setTag(const std::string& inTag);

            const std::string& getId() const;
            void setId(const std::string& inId);

            const std::vector<std::string> getClasses() const;
            const std::string& getClass() const;
            void setClass(const std::string& inClass);

            const Style::Instance& getStyle() const;
            void setStyle(const Style::Source::List& inSources);
            void setStyle(const Style::Properties& inSource);

            bool hasReference(const std::string& inId, bool isLocalOnly = false) const;
            Reference* getReference(const std::string& inId) const;
            void addReference(const Reference::Map& inReference);
            void addReference(const std::string& inId, Reference* inReference);
            void removeReference(const std::string& inId);

            bool hasFunction(const std::string& inId, bool isLocalOnly = false) const;
            const Function getFunction(const std::string& inId) const;
            void addFunction(const Functions& inFunctions);
            void addFunction(const std::string& inId, Function inFunction);
            void removeFunction(const std::string& inId);

            bool hasRoot() const;
            Component* getRoot() const;
            void setRoot(Component* inComponent);

            bool hasParent() const;
            Component* getParent() const;
            void setParent(Component* inComponent);

            bool hasChildren() const;
            const std::vector<Component*>& getChildren() const;
            void addChildren(const pugi::xml_node& inNode);
            void addChild(Component* inComponent);
            ChildrenSubscription watchChildren(
                ChildrenSubscription::NextCallback inNext,
                ChildrenSubscription::ErrorCallback inError = nullptr,
                ChildrenSubscription::CompleteCallback inComplete = nullptr
            );

            // Positioning
            const Vec2& getCursor() const;
            void addCursor(const Vec2& inCursor);
            void addCursor(float inX, float inY);
            void setCursor(const Vec2& inCursor);
            void setCursor(float inX, float inY);

            Vec2 getAvailableSize() const;
            const Vec2& getSize() const;
            void setSize(const Vec2& inSize);
            void setSize(float inWidth, float inHeight);

            const Vec2& getPosition() const;
            void setPosition(const Vec2& inPosition);
            void setPosition(float inX, float inY);

            bool hasPrimitive() const;
            const Primitive& getPrimitive() const;
            void clearPrimitive();
            void setPrimitive(const Primitive& inPrimitive);

            std::string parseText(const std::string& inValue) const;

        protected:
            void refreshStyle();
            void refreshSize();
            void refreshPosition();

            bool isReference(const std::string& inValue) const;
            Reference parseReference(const std::string& inValue) const;

            bool isFunction(const std::string& inValue) const;
            FunctionData parseFunction(const std::string& inRefValue) const;

        protected:
            std::string             m_tag;
            std::string             m_id;
            std::string             m_class;
            Style::Instance         m_style;

            Reference::Map          m_references;
            Functions               m_functions;

            Component*              m_root;
            Component*              m_parent;
            std::vector<Component*> m_children;
            ChildrenObservable      m_childrenObservable;

            Vec2                    m_size;
            Vec2                    m_position;
            Vec2                    m_cursor;

            Primitive               m_primitive;
        };
    }
}