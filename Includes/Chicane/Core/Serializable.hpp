#pragma once

#include <functional>
#include <unordered_map>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    class CHICANE_CORE Serializable
    {
    public:
        using AttributeMap          = XmlNode::Attributes;
        using AttributeCallback     = std::function<void(const String&)>;
        using AttributeObservable   = EventObservable<String>;
        using AttributeSubscription = EventSubscription<String>;

        using Watchers = std::unordered_map<String, AttributeObservable>;

    public:
        Serializable();
        virtual ~Serializable() = default;

        Serializable(const Serializable&)            = delete;
        Serializable& operator=(const Serializable&) = delete;
        Serializable(Serializable&&)                 = default;
        Serializable& operator=(Serializable&&)      = default;

    public:
        void load(const FileSystem::Path& inFilepath);
        void save();
        void save(const FileSystem::Path& inFilepath);

        void parse(const XmlNode& inNode);
        XmlNode createRoot(const String& inName);

        const FileSystem::Path& getFilepath() const;
        void setFilepath(const FileSystem::Path& inFilepath);

        const XmlDocument& getDocument() const;
        XmlDocument& getDocument();

        const XmlNode& getSource() const;
        XmlNode& getSource();

        const AttributeMap& getAttributes() const;
        bool hasAttribute(const String& inName) const;
        const String& getAttribute(const String& inName) const;
        void setAttribute(const String& inName, const String& inValue);
        void removeAttribute(const String& inName);

        float getFloat(const String& inName, float inFallback) const;
        bool getBool(const String& inName, bool inFallback) const;
        std::uint32_t getUint(const String& inName, std::uint32_t inFallback) const;
        String getString(const String& inName, const String& inFallback) const;
        Vec3 getVec3(const String& inName, const Vec3& inFallback) const;
        Vec4 getColor(const String& inName, const Vec4& inFallback) const;

        AttributeSubscription watch(const String& inName, const AttributeCallback& inCallback);

        virtual void syncProperties();
        virtual void refresh() { return; }

    protected:
        void bindSource(const XmlNode& inNode);
        void syncAttributes();
        void emitAttribute(const String& inName, const String& inValue);
        void writeAttributesToSource();

    protected:
        FileSystem::Path m_filepath;
        XmlDocument      m_document;
        XmlNode          m_source;
        AttributeMap     m_attributes;
        Watchers         m_watchers;
    };
}
