#include "Chicane/Box/Skeleton.hpp"

#include <stdexcept>
#include <unordered_set>

namespace Chicane
{
    namespace Box
    {
        static const SkeletonBone* findBone(const SkeletonBone::List& inBones, const String& inId)
        {
            for (const SkeletonBone& bone : inBones)
            {
                if (bone.id.equals(inId))
                {
                    return &bone;
                }

                if (const SkeletonBone* child = findBone(bone.children, inId))
                {
                    return child;
                }
            }

            return nullptr;
        }

        static void flattenBone(
            const SkeletonBone&         inBone,
            std::int32_t                inParentIndex,
            SkeletonBoneEntry::List&    outEntries,
            std::unordered_set<String>& outIds
        )
        {
            if (inBone.id.isEmpty())
            {
                throw std::runtime_error("Skeleton bone is missing an id");
            }

            if (!outIds.insert(inBone.id).second)
            {
                throw std::runtime_error("Skeleton bone [" + inBone.id.toStandard() + "] is duplicated");
            }

            const std::int32_t index = static_cast<std::int32_t>(outEntries.size());

            SkeletonBoneEntry entry;
            entry.id          = inBone.id;
            entry.parentIndex = inParentIndex;
            entry.transform.setTransform(inBone.transform);
            outEntries.push_back(entry);

            for (const SkeletonBone& child : inBone.children)
            {
                flattenBone(child, index, outEntries, outIds);
            }
        }

        Skeleton::Skeleton(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_bones({}),
              m_entries({})
        {
            fetchBones();
        }

        const SkeletonBone::List& Skeleton::getBones() const
        {
            return m_bones;
        }

        void Skeleton::setBones(const SkeletonBone::List& inBones)
        {
            m_bones = inBones;
            rebuildEntries();
            writeBones();
        }

        const SkeletonBoneEntry::List& Skeleton::getEntries() const
        {
            return m_entries;
        }

        const SkeletonBone* Skeleton::getBone(const String& inId) const
        {
            return findBone(m_bones, inId);
        }

        std::int32_t Skeleton::findIndex(const String& inId) const
        {
            for (std::int32_t i = 0; i < static_cast<std::int32_t>(m_entries.size()); i++)
            {
                if (m_entries.at(i).id.equals(inId))
                {
                    return i;
                }
            }

            return -1;
        }

        bool Skeleton::hasBone(const String& inId) const
        {
            return findIndex(inId) >= 0;
        }

        void Skeleton::fetchBones()
        {
            m_bones.clear();

            if (getFilepath().isEmpty() || isXMLEmpty())
            {
                return;
            }

            for (const pugi::xml_node& child : getXML().children())
            {
                if (!String(child.name()).equals(SkeletonBone::TAG))
                {
                    continue;
                }

                SkeletonBone bone;
                bone.setFrom(child);
                m_bones.push_back(bone);
            }

            rebuildEntries();
        }

        void Skeleton::rebuildEntries()
        {
            m_entries.clear();

            std::unordered_set<String> ids;
            for (const SkeletonBone& bone : m_bones)
            {
                flattenBone(bone, -1, m_entries, ids);
            }

            std::vector<Mat4> worlds(m_entries.size(), Mat4::One);
            for (std::size_t i = 0; i < m_entries.size(); i++)
            {
                const Mat4 local = m_entries[i].transform.getMatrix();
                if (m_entries[i].parentIndex >= 0)
                {
                    worlds[i] = worlds[static_cast<std::size_t>(m_entries[i].parentIndex)] * local;
                }
                else
                {
                    worlds[i] = local;
                }

                m_entries[i].inverseBind = worlds[i].inverse();
            }
        }

        void Skeleton::writeBones()
        {
            pugi::xml_node root = getXML();
            for (pugi::xml_node child = root.first_child(); child;)
            {
                pugi::xml_node next = child.next_sibling();
                if (String(child.name()).equals(SkeletonBone::TAG))
                {
                    root.remove_child(child);
                }

                child = next;
            }

            for (const SkeletonBone& bone : m_bones)
            {
                pugi::xml_node boneNode = root.append_child(SkeletonBone::TAG);
                bone.saveTo(boneNode);
            }
        }
    }
}
