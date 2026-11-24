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
        void Component::refreshDirectives()
        {
            const auto ifAttribute  = m_attributes.find(IF_DIRECTIVE_KEYWORD);
            const auto forAttribute = m_attributes.find(FOR_DIRECTIVE_KEYWORD);
            const bool bHasIf =
                ifAttribute != m_attributes.end() && !ifAttribute->second.isEmpty();
            const bool bHasFor =
                forAttribute != m_attributes.end() && !forAttribute->second.isEmpty();

            if (!bHasIf && !bHasFor)
            {
                return;
            }

            if (bHasIf)
            {
                runDirective(IF_DIRECTIVE_KEYWORD, ifAttribute->second);
            }

            if (bHasFor)
            {
                runDirective(FOR_DIRECTIVE_KEYWORD, forAttribute->second);
            }
        }

        void Component::runDirective(const String& inKey, const String& inValue)
        {
            const auto& found = m_directives.find(inKey);

            if (found == m_directives.end() || !found->second)
            {
                return;
            }

            found->second(inValue);
        }

        void Component::setDirective(const String& inKey, const Directive& inValue)
        {
            m_directives[inKey] = inValue;
        }

        ReflectionFieldAccessor Component::getField(const String& inId) const
        {
            if (inId.isEmpty())
            {
                return {};
            }

            const auto& local = m_variables.find(inId);

            if (local != m_variables.end())
            {
                return local->second;
            }

            const std::vector<String> parts = inId.split('.');
            if (parts.size() > 1)
            {
                const auto& variable = m_variables.find(parts.at(0));

                if (variable != m_variables.end())
                {
                    const ReflectionFieldAccessor& base = variable->second;

                    if (!base.isValid() || !base.typeIndex.has_value())
                    {
                        return {};
                    }

                    const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(base.typeIndex.value());

                    if (!type)
                    {
                        return {};
                    }

                    const String            subPath = inId.substr(parts.at(0).size() + 1);
                    ReflectionFieldAccessor result  = type->resolve(subPath);

                    if (!result.isValid())
                    {
                        return {};
                    }

                    if (base.boundInstance != nullptr)
                    {
                        result.boundInstance = base.boundInstance;
                        result.bNeedsDeref   = false;
                    }

                    return result;
                }
            }

            if (const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*this)))
            {
                const ReflectionFieldAccessor result = type->resolve(inId);

                if (result.isValid())
                {
                    return result;
                }
            }

            return {};
        }

        ReflectionTypeMethod Component::getMethod(const String& inValue) const
        {
            return getMethod(inValue, this);
        }

        bool Component::hideIfDirective()
        {
            const String& attribute = getAttribute(IF_DIRECTIVE_KEYWORD);
            if (attribute.isEmpty())
            {
                return false;
            }

            const bool bShouldShow = parseText(attribute).equals("true", "1");
            const bool bWasHidden  = style.isDisplay(StyleDisplay::None);
            if (bShouldShow)
            {
                if (bWasHidden)
                {
                    markStyleDirty();
                    markLayoutDirty();

                    if (m_parent)
                    {
                        m_parent->markLayoutDirty();
                    }
                }

                return false;
            }

            if (!bWasHidden && m_parent)
            {
                m_parent->markLayoutDirty();
            }

            style.display.set(StyleDisplay::None);
            setCulled(false);
            setFlag(ComponentDirty::Style, false);
            setFlag(ComponentDirty::Layout, false);

            return true;
        }

        String Component::parseText(const String& inValue) const
        {
            if (inValue.isEmpty() || !isReference(inValue))
            {
                return inValue;
            }

            const std::size_t openPosition = inValue.firstOf(REFERENCE_VALUE_OPENING);
            if (openPosition == String::npos)
            {
                return inValue;
            }

            const std::size_t closePosition = inValue.firstOf(REFERENCE_VALUE_CLOSING, openPosition + 2);
            if (closePosition == String::npos)
            {
                return inValue;
            }

            const std::size_t valueStart = openPosition + 2;

            String result;

            const String prefix = inValue.substr(0, openPosition);
            if (!prefix.isEmpty())
            {
                result.append(parseText(prefix));
            }

            const String value = inValue.substr(valueStart, closePosition - valueStart).trim();
            if (!value.isEmpty())
            {
                result.append(parseReference(value));
            }

            const String suffix = inValue.substr(closePosition + 2);
            if (!suffix.isEmpty())
            {
                result.append(parseText(suffix));
            }

            return result;
        }

        void Component::refreshClassName()
        {
            const auto found = m_attributes.find(CLASS_ATTRIBUTE_NAME);
            if (found == m_attributes.end())
            {
                return;
            }

            const String className = parseText(found->second);
            if (className.equals(getClassName()))
            {
                return;
            }

            setClassName(className);
        }

        void Component::refreshId()
        {
            const auto found = m_attributes.find(ID_ATTRIBUTE_NAME);
            if (found == m_attributes.end() || !isReference(found->second))
            {
                return;
            }

            const String id = parseText(found->second).trim();
            if (id.equals(m_id))
            {
                return;
            }

            setId(id);
        }

        bool Component::isReference(const String& inValue) const
        {
            const bool bHasOpening = inValue.firstOf(REFERENCE_VALUE_OPENING) != String::npos;
            const bool bHasClosing = inValue.lastOf(REFERENCE_VALUE_CLOSING) != String::npos;

            return bHasOpening && bHasClosing;
        }

        String Component::parseReference(const String& inValue) const
        {
            if (inValue.startsWith('!') && !inValue.startsWith("!="))
            {
                const String inner = parseReference(inValue.substr(1).trim());

                return inner.equals("true", "1") ? "false" : "true";
            }

            const std::size_t equals = inValue.firstOf("==");
            if (equals != String::npos)
            {
                const String left  = parseReference(inValue.substr(0, equals).trim());
                const String right = parseReference(inValue.substr(equals + 2).trim());

                return left.equals(right) ? "true" : "false";
            }

            if (isMethod(inValue))
            {
                return parseMethod(inValue);
            }

            ReflectionFieldAccessor accessor = getField(inValue);

            if (accessor.isValid())
            {
                const void* instance =
                    accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(this);

                return accessor.toString(instance);
            }

            return hasParent() ? m_parent->parseReference(inValue) : inValue;
        }

        static std::size_t findMatchingClose(const String& inValue, std::size_t inOpen)
        {
            if (inOpen >= inValue.size() || inValue.at(inOpen) != METHOD_PARAMS_OPENING)
            {
                return String::npos;
            }

            std::uint32_t depth = 0;
            for (std::size_t i = inOpen; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);
                if (character == METHOD_PARAMS_OPENING)
                {
                    depth++;

                    continue;
                }

                if (character != METHOD_PARAMS_CLOSING || depth == 0)
                {
                    continue;
                }

                depth--;
                if (depth == 0)
                {
                    return i;
                }
            }

            return String::npos;
        }

        String Component::parseMethod(const String& inValue) const
        {
            const std::size_t open = inValue.firstOf(METHOD_PARAMS_OPENING);
            if (open == String::npos)
            {
                return String::sEmpty();
            }

            const std::size_t close = findMatchingClose(inValue, open);
            if (close == String::npos)
            {
                return String::sEmpty();
            }

            const String qualified = inValue.substr(0, open).trim();
            if (qualified.isEmpty())
            {
                return String::sEmpty();
            }

            const std::size_t dot      = qualified.lastOf('.');
            const String      receiver = dot == String::npos ? String::sEmpty() : qualified.substr(0, dot);
            const String      name     = dot == String::npos ? qualified : qualified.substr(dot + 1);
            if (name.isEmpty())
            {
                return String::sEmpty();
            }

            const String signature = inValue.substr(0, close + 1);
            const String postfix   = inValue.substr(close + 1).trim();

            for (const Component* node = this; node != nullptr; node = node->hasParent() ? node->getParent() : nullptr)
            {
                const ReflectionTypeInfo* type     = nullptr;
                void*                     instance = nullptr;

                const bool bReceiverEmpty = static_cast<bool>(receiver.isEmpty());

                if (bReceiverEmpty)
                {
                    type     = ReflectionTypeRegistry::sInstance().find(typeid(*node));
                    instance = const_cast<Component*>(node);
                }

                if (!bReceiverEmpty)
                {
                    const ReflectionFieldAccessor accessor = node->getField(receiver);
                    if (!accessor.isValid() || !accessor.typeIndex.has_value())
                    {
                        if (node->isRoot())
                        {
                            break;
                        }

                        continue;
                    }

                    type     = ReflectionTypeRegistry::sInstance().find(accessor.typeIndex.value());
                    instance = const_cast<char*>(accessor.address(node));

                    if (name.equals("isEmpty") && accessor.isType<String>())
                    {
                        const String* value = accessor.getValue<String>(node);

                        return (!value || value->isEmpty()) ? "true" : "false";
                    }
                }

                if (type && instance)
                {
                    if (const ReflectionTypeMethodInfo* method = type->findMethod(name))
                    {
                        try
                        {
                            ReflectionTypeMethod call(method);
                            call.bind(instance);

                            populateMethodParams(call, signature);

                            return method->toString(call.invoke(), postfix);
                        }
                        catch (const std::exception&)
                        {
                            return String::sEmpty();
                        }
                    }
                }

                if (node->isRoot())
                {
                    break;
                }
            }

            return String::sEmpty();
        }

        void Component::addVariable(const String& inId, const ReflectionFieldAccessor& inValue)
        {
            if (!inValue.isValid())
            {
                m_variables.erase(inId);

                return;
            }

            m_variables[inId] = std::move(inValue);
        }

        void Component::syncForLoop(
            const String& inVariableId, const ReflectionFieldAccessor& inAccessor, const void* inInstance
        )
        {
            if (!hasParent())
            {
                return;
            }

            Component*        parent = getParent();
            const std::size_t count  = inAccessor.getSize(inInstance);

            if (m_forInstances.empty())
            {
                m_forInstances.push_back(this);
            }

            bool bWasMutated = false;

            while (m_forInstances.size() < count)
            {
                Component* instance = cloneTemplate();
                if (!instance)
                {
                    break;
                }

                const std::vector<Component*>& siblings = parent->getChildren();
                const auto        found = std::find(siblings.begin(), siblings.end(), m_forInstances.back());
                const std::size_t index = found == siblings.end()
                                              ? siblings.size()
                                              : static_cast<std::size_t>(std::distance(siblings.begin(), found) + 1);

                parent->addChild(instance, index);
                m_forInstances.push_back(instance);
                bWasMutated = true;
            }

            while (m_forInstances.size() > std::max(count, static_cast<std::size_t>(1)))
            {
                Component* extra = m_forInstances.back();
                m_forInstances.pop_back();

                if (extra == this)
                {
                    m_forInstances.insert(m_forInstances.begin(), extra);

                    break;
                }

                parent->removeChild(extra);

                bWasMutated = true;
            }

            for (std::size_t i = 0; i < m_forInstances.size(); i++)
            {
                Component* instance = m_forInstances.at(i);

                if (i >= count)
                {
                    instance->style.display.set(StyleDisplay::None);
                    instance->m_variables.erase(inVariableId);

                    continue;
                }

                ReflectionFieldAccessor element = inAccessor.getElement(inInstance, i);

                if (!element.isValid())
                {
                    instance->style.display.set(StyleDisplay::None);

                    continue;
                }

                instance->addVariable(inVariableId, element);

                const bool bWasHidden = instance->getStyle().isDisplay(StyleDisplay::None);
                instance->style.display.set(StyleDisplay::Flex);

                if (bWasHidden)
                {
                    instance->markStyleDirtySubtree();
                    instance->markLayoutDirtySubtree();
                    bWasMutated = true;
                }
            }

            if (bWasMutated)
            {
                parent->markLayoutDirty();
                parent->rebuildPaintChildren();
            }
        }

        bool Component::isMethod(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return false;
            }

            const std::size_t open = inValue.firstOf(METHOD_PARAMS_OPENING);
            if (open == String::npos)
            {
                return false;
            }

            return findMatchingClose(inValue, open) != String::npos;
        }

        static bool parseLuaStringArgs(const String& inSignature, std::vector<String>& outArgs)
        {
            outArgs.clear();

            const std::size_t open  = inSignature.firstOf(METHOD_PARAMS_OPENING);
            const std::size_t close = inSignature.lastOf(METHOD_PARAMS_CLOSING);
            if (open == String::npos || close == String::npos || close <= open)
            {
                return false;
            }

            const String raw = inSignature.substr(open + 1, close - open - 1).trim();
            if (raw.isEmpty())
            {
                return true;
            }

            const char*       cursor = raw.toChar();
            const char* const end    = cursor + raw.size();

            while (cursor < end)
            {
                while (cursor < end && (*cursor == ' ' || *cursor == '\t'))
                {
                    cursor++;
                }

                if (cursor >= end)
                {
                    break;
                }

                const char quote = *cursor;
                if (quote != '"' && quote != '\'')
                {
                    return false;
                }

                cursor++;
                const char* start = cursor;
                while (cursor < end && *cursor != quote)
                {
                    cursor++;
                }

                if (cursor >= end)
                {
                    return false;
                }

                outArgs.emplace_back(String(std::string(start, static_cast<std::size_t>(cursor - start))));
                cursor++;

                while (cursor < end && (*cursor == ' ' || *cursor == '\t'))
                {
                    cursor++;
                }

                if (cursor >= end)
                {
                    return true;
                }

                if (*cursor != METHOD_PARAMS_SEPARATOR)
                {
                    return false;
                }

                cursor++;
            }

            return true;
        }

        ReflectionTypeMethod Component::getMethod(const String& inValue, const Component* inParamContext) const
        {
            String signature = inValue.trim();

            if (isReference(signature))
            {
                signature = signature.getBetween(REFERENCE_VALUE_OPENING, REFERENCE_VALUE_CLOSING).trim();
            }

            if (!isMethod(signature))
            {
                return {};
            }

            const String name = signature.substr(0, signature.firstOf(METHOD_PARAMS_OPENING));

            std::vector<String> args;
            if (parseLuaStringArgs(signature, args) && callScript(name, args))
            {
                return {};
            }

            if (const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*this)))
            {
                if (const ReflectionTypeMethodInfo* method = type->findMethod(name))
                {
                    ReflectionTypeMethod result(method);
                    result.bind(const_cast<Component*>(this));

                    if (inParamContext)
                    {
                        inParamContext->populateMethodParams(result, signature);
                    }

                    return result;
                }
            }

            if (!hasParent())
            {
                return {};
            }

            return m_parent->getMethod(inValue, inParamContext);
        }

        void Component::populateMethodParams(ReflectionTypeMethod& outMethod, const String& inSignature) const
        {
            const std::size_t open  = inSignature.firstOf(METHOD_PARAMS_OPENING);
            const std::size_t close = inSignature.lastOf(METHOD_PARAMS_CLOSING);

            if (open == String::npos || close == String::npos || close <= open)
            {
                return;
            }

            const String paramsRaw = inSignature.substr(open + 1, close - open - 1).trim();
            if (paramsRaw.isEmpty())
            {
                return;
            }

            const ReflectionTypeMethodInfo* info       = outMethod.getInfo();
            std::size_t                     paramIndex = 0;

            for (const String& rawParam : splitMethodParams(paramsRaw))
            {
                const String param = rawParam.trim();
                if (param.isEmpty())
                {
                    continue;
                }

                const String expected =
                    info && paramIndex < info->paramTypes.size() ? info->paramTypes.at(paramIndex) : String::sEmpty();
                paramIndex++;

                if ((param.startsWith("\"") && param.endsWith("\"")) || (param.startsWith("'") && param.endsWith("'")))
                {
                    outMethod.addParam(param.substr(1, param.size() - 2));

                    continue;
                }

                ReflectionFieldAccessor accessor = getField(param);
                const Component*        owner    = this;
                if (!accessor.isValid())
                {
                    for (const Component* node = hasParent() ? getParent() : nullptr; node != nullptr;
                         node                  = node->hasParent() ? node->getParent() : nullptr)
                    {
                        accessor = node->getField(param);
                        if (accessor.isValid())
                        {
                            owner = node;

                            break;
                        }

                        if (node->isRoot())
                        {
                            break;
                        }
                    }
                }

                if (accessor.isValid())
                {
                    const void* instance =
                        accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(owner);

                    if (expected.endsWith('*'))
                    {
                        const void* fieldAddr = accessor.address(instance);
                        void*       pointer   = nullptr;

                        if (fieldAddr)
                        {
                            const bool bSizeVoid = static_cast<bool>(accessor.size == sizeof(void*));

                            if (bSizeVoid)
                            {
                                pointer = *reinterpret_cast<void* const*>(fieldAddr);
                            }

                            if (!bSizeVoid)
                            {
                                pointer = const_cast<void*>(fieldAddr);
                            }
                        }

                        outMethod.addParam(pointer);

                        continue;
                    }

                    outMethod.addParam(accessor.toString(instance));

                    continue;
                }

                outMethod.addParam(param);
            }
        }

        std::vector<String> Component::splitMethodParams(const String& inValue) const
        {
            std::vector<String> result;

            std::size_t   start            = 0;
            std::uint32_t parenthesisCount = 0;

            for (std::size_t i = 0; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);

                if (character == METHOD_PARAMS_OPENING)
                {
                    parenthesisCount++;

                    continue;
                }

                if (character == METHOD_PARAMS_CLOSING)
                {
                    if (parenthesisCount > 0)
                    {
                        parenthesisCount--;
                    }

                    continue;
                }

                if (character != METHOD_PARAMS_SEPARATOR || parenthesisCount > 0)
                {
                    continue;
                }

                result.push_back(inValue.substr(start, i - start));
                start = i + 1;
            }

            if (start <= inValue.size())
            {
                result.push_back(inValue.substr(start));
            }

            return result;
        }
    }
}
