#include "Chicane/Core/Xml/Document.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>

#include <pugixml.hpp>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    struct XmlDocument::Impl
    {
        pugi::xml_document document;

        struct Writer : pugi::xml_writer
        {
            std::string data;

            void write(const void* inData, size_t inSize) override
            {
                data.append(static_cast<const char*>(inData), inSize);
            }
        };
    };

    static pugi::xml_node asPugi(void* inNode)
    {
        return pugi::xml_node(static_cast<pugi::xml_node_struct*>(inNode));
    }

    static void* asHandle(const pugi::xml_node& inNode)
    {
        return inNode.internal_object();
    }

    XmlDocument::XmlDocument()
        : m_impl(std::make_unique<Impl>())
    {}

    XmlDocument::~XmlDocument() = default;

    XmlDocument::XmlDocument(XmlDocument&&) noexcept            = default;
    XmlDocument& XmlDocument::operator=(XmlDocument&&) noexcept = default;

    bool XmlDocument::empty() const
    {
        return !m_impl || m_impl->document.children().empty();
    }

    bool XmlDocument::isEmpty() const
    {
        return empty();
    }

    XmlDocument::operator bool() const
    {
        return !empty();
    }

    void XmlDocument::reset()
    {
        if (!m_impl)
        {
            m_impl = std::make_unique<Impl>();

            return;
        }

        m_impl->document.reset();
    }

    XmlNode XmlDocument::getFirstChild() const
    {
        if (!m_impl)
        {
            return XmlNode();
        }

        return XmlNode(asHandle(m_impl->document.first_child()));
    }

    XmlNode XmlDocument::appendChild(const String& inName)
    {
        if (!m_impl)
        {
            m_impl = std::make_unique<Impl>();
        }

        return XmlNode(asHandle(m_impl->document.append_child(inName.toChar())));
    }

    XmlNode XmlDocument::appendCopy(const XmlNode& inNode)
    {
        if (!m_impl)
        {
            m_impl = std::make_unique<Impl>();
        }

        return XmlNode(asHandle(m_impl->document.append_copy(asPugi(inNode.handle()))));
    }

    XmlDocument XmlDocument::sLoad(const FileSystem::Path& inFilepath)
    {
        if (inFilepath.isEmpty())
        {
            throw std::runtime_error("The XML document path is empty");
        }

        const FileSystem::Path path = FileSystem::resolve(inFilepath);

        XmlDocument result;
        if (!result.m_impl->document.load_file(path.toChar(), pugi::parse_default | pugi::parse_fragment))
        {
            throw std::runtime_error("Failed to read the XML document [" + path.toString() + "]");
        }

        return result;
    }

    bool XmlDocument::loadBuffer(const void* inData, std::size_t inSize)
    {
        if (!m_impl)
        {
            m_impl = std::make_unique<Impl>();
        }

        if (!inData || inSize == 0)
        {
            return false;
        }

        return m_impl->document.load_buffer(inData, inSize, pugi::parse_default | pugi::parse_fragment);
    }

    void XmlDocument::save(const FileSystem::Path& inFilepath) const
    {
        if (!m_impl)
        {
            throw std::runtime_error("The XML document is empty");
        }

        if (inFilepath.isEmpty())
        {
            throw std::runtime_error("The XML document path is empty");
        }

        const FileSystem::Path path = FileSystem::exists(inFilepath)
                                          ? FileSystem::resolve(inFilepath)
                                          : FileSystem::Path(std::filesystem::absolute(inFilepath));

        XmlDocument::Impl::Writer writer;
        m_impl->document.save(
            writer,
            "    ",
            pugi::format_default | pugi::format_no_empty_element_tags | pugi::format_no_declaration
        );

        const std::size_t close = writer.data.rfind("</");
        if (close != std::string::npos && close > 0 && writer.data[close - 1] != '\n')
        {
            writer.data.insert(close, 1, '\n');
        }

        FileSystem::write(writer.data, path);
    }
}
