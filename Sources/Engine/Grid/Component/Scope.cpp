#include "Chicane/Grid/Component/Scope.hpp"

namespace Chicane
{
    namespace Grid
    {
        static thread_local Component* g_scope = nullptr;

        Scope::Scope(Component* inComponent)
            : previous(g_scope)
        {
            g_scope = inComponent;
        }

        Scope::~Scope()
        {
            g_scope = previous;
        }

        Component* Scope::sCurrent()
        {
            return g_scope;
        }
    }
}
