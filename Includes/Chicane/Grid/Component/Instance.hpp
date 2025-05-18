#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Function.hpp"
#include "Chicane/Grid/Reference.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Component : public Changeable
        {
        public:
            typedef std::function<Component* (const pugi::xml_node& inNode)> Compiler;

        public:
            Component(const pugi::xml_node& inNode);
            Component(const std::string& inTag);

            virtual ~Component();

        public:
            virtual bool isDrawable() const;

            virtual void onEvent(const SDL_Event& inEvent) { return; }

            // Lifecycle
            virtual void onParentAddition(Component* inComponent) { return; }
            virtual void onChildAddition(Component* inComponent) { return; }
            virtual void onTick(float inDelta) { return; }

        protected:
            virtual void refreshPrimitive() { return; }

        public:
            // Checkers
            bool isRoot() const;
            bool isVisible() const;
            bool isValidChild(Component* inComponent) const;

            // Lifecycle
            void tick(float inDelta);

            // Properties
            const std::string& getTag() const;
            void setTag(const std::string& inTag);

            const std::string& getId() const;
            void setId(const std::string& inId);

            const std::vector<std::string> getClasses() const;
            const std::string& getClass() const;
            void setClass(const std::string& inClass);

            const Style::Instance& getStyle() const;
            void setStyle(const Style::Sources& inSources);
            void setStyle(const Style::Properties& inSource);

            bool hasReference(const std::string& inId, bool isLocalOnly = false) const;
            Reference* getReference(const std::string& inId) const;
            void addReference(const References& inReference);
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

            // Positioning
            const Vec<2, float>& getCursor() const;
            void addCursor(const Vec<2, float>& inCursor);
            void setCursor(const Vec<2, float>& inCursor);

            Vec<2, float> getAvailableSize() const;
            const Vec<2, float>& getSize() const;
            void setSize(const Vec<2, float>& inSize);
            void setSize(int inWidth, int inHeight);

            const Vec<2, float>& getPosition() const;
            void setPosition(const Vec<2, float>& inPosition);
            void setPosition(int inX, int inY);

            bool isPrimitiveEmpty() const;
            std::uint32_t getPrimitiveCount() const;
            const std::vector<Vertex>& getPrimitive() const;

            std::string parseText(const std::string& inValue) const;

        protected:
            void refreshStyle();
            void refreshSize();
            void refreshPosition();
            void refresh();

            bool isReference(const std::string& inValue) const;
            Reference parseReference(const std::string& inValue) const;

            bool isFunction(const std::string& inValue) const;
            FunctionData parseFunction(const std::string& inRefValue) const;

        protected:
            std::string             m_tag;
            std::string             m_id;
            std::string             m_class;
            Style::Instance         m_style;

            References              m_references;
            Functions               m_functions;

            Component*              m_root;
            Component*              m_parent;
            std::vector<Component*> m_children;

            Vec<2, float>           m_size;
            Vec<2, float>           m_position;
            Vec<2, float>           m_cursor;

            std::vector<Vertex>     m_primitive;
        };
    }
}