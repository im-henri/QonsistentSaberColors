#include "QonsistentSaberColors.hpp"

#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Component.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Color.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ColormanagerInstaller.hpp"
#include "GlobalNamespace/ColorSchemeSO.hpp"
#include "GlobalNamespace/PlayerData.hpp"

#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/colorSchemesSettings.hpp"
#include "codegen/include/System/Collections/Generic/Dictionary_2.hpp"


#include <sstream>
#include <string>

QonsistentSaberColors modManager;

void QonsistentSaberColors::InstallHooks() {
    _Hook_SceneManager_SetActiveScene();
    _Hook_ColorSchemesSettings_SetColorSchemeForId();
    _Hook_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged();
    _Hook_ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx();
}

void QonsistentSaberColors::UpdateColors(){
    auto PlayerDataModel_go = UnityEngine::GameObject::Find(il2cpp_utils::createcsstr("PlayerDataModel(Clone)"));
    auto PlayerDataModel = PlayerDataModel_go->GetComponent(il2cpp_utils::createcsstr("PlayerDataModel"));
    auto PlayerDataModel_il2cpp = il2cpp_utils::ToIl2CppObject(PlayerDataModel);

    auto playerData = (GlobalNamespace::PlayerData*) CRASH_UNLESS(il2cpp_utils::GetPropertyValue(PlayerDataModel_il2cpp, "playerData"));
    
    UnityEngine::Color colA;
    UnityEngine::Color colB;

    // Get saber target colors
    if( ! playerData->colorSchemesSettings->overrideDefaultColors){
        colA = defaultLeftColor;
        colB = defaultRightColor;
    } else{
        auto key = playerData->colorSchemesSettings->selectedColorSchemeId;
        auto val = playerData->colorSchemesSettings->colorSchemesDict->get_Item(key);
        colA = val->get_saberAColor();
        colB = val->get_saberBColor();
    }
    
    // Update Left Colors
    auto Normal =UnityEngine::GameObject::Find(il2cpp_utils::createcsstr("ControllerLeft/MenuHandle/Normal"));
    auto SetSaberGlowColor_component = Normal->GetComponent(il2cpp_utils::createcsstr("SetSaberGlowColor"));
    auto SetSaberGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberGlowColor_component);
    auto _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberGlowColor, "_colorManager"));
    auto colorScheme   = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    colorScheme->saberAColor = colA;
    il2cpp_utils::RunMethod(SetSaberGlowColor,  "SetColors");

    // Update Right Colors
    Normal = UnityEngine::GameObject::Find(il2cpp_utils::createcsstr("ControllerRight/MenuHandle/Normal"));
    SetSaberGlowColor_component = Normal->GetComponent(il2cpp_utils::createcsstr("SetSaberGlowColor"));
    SetSaberGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberGlowColor_component);
    _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberGlowColor, "_colorManager"));
    colorScheme   = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    colorScheme->saberBColor = colB;
    il2cpp_utils::RunMethod(SetSaberGlowColor,  "SetColors");

}



namespace ModUtils{
    void LogAllFields(Il2CppObject* il2CppObj, std::string prependText = ""){
        auto x = getLogger().WithContext(prependText);
        il2cpp_utils::LogFields(x, il2cpp_utils::ExtractClass(il2CppObj));
    }
    void LogAllProperties(Il2CppObject* il2CppObj, std::string prependText = ""){
        auto x = getLogger().WithContext(prependText);
        il2cpp_utils::LogProperties(x, il2cpp_utils::ExtractClass(il2CppObj));
    }
    void LogAllMethods(Il2CppObject* il2CppObj, std::string prependText = ""){
        auto x = getLogger().WithContext(prependText);
        il2cpp_utils::LogMethods(x, il2cpp_utils::ExtractClass(il2CppObj));
    }

    template <typename T>
    void Log_GetComponents(UnityEngine::GameObject* gameobj){
        auto componentArr = gameobj->GetComponents<T>();
        std::stringstream buff;
        for (int i=0; i<componentArr->Length(); i++){
            auto comp = (*componentArr)[i];
            auto name = comp->get_name();
            auto fullName = comp->GetType()->get_FullName();
            buff << to_utf8(csstrtostr(fullName)).c_str() << " " << to_utf8(csstrtostr(name)).c_str() << "\n";
        }
        getLogger().info("%s",buff.str().c_str());
    }
    template <typename T>
    void Log_GetComponentsInChildren(UnityEngine::GameObject* gameobj){
        auto componentArr = gameobj->GetComponentsInChildren<T>();
        std::stringstream buff;
        for (int i=0; i<componentArr->Length(); i++){
            auto comp = (*componentArr)[i];
            auto name = comp->get_name();
            auto fullName = comp->GetType()->get_FullName();
            buff  << to_utf8(csstrtostr(fullName)).c_str() << " " << to_utf8(csstrtostr(name)).c_str() << "\n";
        }
        getLogger().info("%s",buff.str().c_str());
    }
}