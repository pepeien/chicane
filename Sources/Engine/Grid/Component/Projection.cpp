#include "Chicane/Grid/Component/Projection.hpp"

namespace Chicane
{
    namespace Grid
    {
        static thread_local std::vector<Component*>* g_projected = nullptr;

        Projection::Projection(std::vector<Component*>& inChildren)
            : previous(g_projected)
        {
            g_projected = &inChildren;
        }

        Projection::~Projection()
        {
            g_projected = previous;
        }

        std::vector<Component*>* Projection::sCurrent()
        {
            return g_projected;
        }
    }
}
