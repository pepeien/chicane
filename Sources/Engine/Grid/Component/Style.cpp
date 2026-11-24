#include "Chicane/Grid/Component.reflected.hpp"

#include <algorithm>
#include <functional>
#include <string>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/Time.hpp"

#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Loop.hpp"
#include "Chicane/Drift/Track.hpp"

#include "Chicane/Grid/Component/LayoutMetrics.hpp"
#include "Chicane/Grid/Component/Loading.hpp"
#include "Chicane/Grid/Component/Projection.hpp"
#include "Chicane/Grid/Component/Scope.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Component/StyleMatch.hpp"
#include "Chicane/Grid/Component/View.hpp"
#include "Chicane/Grid/Component/Script.hpp"

namespace Chicane
{
    namespace Grid
    {
        void Component::refreshStyleRuleset()
        {
            if (m_bIsAnimationReady)
            {
                style.snapshot();
            }

            m_styleVariables.clear();

            style.resetValues();

            std::vector<StyleFile*> files;
            auto                    addFile = [&](StyleFile* file)
            {
                if (!file)
                {
                    return;
                }

                if (std::find(files.begin(), files.end(), file) != files.end())
                {
                    return;
                }

                files.push_back(file);
            };

            std::vector<StyleFile*> ancestors;
            for (Component* ancestor = m_parent; ancestor && ancestor != this;)
            {
                if (ancestor->m_styleFile)
                {
                    ancestors.push_back(ancestor->m_styleFile);
                }

                if (ancestor->m_bHasOwnStyle)
                {
                    ancestors.push_back(ancestor->m_styles.get());
                }

                Component* next = ancestor->m_parent;
                if (!next || next == ancestor)
                {
                    break;
                }

                ancestor = next;
            }

            for (auto it = ancestors.rbegin(); it != ancestors.rend(); it++)
            {
                addFile(*it);
            }

            addFile(m_styleFile);
            if (m_bHasOwnStyle)
            {
                addFile(m_styles.get());
            }

            const String inlineStyle =
                hasAttribute(STYLE_ATTRIBUTE_NAME) ? getAttribute(STYLE_ATTRIBUTE_NAME) : String::sEmpty();
            const bool bHasInlineStyle = !inlineStyle.trim().isEmpty();

            if (files.empty() && !bHasInlineStyle)
            {
                style.restore();

                return;
            }

            std::vector<StyleMatch> matches;
            std::uint32_t           order = 0;

            for (StyleFile* file : files)
            {
                const std::uint32_t origin =
                    (file == m_styleFile || (m_bHasOwnStyle && file == m_styles.get())) ? 1U : 0U;

                for (const StyleRuleset& source : file->getRulesets())
                {
                    if (source.isEmpty())
                    {
                        continue;
                    }

                    bool          bHasMatched = false;
                    std::uint32_t specificity = 0;

                    const bool bCompiledEmpty = static_cast<bool>(!source.compiled.empty());

                    if (bCompiledEmpty)
                    {
                        for (const StyleCompiledSelector& selector : source.compiled)
                        {
                            if (!matchesCompiledSelector(selector))
                            {
                                continue;
                            }

                            bHasMatched = true;
                            specificity = std::max(specificity, selector.specificity());
                        }
                    }

                    if (!bCompiledEmpty)
                    {
                        for (const String& selector : source.selectors)
                        {
                            if (!hasSelector(selector.trim()))
                            {
                                continue;
                            }

                            bHasMatched = true;
                        }
                    }

                    if (!bHasMatched)
                    {
                        continue;
                    }

                    matches.push_back({origin, specificity, order++, &source});
                }
            }

            std::stable_sort(
                matches.begin(),
                matches.end(),
                [](const StyleMatch& inLeft, const StyleMatch& inRight)
                {
                    if (inLeft.specificity != inRight.specificity)
                    {
                        return inLeft.specificity < inRight.specificity;
                    }

                    if (inLeft.origin != inRight.origin)
                    {
                        return inLeft.origin < inRight.origin;
                    }

                    return inLeft.order < inRight.order;
                }
            );

            StyleRuleset::Properties properties;
            for (const StyleMatch& match : matches)
            {
                for (const auto& [key, value] : match.source->properties)
                {
                    if (key.startsWith(Style::VARIABLE_KEYWORD))
                    {
                        m_styleVariables[key.substr(1)] = value;

                        continue;
                    }

                    properties[key] = value;
                }
            }

            if (bHasInlineStyle)
            {
                for (const auto& [key, value] : parseDeclarations(inlineStyle))
                {
                    if (key.startsWith(Style::VARIABLE_KEYWORD))
                    {
                        m_styleVariables[key.substr(1)] = value;

                        continue;
                    }

                    properties[key] = value;
                }
            }

            if (!properties.empty())
            {
                addStyleProperties(properties);
            }

            if (hasParent() && !isRoot() && properties.find(Style::FOREGROUND_COLOR_ATTRIBUTE_NAME) == properties.end())
            {
                style.foregroundColor.copyValue(m_parent->getStyle().foregroundColor);
            }

            m_live = String::sEmpty();
            for (const auto& [key, value] : properties)
            {
                if (value.contains("ref(") || isReference(value))
                {
                    if (!m_live.isEmpty())
                    {
                        m_live.append('\n');
                    }

                    m_live.append(value);
                }
            }

            setFlag(ComponentDirty::LiveBind, isReference(getAttribute(CLASS_ATTRIBUTE_NAME)) || !m_live.isEmpty());

            style.restore();
        }

        bool Component::hasStyleFile() const
        {
            return m_styleFile != nullptr;
        }

        const StyleFile* Component::getStyleFile() const
        {
            return m_styleFile;
        }

        void Component::setStyleFile(StyleFile* inSource)
        {
            m_styleFile = inSource;

            refreshStyleRuleset();

            for (Component* child : m_children)
            {
                child->setStyleFile(child->m_bHasOwnStyle ? child->m_styleFile : inSource);
            }
        }

        void Component::importStyleFile(const FileSystem::Path& inValue)
        {
            if (inValue.isEmpty())
            {
                return;
            }

            if (!m_styles)
            {
                m_styles = std::make_unique<StyleFile>();
            }

            m_styles->parse(inValue);
            m_bHasOwnStyle = true;
            setStyleFile(m_styles.get());
        }

        void Component::addStyleRuleset(const StyleRuleset::List& inSources)
        {
            if (!hasStyleFile())
            {
                return;
            }

            m_styleFile->addRuleset(inSources);

            refreshStyleRuleset();
        }

        void Component::addStyleProperties(const StyleRuleset::Properties& inSource)
        {
            style.setProperties(inSource);
        }

        const Style& Component::getStyle() const
        {
            return style;
        }

        float Component::getOpacity() const
        {
            return m_draw.opacity;
        }

        float Component::getFilterBlur() const
        {
            return m_draw.filterBlur;
        }

        const String& Component::getStyleVariable(const String& inName) const
        {
            const auto found = m_styleVariables.find(inName);
            if (found != m_styleVariables.end())
            {
                return found->second;
            }

            if (hasStyleFile())
            {
                const String& value = m_styleFile->getVariable(inName);
                if (!value.isEmpty())
                {
                    return value;
                }
            }

            if (hasParent() && !isRoot())
            {
                return m_parent->getStyleVariable(inName);
            }

            return String::sEmpty();
        }

        void Component::refreshStyleSubtree()
        {
            refreshStyleRuleset();

            for (Component* child : m_children)
            {
                child->refreshStyleSubtree();
            }
        }

        static bool matchesSiblingSelector(const Component& inComponent, const StyleSiblingSelector& inSelector);

        bool Component::hasLocalSelector(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return false;
            }

            String                            value = inValue.trim();
            std::vector<StyleSiblingSelector> siblings;
            ComponentStatus                   required = Style::sConsumePseudoClasses(value, &siblings);

            if (!hasAll(getStatus(), required))
            {
                return false;
            }

            for (const StyleSiblingSelector& sibling : siblings)
            {
                if (!matchesSiblingSelector(*this, sibling))
                {
                    return false;
                }
            }

            value = value.trim();

            if (value.isEmpty() || value.equals(Style::INCLUSIVE_SELECTOR))
            {
                return true;
            }

            String              tag;
            String              id;
            std::vector<String> classes;

            std::size_t cursor = value.firstOfChars(Style::CLASS_SELECTOR, Style::ID_SELECTOR);

            const bool bCursorMissing = static_cast<bool>(cursor == String::npos);

            if (bCursorMissing)
            {
                tag = value;
            }

            if (!bCursorMissing)
            {
                if (cursor > 0)
                {
                    tag = value.substr(0, cursor);
                }

                value = value.substr(cursor);
            }

            while (value.startsWithChars(Style::CLASS_SELECTOR, Style::ID_SELECTOR))
            {
                const char prefix = value.at(0);

                std::size_t next = String::npos;

                for (std::size_t i = 1; i < value.size(); i++)
                {
                    const char c = value.at(i);

                    if (c == Style::CLASS_SELECTOR || c == Style::ID_SELECTOR)
                    {
                        next = i;
                        break;
                    }
                }

                const std::size_t length = (next == String::npos) ? value.size() : next;

                const String token = value.substr(0, length);

                const bool bPrefixClass = static_cast<bool>(prefix == Style::CLASS_SELECTOR);

                if (bPrefixClass)
                {
                    classes.push_back(token);
                }

                const bool bPrefixId = !bPrefixClass && (prefix == Style::ID_SELECTOR);

                if (bPrefixId)
                {
                    id = token;
                }

                if (next == String::npos)
                {
                    break;
                }

                value = value.substr(next);
            }

            if (!tag.isEmpty() && !tag.equals(getTag()))
            {
                return false;
            }

            if (!id.isEmpty())
            {
                if (!id.equals(Style::ID_SELECTOR + getId()))
                {
                    return false;
                }
            }

            for (const String& className : classes)
            {
                if (!classList.contains(className))
                {
                    return false;
                }
            }

            return true;
        }

        bool Component::hasSelector(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return false;
            }

            std::vector<String> parts = inValue.split(Style::SELECTOR_SEPARATOR_SPACE);
            if (parts.empty())
            {
                return false;
            }

            if (!hasLocalSelector(parts.back().trim()))
            {
                return false;
            }

            if (parts.size() == 1)
            {
                return true;
            }

            Component* ancestor = m_parent;
            int        index    = static_cast<int>(parts.size()) - 2;
            while (ancestor && ancestor != this && index >= 0)
            {
                if (ancestor->hasLocalSelector(parts[index].trim()))
                {
                    index--;
                }

                Component* next = ancestor->m_parent;
                if (!next || next == ancestor)
                {
                    break;
                }

                ancestor = next;
            }

            return index < 0;
        }

        static bool matchesSiblingSelector(const Component& inComponent, const StyleSiblingSelector& inSelector)
        {
            const Component* parent = inComponent.getParent();
            if (!parent)
            {
                return false;
            }

            int position = 0;
            int count    = 0;

            for (Component* sibling : parent->getChildren())
            {
                if (!sibling)
                {
                    continue;
                }

                if (inSelector.bOfType && !sibling->getTag().equals(inComponent.getTag()))
                {
                    continue;
                }

                count++;

                if (sibling == &inComponent)
                {
                    position = count;
                }
            }

            if (position <= 0)
            {
                return false;
            }

            const int index = inSelector.bFromEnd ? (count - position + 1) : position;
            if (index <= 0)
            {
                return false;
            }

            if (inSelector.step == 0)
            {
                return index == inSelector.offset;
            }

            const int delta = index - inSelector.offset;
            if (delta % inSelector.step != 0)
            {
                return false;
            }

            return delta / inSelector.step >= 0;
        }

        bool Component::matchesCompiledSelector(const StyleCompiledSelector& inSelector) const
        {
            if (inSelector.chain.empty())
            {
                return false;
            }

            if (!matchesCompiledPart(inSelector.chain.back()))
            {
                return false;
            }

            if (inSelector.chain.size() == 1)
            {
                return true;
            }

            Component* ancestor = m_parent;
            int        index    = static_cast<int>(inSelector.chain.size()) - 2;

            while (ancestor && ancestor != this && index >= 0)
            {
                if (ancestor->matchesCompiledPart(inSelector.chain[static_cast<std::size_t>(index)]))
                {
                    index--;
                }

                Component* next = ancestor->m_parent;
                if (!next || next == ancestor)
                {
                    break;
                }

                ancestor = next;
            }

            return index < 0;
        }

        bool Component::matchesCompiledPart(const StyleSelectorPart& inPart) const
        {
            if (!hasAll(getStatus(), inPart.status))
            {
                return false;
            }

            for (const StyleSiblingSelector& sibling : inPart.siblings)
            {
                if (!matchesSiblingSelector(*this, sibling))
                {
                    return false;
                }
            }

            if (!inPart.tag.isEmpty() && !inPart.tag.equals(getTag()))
            {
                return false;
            }

            if (!inPart.id.isEmpty() && !inPart.id.equals(Style::ID_SELECTOR + getId()))
            {
                return false;
            }

            if (!inPart.classes.empty())
            {
                for (const String& className : inPart.classes)
                {
                    if (!classList.contains(className))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        void Component::refreshStyle()
        {
            style.refresh();
        }

        const StyleKeyframe::List* Component::findKeyframes(const String& inName) const
        {
            std::vector<StyleFile*> files;

            auto addFile = [&](StyleFile* inFile)
            {
                if (!inFile)
                {
                    return;
                }

                if (std::find(files.begin(), files.end(), inFile) != files.end())
                {
                    return;
                }

                files.push_back(inFile);
            };

            std::vector<StyleFile*> ancestors;
            for (const Component* ancestor = m_parent; ancestor && ancestor != this; ancestor = ancestor->m_parent)
            {
                addFile(ancestor->m_styleFile);

                if (ancestor->isRoot())
                {
                    break;
                }
            }

            for (auto iterator = ancestors.rbegin(); iterator != ancestors.rend(); iterator++)
            {
                addFile(*iterator);
            }

            addFile(m_styleFile);

            if (m_bHasOwnStyle)
            {
                addFile(m_styles.get());
            }

            for (auto iterator = files.rbegin(); iterator != files.rend(); iterator++)
            {
                if ((*iterator)->hasKeyframes(inName))
                {
                    return &(*iterator)->getKeyframes(inName);
                }
            }

            return nullptr;
        }

        Drift::Clip Component::makeAnimationClip(const StyleKeyframe::List& inKeyframes) const
        {
            Drift::Clip clip(style.animation.name);
            clip.duration   = Time::sFromMilliseconds(style.animation.duration);
            clip.iterations = style.animation.iterations;
            clip.loop       = style.animation.bIsAlternate
                                  ? Drift::Loop::PingPong
                                  : (style.animation.iterations == 1 ? Drift::Loop::Once : Drift::Loop::Repeat);

            std::unordered_map<String, Drift::Track> tracks;

            for (const StyleKeyframe& frame : inKeyframes)
            {
                Style snapshot;
                snapshot.setParent(this);
                snapshot.setProperties(frame.properties);
                snapshot.refresh();

                const Time time = clip.duration * frame.offset;

                for (const auto& [name, value] : frame.properties)
                {
                    StylePropertyId id = StylePropertyId::Count;

                    if (!StylePropertyTable::sFind(name, id))
                    {
                        continue;
                    }

                    float values[MAX_PROPERTY_ARITY] = {};

                    if (!snapshot.readAnimated(id, values))
                    {
                        continue;
                    }

                    const std::vector<float> parsed(values, values + StylePropertyTable::sGet(id).arity);

                    auto found = tracks.find(name);

                    if (found == tracks.end())
                    {
                        tracks.insert({name, Drift::Track(name)});
                        found = tracks.find(name);
                    }

                    found->second.addKeyframe(time, parsed, style.animation.easing);
                }
            }

            for (const auto& [name, track] : tracks)
            {
                clip.addTrack(track);
            }

            return clip;
        }
    }
}
