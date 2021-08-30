#include "main.hpp"
#include "QonsistentSaberColors.hpp"

#include "GlobalNamespace/ColorsOverrideSettingsPanelController.hpp"

MAKE_HOOK_MATCH(
    ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged, 
    &GlobalNamespace::ColorsOverrideSettingsPanelController::HandleOverrideColorsToggleValueChanged,
    void,
    GlobalNamespace::ColorsOverrideSettingsPanelController* self,
    bool isOn
) {
    ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged(self, isOn);

    modManager._postfix_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged();
}

void QonsistentSaberColors::_Hook_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged(){
    
    INSTALL_HOOK(getLogger(), ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged);
}