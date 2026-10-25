#include "features/skinchanger.h"
#include "sdk/interfaces.h"
#include "sdk/netvars.h"
#include "sdk/offsets.h"

namespace features {

skinchanger& skinchanger::instance() {
    static skinchanger sc;
    return sc;
}

void skinchanger::on_frame_stage_notify(int stage) {
    // Stage 5 = FRAME_RENDER_START
    if (stage != 5)
        return;

    apply_skins();
}

void skinchanger::apply_skins() {
    // TODO: get local player, iterate weapons
    // For each weapon with a configured skin:
    // - Set m_nFallbackPaintKit
    // - Set m_nFallbackSeed
    // - Set m_flFallbackWear
    // - Set m_nFallbackStatTrak
    // - Set m_iItemIDHigh to -1 (force fallback)
    // - Set m_iEntityQuality for knife/gloves
}

void skinchanger::apply_knife_model(int definition_index) {
    // Set m_iItemDefinitionIndex, m_nModelIndex for knife
    // Kill icons use m_iViewModelIndex and m_iWorldModelIndex
    (void)definition_index;
}

void skinchanger::apply_glove_model(int definition_index) {
    // Gloves require creating a wearable entity
    // Set m_iItemDefinitionIndex on the glove entity
    (void)definition_index;
}

void skinchanger::force_update_skin(void* weapon, const skin_info& info) {
    if (!weapon) return;
    (void)info;
    // Direct memory writes to weapon entity netvars
    // m_nFallbackPaintKit, m_nFallbackSeed, m_flFallbackWear, etc.
}

}
