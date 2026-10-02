#include "Chicane/Grid/Component/RouterView.reflected.hpp"

#include "Chicane/Grid/Component/Scope.hpp"
#include "Chicane/Grid/Component/View.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        RouterView::RouterView(const XmlNode& inNode)
            : Container(inNode),
              m_mounted(String::sEmpty())
        {}

        void RouterView::tick(float inDeltaTime)
        {
            syncRoute();
            applyVisibility();

            Container::tick(inDeltaTime);

            applyVisibility();
        }

        void RouterView::refreshSize()
        {
            style.display.setRaw(Style::DISPLAY_TYPE_FLEX);
            style.display.refresh();
            style.flex.direction.setRaw(Style::FLEX_DIRECTION_TYPE_COLUMN);
            style.flex.direction.refresh();
            style.flex.wrap.setRaw(Style::FLEX_WRAP_TYPE_NOWRAP);
            style.flex.wrap.refresh();
            style.width.value.setRaw("100%");
            style.height.value.setRaw("100%");
            style.width.refresh();
            style.height.refresh();

            Container::refreshSize();
        }

        void RouterView::syncRoute()
        {
            View* view = dynamic_cast<View*>(getRoot());
            if (!view)
            {
                return;
            }

            const String& route = view->getRoute();
            if (route.equals(m_mounted))
            {
                return;
            }

            const Route* found = view->findRoute(route);
            if (!found)
            {
                m_mounted = route;

                return;
            }

            if (!findPage(found->path))
            {
                mount(*found);
            }

            m_mounted = found->path;
        }

        void RouterView::applyVisibility()
        {
            bool bChanged = false;

            for (const MountedPage& page : m_pages)
            {
                if (!page.page)
                {
                    continue;
                }

                const bool bVisible = page.path.equals(m_mounted);
                const bool bHidden  = page.page->style.display.isRaw(Style::DISPLAY_TYPE_NONE);

                if (bVisible)
                {
                    if (!bHidden)
                    {
                        continue;
                    }

                    page.page->markStyleDirty();
                    page.page->markLayoutDirty();
                    bChanged = true;

                    continue;
                }

                page.page->style.display.setRaw(Style::DISPLAY_TYPE_NONE);
                page.page->style.display.refresh();
                page.page->setFlag(ComponentDirty::Style, false);

                if (bHidden)
                {
                    continue;
                }

                page.page->markLayoutDirty();
                bChanged = true;
            }

            if (bChanged)
            {
                markLayoutDirty();
            }
        }

        Component* RouterView::findPage(const String& inPath) const
        {
            for (const MountedPage& page : m_pages)
            {
                if (page.path.equals(inPath))
                {
                    return page.page;
                }
            }

            return nullptr;
        }

        void RouterView::mount(const Route& inRoute)
        {
            if (inRoute.file.isEmpty())
            {
                return;
            }

            View*      view = dynamic_cast<View*>(getRoot());
            Scope      scope(view ? static_cast<Component*>(view) : static_cast<Component*>(this));
            Component* created = sCreate(inRoute.file);
            if (!created)
            {
                return;
            }

            addChild(created);

            MountedPage mounted;
            mounted.path = inRoute.path;
            mounted.page = created;
            m_pages.push_back(mounted);
        }
    }
}
