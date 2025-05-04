#pragma once

#include "Core.hpp"
#include "Grid/Function.hpp"
#include "Grid/Reference.hpp"
#include "Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE Component
        {
        public:
            typedef std::function<Component* (const pugi::xml_node& inNode)> Compiler;

        public:
            static Component* createComponent(const pugi::xml_node& inNode);
            static void registerComponent(const std::string& inTag, Compiler inCompiler);

        public:
            Component(const pugi::xml_node& inNode);
            Component(const std::string& inTag, const std::string& inId);

            virtual ~Component();

        public:
            virtual void onChildAddition(Component* inComponent) { return; }

        public:
            bool isValidChild(const Component* inComponent) const;

            const std::string& getTag() const;
            void setTag(const std::string& inTag);

            const std::string& getId() const;
            void setId(const std::string& inId);

            const std::string& getClass() const;
            void setClass(const std::string& inClass);

            const Style::Instance& getStyle() const;
            void setStyle(const Style::Sources& inSources);
            void setStyle(const Style::Properties& inSource);
            void setStyle(const Style::Instance& inStyle);

            const Vec<2, int>& getSize() const;
            void setSize(const Vec<2, int>& inSize);
            void setSize(int inWidth, int inHeight);

            const Vec<2, int>& getPosition() const;
            void setPosition(const Vec<2, int>& inPosition);
            void setPosition(int inX, int inY);

            bool hasReference(const std::string& inId) const;
            Reference* getReference(const std::string& inId) const;
            void addReference(const References& inReference);
            void addReference(const std::string& inId, Reference* inReference);
            void removeReference(const std::string& inId);

            bool hasFunction(const std::string& inId) const;
            const Function getFunction(const std::string& inId) const;
            void addFunction(const Functions& inFunctions);
            void addFunction(const std::string& inId, Function inFunction);
            void removeFunction(const std::string& inId);

            bool hasRoot() const;
            const Component* getRoot() const;
            void setRoot(const Component* inComponent);

            bool hasParent() const;
            const Component* getParent() const;
            void setParent(const Component* inComponent);

            bool hasChildren() const;
            const std::vector<Component*>& getChildren() const;
            void addChildren(const pugi::xml_node& inNode);
            void addChild(Component* inComponent);

        protected:
            std::string             m_tag;
            std::string             m_id;
            std::string             m_class;
            Style::Instance         m_style;

            References              m_references;
            Functions               m_functions;

            const Component*        m_root;
            const Component*        m_parent;
            std::vector<Component*> m_children;

            Vec<2, int>             m_size;
            Vec<2, int>             m_position;
        };
    }
}