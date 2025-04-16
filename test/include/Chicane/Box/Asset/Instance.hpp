#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            class CHICANE Instance
            {
            public:
                static std::string TAG;

            public:
                Instance(const std::string& inFilepath);

            public:
                bool isType(Type inType) const;

                const Header& getHeader() const;
                void setHeader(const Header& inHeader);

                const std::filesystem::path& getFilepath() const;
                void setFilepath(const std::filesystem::path& inFilepath);

                std::uint32_t getVersion() const;
                void setVersion(std::uint32_t inVersion);

                const std::string& getId() const;
                void setId(const std::string& inId);

                Type getType() const;
                void setType(Type inType);

                void saveXML();

            protected:
                bool isXMLEmpty();
                pugi::xml_node getXMLRoot();

            private:
                void createXML(const std::string& inFilepath);
                void fetchXML(const std::string& inFilepath);
                void fetchHeader(const std::string& inFilepath);

            protected:
                Header             m_header;
                pugi::xml_document m_xml;
            };
        }
    }
}