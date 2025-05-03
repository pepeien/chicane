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
            Component(const pugi::xml_node& inNode);
            Component(const std::string& inId);

            virtual ~Component() = default;

        public:
            void render() const;

            const std::string& getId() const;

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

            bool hasParent() const;
            const Component* getParent() const;
            void setParent(const Component* inParent);

            bool hasChildren() const;
            const std::vector<Component*>& getChildren() const;
            void addChild(Component* inChild);

        protected:
            std::string             m_id;
            Style                   m_style;

            References              m_references;
            Functions               m_functions;

            const Component*        m_parent;
            std::vector<Component*> m_children;

            Vec<2, int>             m_size;
            Vec<2, int>             m_position;
        };
    }
}