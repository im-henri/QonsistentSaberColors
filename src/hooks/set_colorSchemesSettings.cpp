#include "QonsistentSaberColors.hpp"
#include "main.hpp"

#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"



MAKE_HOOK_MATCH(
    ColorSchemesSettings_SetColorSchemeForId, 
    &GlobalNamespace::ColorSchemesSettings::SetColorSchemeForId, 
    void,
    GlobalNamespace::ColorSchemesSettings* self,
    GlobalNamespace::ColorScheme* value
) {
    ColorSchemesSettings_SetColorSchemeForId(self, value);

    if(modManager.initialColorsSet){
        modManager.UpdateColors();
    }
}

void QonsistentSaberColors::_Hook_ColorSchemesSettings_SetColorSchemeForId(){
    INSTALL_HOOK(getLogger(), ColorSchemesSettings_SetColorSchemeForId);
}
