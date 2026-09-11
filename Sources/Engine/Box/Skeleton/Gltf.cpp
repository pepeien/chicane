#include "Chicane/Box/Skeleton/Gltf.hpp"

#include "../Gltf/Parser.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace SkeletonGltf
        {
            bool hasSkin(const FileSystem::Path& inFilepath)
            {
                const Document   document(inFilepath);
                const tg3_model& model = document.get();

                for (std::uint32_t i = 0; i < model.skins_count; ++i)
                {
                    if (model.skins[i].joints && model.skins[i].joints_count > 0)
                    {
                        return true;
                    }
                }

                return false;
            }

            SkeletonBone::List parse(const FileSystem::Path& inFilepath)
            {
                const Document                   document(inFilepath);
                const std::vector<std::int32_t>  joints  = collectJoints(document);
                const std::vector<std::int32_t>  parents = document.parents();
                std::unordered_set<std::int32_t> jointSet(joints.begin(), joints.end());

                std::unordered_map<std::int32_t, std::vector<std::int32_t>> children;
                std::vector<std::int32_t>                                   roots;
                std::unordered_set<std::int32_t>                            seen;

                for (const std::int32_t joint : joints)
                {
                    if (!seen.insert(joint).second)
                    {
                        continue;
                    }

                    const std::int32_t parent = findJointParent(joint, parents, jointSet);
                    if (parent < 0)
                    {
                        roots.push_back(joint);

                        continue;
                    }

                    children[parent].push_back(joint);
                }

                if (roots.empty())
                {
                    throw std::runtime_error(
                        "The glTF file [" + inFilepath.toString().toStandard() + "] has no skeleton bones"
                    );
                }

                SkeletonBone::List bones;
                for (const std::int32_t root : roots)
                {
                    bones.push_back(buildBone(document, root, children));
                }

                return bones;
            }
        }
    }
}
