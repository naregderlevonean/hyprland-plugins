#include "BarPassElement.hpp"
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/desktop/view/window/Window.hpp>
#include <hyprland/src/desktop/view/window/WindowPresentation.hpp>
#include "barDeco.hpp"

using namespace Render::GL;

CBarPassElement::CBarPassElement(const CBarPassElement::SBarData& data_) : data(data_) {
    ;
}

std::vector<UP<IPassElement>> CBarPassElement::draw(Render::CRenderContext& ctx) {
    data.deco->renderPass(ctx, ctx.m_data.pMonitor.lock(), data.a);
    return {};
}

bool CBarPassElement::needsLiveBlur(Render::CRenderContext&) {
    static auto PENABLEBLURGLOBAL = CConfigValue<Config::BOOL>("decoration:blur:enabled");

    CHyprColor  color = data.deco->m_bForcedBarColor.value_or(CHyprColor{static_cast<uint64_t>(g_pGlobalState->config.barColor->value())});
    color.a *= data.a;
    const bool SHOULDBLUR = g_pGlobalState->config.barBlur->value() && *PENABLEBLURGLOBAL && color.a < 1.F;

    return SHOULDBLUR;
}

std::optional<CBox> CBarPassElement::boundingBox(Render::CRenderContext& ctx) {
    const auto WINDOW = data.deco->m_pWindow.lock();
    if (!WINDOW)
        return std::nullopt;

    const auto PRECEDENCE = g_pGlobalState->config.barPrecedenceOverBorder->value();
    const auto ROUNDING   = WINDOW->presentation().rounding() + (PRECEDENCE ? 0 : WINDOW->presentation().borderSize());

    CBox box = data.deco->assignedBoxGlobal();

    // The bar is drawn a bit below its assigned box to fill the gap between its
    // own and the window's rounded corners; include that strip in the bbox,
    // otherwise damage/occlusion do not cover the actually drawn area.
    box.h += ROUNDING * 3;

    // Temporary fix: expand the bar bb a bit, otherwise occlusion gets too aggressive.
    return box.translate(-ctx.m_data.pMonitor->m_position).expand(10);
}

bool CBarPassElement::needsPrecomputeBlur(Render::CRenderContext&) {
    return false;
}
