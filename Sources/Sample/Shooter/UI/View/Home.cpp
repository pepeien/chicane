#include "Sample/Shooter/UI/View/Home.reflected.hpp"

#include "Sample/Shooter/UI/Component/Crosshair.hpp"
#include "Sample/Shooter/UI/Component/Telemetry.hpp"

HomeView::HomeView()
    : Chicane::Grid::View(),
      crosshair()
{
    import <Crosshair>();
    import <Telemetry>();
    load("Assets/Sample/Shooter/UI/Views/Home/Index.grid", "Assets/Sample/Shooter/UI/Views/Home/Index.decal");
}
