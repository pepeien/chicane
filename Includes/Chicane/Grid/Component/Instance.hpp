#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Function.hpp"
#include "Chicane/Grid/Reference.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE Component
        {
        public:
            typedef std::function<Component* (const pugi::xml_node& inNode)> Compiler;

        public:
            Component(const pugi::xml_node& inNode);
            Component(const std::string& inTag);

            virtual ~Component();

        public:
            virtual void onEvent(const SDL_Event& inEvent) { return; }
            virtual void onChildAddition(Component* inComponent);
            virtual void onTick(float inDelta);

        public:
            // Checkers
            bool isRoot() const;
            bool isVisible() const;
            bool isDrawable() const;
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
            void setStyle(const Style::Instance& inStyle);

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

            const Vec<2, float>& getSize() const;
            void setSize(const Vec<2, float>& inSize);
            void setSize(int inWidth, int inHeight);
            Subscription<const Vec<2, float>&>* watchSize(
                std::function<void (const Vec<2, float>&)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            ) const;

            const Vec<2, float>& getPosition() const;
            void setPosition(const Vec<2, float>& inPosition);
            void setPosition(int inX, int inY);
            Subscription<const Vec<2, float>&>* watchPosition(
                std::function<void (const Vec<2, float>&)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            ) const;

            // Style Properties
            bool isDisplayStyle(Style::Display inDisplay) const;
            Style::Display getDisplayStyle() const;

            float getWidthStyle() const;
            float getHeightStyle() const;

            bool isPositionStyle(Style::Position inPosition) const;
            Style::Position getPositionStyle() const;

            std::string getBackgroundColorStyle() const;

        protected:
            void refreshSize();
            void refreshPosition();

            float calculateSize(const std::string& inValue, Style::Direction inDirection) const;

            float calculateSizeFromCalculation(
                const std::string& inValue,
                Style::Direction inDirection
            ) const;

            float calculateSizeFromPercentage(const std::string& inValue, Style::Direction inDirection) const;
            float calculateSizeFromPercentage(float inValue, Style::Direction inDirection) const;

            float calculateSizeFromViewportHeight(const pugi::xml_attribute& inAttribute) const;
            float calculateSizeFromViewportHeight(const std::string& inValue) const;
            float calculateSizeFromViewportHeight(float inValue) const;

            float calculateSizeFromViewportWidth(const pugi::xml_attribute& inAttribute) const;
            float calculateSizeFromViewportWidth(const std::string& inValue) const;
            float calculateSizeFromViewportWidth(float inValue) const;

            float calculateSizeFromPixel(const pugi::xml_attribute& inAttribute) const;
            float calculateSizeFromPixel(const std::string& inValue) const;

            std::string parseText(const std::string& inText) const;

            bool doesTextContainsReference(const std::string& inText) const;
            Reference parseReference(const std::string& inValue) const;

            bool doesTextContainsFunction(const std::string& inValue) const;
            FunctionData parseFunction(const std::string& inRefValue) const;

        protected:
            std::string                                       m_tag;
            std::string                                       m_id;
            std::string                                       m_class;
            Style::Instance                                   m_style;

            References                                        m_references;
            Functions                                         m_functions;

            Component*                                        m_root;
            Component*                                        m_parent;
            std::vector<Component*>                           m_children;

            Vec<2, float>                                     m_cursor;

            Vec<2, float>                                     m_size;
            std::unique_ptr<Observable<const Vec<2, float>&>> m_sizeObservable;
            Vec<2, float>                                     m_position;
            std::unique_ptr<Observable<const Vec<2, float>&>> m_positionObservable;
        };
    }
}