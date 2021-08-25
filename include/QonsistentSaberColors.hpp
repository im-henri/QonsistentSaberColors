#pragma once

#include "main.hpp"

#include "GlobalNamespace/OculusVRHelper.hpp"
#include "UnityEngine/Color.hpp"

class QonsistentSaberColors{
    public:
        
        void InstallHooks();
        
        // Public variables
        bool initialColorsSet = false;
        const UnityEngine::Color defaultRightColor{0.156863, 0.556863, 0.823529, 1.000000};
        const UnityEngine::Color defaultLeftColor {0.784314, 0.078431, 0.078431, 1.000000}; 

        // Public Methods
        void UpdateColors();  

    private:
        // Hook Install Calls
        void _Hook_ColorSchemesSettings_SetColorSchemeForId();
        void _Hook_SceneManager_SetActiveScene();
        void _Hook_OculusVRHelper_VRControllersInputManager();
        void _Hook_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged();
        void _Hook_ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx();
};

extern QonsistentSaberColors modManager;