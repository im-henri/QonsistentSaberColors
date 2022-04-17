#include "QonsistentSaberColors.hpp"

#include "Config.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Component.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/HideFlags.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"

#include "GlobalNamespace/OVRInput.hpp" // For debug purposes
#include "GlobalNamespace/OVRInput_Button.hpp" // For debug purposes

#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ColorManagerInstaller.hpp"
#include "GlobalNamespace/ColorSchemeSO.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"
#include "GlobalNamespace/SaberType.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"

#include "codegen/include/System/Collections/Generic/Dictionary_2.hpp"

#include <sstream>
#include <string>


// TODO: Waiting for qosmetics to be updated to the latest version
//#include "qosmetics-api/shared/QosmeticsAPI.hpp"
//#include "qosmetics-api/shared/SaberAPI.hpp"
//#include "qosmetics-api/shared/Components/ColorComponent.hpp"
//#include "qosmetics-api/shared/Components/AltTrail.hpp"
//#include "qosmetics-api/shared/Components/TrailHelper.hpp"
//#include "qosmetics-api/shared/Components/ColorComponent.hpp"

#include "ModDebugUtils.hpp"

// First and only instance is created here.
// Accessed in other files by including QonsistentSaberColors.hpp
QonsistentSaberColors modManager;

void QonsistentSaberColors::InstallHooks() {
    _Hook_SceneManager_SetActiveScene();
    _Hook_ColorSchemesSettings_SetColorSchemeForId();
    _Hook_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged();
    _Hook_ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx();
    _Hook_MainMenuViewController_DidActivate();
    _Hook_VRPointer_CreateLaserPointerAndLaserHit();

    //_Hook_OculusVRHelper_VRControllersInputManager(); // For debug purposes
}

// --- Hook Postfixes ---
void QonsistentSaberColors::_postfix_SetSceneActive(UnityEngine::SceneManagement::Scene& scene){
    getLogger().info("_postfix_SetSceneActive");
    // When scene changes, presumably objects get destroyed.. etc
    // --> pointers to coloringObjects are no longer valid.
    _SetColoringObjectsDirty();
    
    /* If original sabers are being used, they should be updated at scene change only once.
       Afterwards color changes are caused by either user changing color etc. */
    if ( !initialColorsSet ){
        auto sceneName = scene.get_name();
        bool is_MainMenu = (to_utf8(csstrtostr(  sceneName )).compare("MainMenu") == 0);
        bool is_ShaderWarmup = (to_utf8(csstrtostr(  sceneName )).compare("ShaderWarmup") == 0);
        bool is_HealthWarning = (to_utf8(csstrtostr(  sceneName )).compare("HealthWarning") == 0);
    
        if((is_MainMenu == true) || (is_ShaderWarmup == true) || (is_HealthWarning == true)){
            getLogger().info("Initial color update");
            UpdateColors();
            initialColorsSet = true;
        }
    }
    
}
void QonsistentSaberColors::_postfix_MainMenuViewController_DidActivate(){
    getLogger().info("_postfix_MainMenuViewController_DidActivate");
    // Due to issues with detecing qosmetics saber changes, when entering mainMenu, 
    // re-coloring the sabers. This way the sabers colors update at latest in mainMenu.
    UpdateColors();
}
void QonsistentSaberColors::_postfix_VRPointer_CreateLaserPointerAndLaserHit(){
    getLogger().info("_postfix_VRPointer_CreateLaserPointerAndLaserHit()");
    // Upon laser pointer hand change the color should be updated.
    _Update_LaserPointerColor();
}
void QonsistentSaberColors::_postfix_ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx(){
    getLogger().info("_postfix_ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx()");
    modManager.UpdateColors();
}
void QonsistentSaberColors::_postfix_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged(){
    getLogger().info("_postfix_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged()");
    modManager.UpdateColors();
}
void QonsistentSaberColors::_postfix_ColorSchemesSettings_SetColorSchemeForId(){
    getLogger().info("_postfix_ColorSchemesSettings_SetColorSchemeForId()");
    if(modManager.initialColorsSet){
        modManager.UpdateColors();
    }
}

#if QOSMETICS_API_EXISTS
// Yanked from RedBrumbler/Qosmetics "MaterialUtils.cpp"
// Returns boolean of can the matrial be colored.
namespace Qosmetics{
    bool _ShouldCC(UnityEngine::Material* mat) {
        static int CustomColorID = 0;
        static int GlowID = 0;
        static int BloomID = 0;

        if (!CustomColorID) CustomColorID = UnityEngine::Shader::PropertyToID(StringW("_CustomColors"));
        if (!GlowID) GlowID = UnityEngine::Shader::PropertyToID(StringW("_Glow"));
        if (!BloomID) BloomID = UnityEngine::Shader::PropertyToID(StringW("_Bloom"));

        // ew ugly else if ladder, but there is no other way of doing it
        if (mat->HasProperty(CustomColorID)) {
            return mat->GetFloat(CustomColorID) > 0;
        } else if (mat->HasProperty(GlowID)) {
            return mat->GetFloat(GlowID) > 0;
        } else if (mat->HasProperty(BloomID)) {
            return mat->GetFloat(BloomID) > 0;
        }
        return false;
    }
}

bool QonsistentSaberColors::_UpdateQosMenuPointerColor(UnityEngine::Color col, bool rightHand){
    const std::string itemHierarchy = rightHand ? "ControllerRight/RightPointer" : "ControllerLeft/LeftPointer";
    UnityEngine::GameObject* qosSaber = UnityEngine::GameObject::Find(StringW(itemHierarchy));
    if(qosSaber){
        int renderQueue = 0;
        auto object = qosSaber;
        auto color = col;
        auto ColorID = UnityEngine::Shader::PropertyToID(StringW("_Color"));

        // get all renderers on the object
        auto renderers = object->GetComponentsInChildren<UnityEngine::Renderer*>(true);
        if (!renderers) return false;
        for (int i = 0; i < renderers->Length(); i++) {
            UnityEngine::Renderer* currentRenderer = renderers->values[i];
            if (!currentRenderer) continue;
            auto materials = currentRenderer->GetMaterialArray();
            int materialLength = materials->Length();
            for (int j = 0; j < materialLength; j++) {
                UnityEngine::Material* currentMaterial = materials[j];
                // if renderqueue is given, set it
                //currentMaterial->set_renderQueue(renderQueue);
                if (!currentMaterial || !Qosmetics::_ShouldCC(currentMaterial)) continue;
                if (currentMaterial->HasProperty(ColorID)) currentMaterial->SetColor(ColorID, color);
            }
        }
        return true;
    }
    return false;
}

void QonsistentSaberColors::_UpdateQosMenuPointerTrails(){
    // As of right now there is a bug in qosmetics where changing sabers when menu pointers are on
    // will cause trails to dissapear. -> Have to make sure TrailHelper actually exists. 
    auto trails = UnityEngine::GameObject::Find(StringW("ControllerLeft/LeftPointer"));
    if(trails) {
        auto trailhelper = trails->GetComponentInChildren<Qosmetics::TrailHelper*>();
        if(trailhelper) trailhelper->SetColors(colA, colB);
    }
    auto trails2 = UnityEngine::GameObject::Find(StringW("ControllerRight/RightPointer"));
    if(trails2) {
        auto trailhelper = trails2->GetComponentInChildren<Qosmetics::TrailHelper*>();
        if (trailhelper) trailhelper->SetColors(colA, colB);
    }
}
#endif

/*static bool rButton_prev = false;
// Fixed update to get right hand button press. Helpful for debugging purposes.
void QonsistentSaberColors::fixedUpdate(){
    bool rButton = GlobalNamespace::OVRInput::Get
            (GlobalNamespace::OVRInput::Button::One, GlobalNamespace::OVRInput::Controller::RTouch) ? true : false;
    if((rButton == true) && (rButton_prev == false)){
        getLogger().info("R_Button Pressed");
        //ModDebugUtils::WriteToLog_AllGameObjectsInScene();
    }
    rButton_prev = rButton;
}*/


void QonsistentSaberColors::_Update_LaserPointerColor(){ 
    auto pointerPrefab = UnityEngine::GameObject::Find(StringW("VRLaserPointer(Clone)"));
    if(pointerPrefab){
        auto parentName = pointerPrefab->get_transform()->get_parent()->get_name();
        UnityEngine::Color pointerCol;
        if(getModConfig().laserPointerColors.GetValue())
            pointerCol = (to_utf8(csstrtostr(parentName)).compare("ControllerRight") == 0) ? this->colB : this->colA;
        else
            pointerCol = (to_utf8(csstrtostr(parentName)).compare("ControllerRight") == 0) ? this->defaultRightColor : this->defaultLeftColor;
        pointerCol.a = 0;
        auto mr = pointerPrefab->GetComponentInChildren<UnityEngine::MeshRenderer*>();
        auto mats = mr->GetMaterialArray();
        for(int j=0; j<mats->Length(); j++){
            auto mat = (mats)[j];
            mat->set_color(pointerCol);
        }
    }
}

void QonsistentSaberColors::_SetColoringObjectsDirty(){
    this->_coloringObjectsDirty = true;
}

void QonsistentSaberColors::_SetGlowColors(UnityEngine::Color colA, UnityEngine::Color colB){
    // Make sure objects exist
    if(_coloringObjectsDirty) 
        this->_update_coloring_objects();

    // --- Left  Hand ---
    l_FakeGlow0.second->saberAColor = colA; il2cpp_utils::RunMethod(l_FakeGlow0.first,  "SetColors");
    l_FakeGlow1.second->saberAColor = colA; il2cpp_utils::RunMethod(l_FakeGlow1.first,  "SetColors");
    l_Glowing.second->saberAColor   = colA; il2cpp_utils::RunMethod(l_Glowing.first,  "SetColors");
    // --- Right Hand --- 
    r_FakeGlow0.second->saberBColor = colB; il2cpp_utils::RunMethod(r_FakeGlow0.first,  "SetColors");
    r_FakeGlow1.second->saberBColor = colB; il2cpp_utils::RunMethod(r_FakeGlow1.first,  "SetColors");
    r_Glowing.second->saberBColor   = colB; il2cpp_utils::RunMethod(r_Glowing.first,  "SetColors");
}
void QonsistentSaberColors::HideOfficalSaberGlows(){
    // Instead of disabling the gameobject it is better to set color to invisible
    // --> disabling GameObject will make it invisible to GameObject::Find() Method.
    UnityEngine::Color col{1,1,1,0}; 
    this->_SetGlowColors(col, col);
}

void QonsistentSaberColors::_SetOriginalSaberColors(){
    // Make sure objects exist
    if(_coloringObjectsDirty) 
        this->_update_coloring_objects();

    // Saber color of Left/Right Hand
    l_Normal.second->saberAColor = colA; il2cpp_utils::RunMethod(l_Normal.first,  "SetColors");
    r_Normal.second->saberBColor = colB; il2cpp_utils::RunMethod(r_Normal.first,  "SetColors");
    // Glows of Left/Right Hand
    this->_SetGlowColors(colA, colB);
}

void QonsistentSaberColors::_update_coloring_objects(){
    // --- Left  Hand ---
    // ### Update Left Colors ####  
    // Fakeglow0
    auto fakeGlow =UnityEngine::GameObject::Find(StringW("ControllerLeft/MenuHandle/FakeGlow0"));
    auto SetSaberFakeGlowColor_component = fakeGlow->GetComponent(StringW("SetSaberFakeGlowColor"));
    auto SetSaberFakeGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberFakeGlowColor_component);
    auto _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberFakeGlowColor, "_colorManager"));
    auto colorScheme = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    l_FakeGlow0 = std::make_pair(SetSaberFakeGlowColor, colorScheme);
    //colorScheme->saberAColor = colA;
    //il2cpp_utils::RunMethod(SetSaberFakeGlowColor,  "SetColors");

    // Fakeglow1
    fakeGlow =UnityEngine::GameObject::Find(StringW("ControllerLeft/MenuHandle/FakeGlow1"));
    SetSaberFakeGlowColor_component = fakeGlow->GetComponent(StringW("SetSaberFakeGlowColor"));
    SetSaberFakeGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberFakeGlowColor_component);
    _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberFakeGlowColor, "_colorManager"));
    colorScheme = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    l_FakeGlow1 = std::make_pair(SetSaberFakeGlowColor, colorScheme);
    //colorScheme->saberAColor = colA;
    //il2cpp_utils::RunMethod(SetSaberFakeGlowColor,  "SetColors");
    
    // Glow color
    auto Normal = UnityEngine::GameObject::Find(StringW("ControllerLeft/MenuHandle/Glowing"));
    auto SetSaberGlowColor_component = Normal->GetComponent(StringW("SetSaberGlowColor"));
    auto SetSaberGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberGlowColor_component);
    _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberGlowColor, "_colorManager"));
    colorScheme   = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    l_Glowing = std::make_pair(SetSaberGlowColor, colorScheme);
    //colorScheme->saberAColor = colA;
    //il2cpp_utils::RunMethod(SetSaberGlowColor,  "SetColors");

    // Actual saber coloring
    Normal = UnityEngine::GameObject::Find(StringW("ControllerLeft/MenuHandle/Normal"));
    SetSaberGlowColor_component = Normal->GetComponent(StringW("SetSaberGlowColor"));
    SetSaberGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberGlowColor_component);
    _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberGlowColor, "_colorManager"));
    colorScheme   = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    l_Normal = std::make_pair(SetSaberGlowColor, colorScheme);
    //colorScheme->saberAColor = colA;
    //il2cpp_utils::RunMethod(SetSaberGlowColor,  "SetColors");


    // --- Right Hand ---
    // Fakeglow0
    fakeGlow =UnityEngine::GameObject::Find(StringW("ControllerRight/MenuHandle/FakeGlow0"));
    SetSaberFakeGlowColor_component = fakeGlow->GetComponent(StringW("SetSaberFakeGlowColor"));
    SetSaberFakeGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberFakeGlowColor_component);
    _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberFakeGlowColor, "_colorManager"));
    colorScheme = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    r_FakeGlow0 = std::make_pair(SetSaberFakeGlowColor, colorScheme);
    //colorScheme->saberBColor = colB;
    //il2cpp_utils::RunMethod(SetSaberFakeGlowColor,  "SetColors");

    // Fakeglow1
    fakeGlow =UnityEngine::GameObject::Find(StringW("ControllerRight/MenuHandle/FakeGlow1"));
    SetSaberFakeGlowColor_component = fakeGlow->GetComponent(StringW("SetSaberFakeGlowColor"));
    SetSaberFakeGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberFakeGlowColor_component);
    _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberFakeGlowColor, "_colorManager"));
    colorScheme = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    r_FakeGlow1 = std::make_pair(SetSaberFakeGlowColor, colorScheme);
    //colorScheme->saberBColor = colB;
    //il2cpp_utils::RunMethod(SetSaberFakeGlowColor,  "SetColors");
    
    // Glow color
    Normal = UnityEngine::GameObject::Find(StringW("ControllerRight/MenuHandle/Glowing"));
    SetSaberGlowColor_component = Normal->GetComponent(StringW("SetSaberGlowColor"));
    SetSaberGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberGlowColor_component);
    _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberGlowColor, "_colorManager"));
    colorScheme   = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    r_Glowing = std::make_pair(SetSaberGlowColor, colorScheme);
    //colorScheme->saberBColor = colB;
    //il2cpp_utils::RunMethod(SetSaberGlowColor,  "SetColors");

    // Actual saber coloring
    Normal = UnityEngine::GameObject::Find(StringW("ControllerRight/MenuHandle/Normal"));
    SetSaberGlowColor_component = Normal->GetComponent(StringW("SetSaberGlowColor"));
    SetSaberGlowColor = il2cpp_utils::ToIl2CppObject(SetSaberGlowColor_component);
    _colorManager = CRASH_UNLESS(il2cpp_utils::GetFieldValue(SetSaberGlowColor, "_colorManager"));
    colorScheme   = (GlobalNamespace::ColorScheme*) CRASH_UNLESS(il2cpp_utils::GetFieldValue(_colorManager, "_colorScheme"));
    r_Normal = std::make_pair(SetSaberGlowColor, colorScheme);
    //colorScheme->saberBColor = colB;
    //il2cpp_utils::RunMethod(SetSaberGlowColor,  "SetColors");

    _coloringObjectsDirty = false;
}


#include "GlobalNamespace/OVRSkeletonRenderer.hpp"

/*void QonsistentSaberColors::_Update_FingerSaberColors(){
    auto rHandTracking_go = UnityEngine::GameObject::Find(StringW("HandTracking_container/rightHandAnchor/rightHandTracking"));
    if(!rHandTracking_go)
        return;
    auto lHandTracking_go = UnityEngine::GameObject::Find(StringW("HandTracking_container/leftHandAnchor/leftHandTracking"));
    if(!lHandTracking_go)
        return;
    
    auto rightOVRSkeletonRenderer = rHandTracking_go->GetComponent<GlobalNamespace::OVRSkeletonRenderer*>();
    rightOVRSkeletonRenderer->skeletonMaterial->SetColor(StringW("_Color"), colB);

    auto leftOVRSkeletonRenderer  = lHandTracking_go->GetComponent<GlobalNamespace::OVRSkeletonRenderer*>();
    leftOVRSkeletonRenderer->skeletonMaterial->SetColor(StringW("_Color"), colA);
}*/

void QonsistentSaberColors::UpdateColors(){
    auto PlayerDataModel_go = UnityEngine::GameObject::Find(StringW("PlayerDataModel(Clone)"));
    auto PlayerDataModel = PlayerDataModel_go->GetComponent(StringW("PlayerDataModel"));
    auto PlayerDataModel_il2cpp = il2cpp_utils::ToIl2CppObject(PlayerDataModel);

    auto playerData = (GlobalNamespace::PlayerData*) CRASH_UNLESS(il2cpp_utils::GetPropertyValue(PlayerDataModel_il2cpp, "playerData"));
    
    // Update saber target colors
    if( ! playerData->colorSchemesSettings->overrideDefaultColors){
        colA = defaultLeftColor;
        colB = defaultRightColor;
    } else{
        auto key = playerData->colorSchemesSettings->selectedColorSchemeId;
        auto val = playerData->colorSchemesSettings->colorSchemesDict->get_Item(key);
        colA = val->get_saberAColor();
        colB = val->get_saberBColor();
    }

    // LaserPointer color
    _Update_LaserPointerColor();
    
    // If FingerSaber exists, change hand colors
    //_Update_FingerSaberColors();

    // Update QosMenuPointer Left/Right
    // Note Order in this if statement is very imporant.
    // -- cpp "&&" guarantees left-to-right evaluation, meaning the getconfig().value() does not cause error in case qos does not exists. 
    // Lastly if UpdateQosMenuPointerColor(..) fails it means menu sabers do not exists and original sabers need to be colored instead. --

#if QOSMETICS_API_EXISTS

    if( (Qosmetics::API::GetExists() == true) &&
            (Qosmetics::API::GetConfig().value().get().saberConfig.enableMenuPointer == true) &&
                (_UpdateQosMenuPointerColor(colB, true) == true)
    ) {
        getLogger().info("Qosmetics menu saber color update");
        
        _UpdateQosMenuPointerColor(colA, false);
        // Trails Left/Right
        _UpdateQosMenuPointerTrails();
        // Manually disable glows. For somereason they still get initialized
        this->HideOfficalSaberGlows();
    }
    // Update Original menu saber colors
    else{    
#endif
        getLogger().info("Original menu saber color update"); 
        _SetOriginalSaberColors();

#if QOSMETICS_API_EXISTS
    }
#endif

}
