#include "main.hpp"
#include "QonsistentSaberColors.hpp"

#include "GlobalNamespace/ColorsOverrideSettingsPanelController.hpp"

MAKE_HOOK_MATCH(
    ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx, 
    &GlobalNamespace::ColorsOverrideSettingsPanelController::HandleDropDownDidSelectCellWithIdx,
    void,
    GlobalNamespace::ColorsOverrideSettingsPanelController* self,
    HMUI::DropdownWithTableView* dropDownWithTableView, 
    int idx
) {
    ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx(self, dropDownWithTableView, idx);

    modManager.UpdateColors();
}

void QonsistentSaberColors::_Hook_ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx(){
    
    INSTALL_HOOK(getLogger(), ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx);
}