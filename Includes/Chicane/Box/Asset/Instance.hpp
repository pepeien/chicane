#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Header.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            class CHICANE_BOX Instance
            {
            protected:
                using Super = Instance;

            public:
                static inline constexpr const char* TAG = "Asset";

            public:
                Instance(const FileSystem::Path& inFilepath);
                virtual ~Instance() = default;

            public:
                bool isType(Type inType) const;
                bool isEmpty() const;

                const Header& getHeader() const;
                void setHeader(const Header& inHeader);

                const FileSystem::Path& getFilepath() const;
                void setFilepath(const FileSystem::Path& inFilepath);

                std::uint32_t getVersion() const;
                void setVersion(std::uint32_t inVersion);

                const String& getId() const;
                void setId(const String& inId);

                Type getType() const;
                void setType(Type inType);

                pugi::xml_node getXML() const;
                void saveXML();

            private:
                void createXML(const FileSystem::Path& inFilepath);
                void fetchXML(const FileSystem::Path& inFilepath);

            protected:
                Header             m_header;
                pugi::xml_document m_xml;
            };
        }
    }
}