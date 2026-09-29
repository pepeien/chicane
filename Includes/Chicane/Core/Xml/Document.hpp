#pragma once

#include <memory>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Xml/Node.hpp"

namespace Chicane
{
    struct XmlDocumentImpl;

    class CHICANE_CORE XmlDocument
    {
        friend class XmlNode;

    public:
        static XmlDocument sLoad(const FileSystem::Path& inFilepath);

    public:
        XmlDocument();
        ~XmlDocument();

        XmlDocument(const XmlDocument&)            = delete;
        XmlDocument& operator=(const XmlDocument&) = delete;
        XmlDocument(XmlDocument&&) noexcept;
        XmlDocument& operator=(XmlDocument&&) noexcept;

    public:
        bool empty() const;
        bool isEmpty() const;
        explicit operator bool() const;

        void reset();

        XmlNode getFirstChild() const;
        XmlNode appendChild(const String& inName);
        XmlNode appendCopy(const XmlNode& inNode);

        bool loadBuffer(const void* inData, std::size_t inSize);
        void save(const FileSystem::Path& inFilepath) const;

    private:
        std::unique_ptr<XmlDocumentImpl> m_impl;
    };
}
