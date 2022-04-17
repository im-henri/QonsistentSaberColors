#pragma once

#include "main.hpp"

#include "GlobalNamespace/OculusVRHelper.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"

class QonsistentSaberColors{
    public:
        // All the hooks get installed here
        void InstallHooks();
        

        bool initialColorsSet = false;
        const UnityEngine::Color defaultLeftColor {0.784314, 0.078431, 0.078431, 1.000000}; 
        const UnityEngine::Color defaultRightColor{0.156863, 0.556863, 0.823529, 1.000000};

        UnityEngine::Color colA = defaultLeftColor;  //Target colors 
        UnityEngine::Color colB = defaultRightColor; //Target colors

        /*  Storing "coloring_objects" to reduce UnityEngine...::Find() Calls
        ColorScheme used for changing color itself.
        Il2CppObject is either SetSaberFakeGlowColor or SetSaberGlowColor script (For calling SetColor() method) */
        std::pair<Il2CppObject*, GlobalNamespace::ColorScheme*> r_FakeGlow0;
        std::pair<Il2CppObject*, GlobalNamespace::ColorScheme*> r_FakeGlow1;
        std::pair<Il2CppObject*, GlobalNamespace::ColorScheme*> r_Glowing;
        std::pair<Il2CppObject*, GlobalNamespace::ColorScheme*> r_Normal;

        std::pair<Il2CppObject*, GlobalNamespace::ColorScheme*> l_FakeGlow0;
        std::pair<Il2CppObject*, GlobalNamespace::ColorScheme*> l_FakeGlow1;
        std::pair<Il2CppObject*, GlobalNamespace::ColorScheme*> l_Glowing;
        std::pair<Il2CppObject*, GlobalNamespace::ColorScheme*> l_Normal;

        // --- Public Methods ---
        // Simply sets the color to invisible (Does not disable gameobject due to it not being seen in UnityEngine..Find() method)
        void HideOfficalSaberGlows();
        // Updates colors of sabers. (Either original sabers or qosmetics sabers)
        void UpdateColors();  
        
        //void _Update_FingerSaberColors();

        // Postfix methods for Hooks to call. (Should not be called anywhere manually).
        void _postfix_SetSceneActive(UnityEngine::SceneManagement::Scene& scene);
        void _postfix_MainMenuViewController_DidActivate();
        void _postfix_VRPointer_CreateLaserPointerAndLaserHit();
        void _postfix_ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx();
        void _postfix_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged();
        void _postfix_ColorSchemesSettings_SetColorSchemeForId();

    private:
        bool _coloringObjectsDirty = true;

        // Private Methods
        // Updates laser pointer that is currently visible.
        void _Update_LaserPointerColor();
        // Original saber color updates
        void _SetOriginalSaberColors();
        // Original saber glow color updates. 
        void _SetGlowColors(UnityEngine::Color colA, UnityEngine::Color colB);


    #if QOSMETICS_API_EXISTS
        // Qos saber trail color update
        void _UpdateQosMenuPointerTrails();
        // Qos saber color update
        bool _UpdateQosMenuPointerColor(UnityEngine::Color col, bool rightHand);
    #endif
    
        // Finger Saber Finger Trail Colors
        //void _Update_FingerSaberColors();

        // Sets _coloringObjectsDirty to true. Meaning that required gameobjects need to be found again with UnityEngine..Find(..).
        void _SetColoringObjectsDirty();
        // Sets valid pointers to "coloring_objects".
        void _update_coloring_objects();


        // Hook Install Calls
        void _Hook_ColorSchemesSettings_SetColorSchemeForId();
        void _Hook_SceneManager_SetActiveScene();
        void _Hook_ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged();
        void _Hook_ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx();
        void _Hook_MainMenuViewController_DidActivate();
        
        void _Hook_VRPointer_CreateLaserPointerAndLaserHit();
        void _Hook_OculusVRHelper_VRControllersInputManager();
};

extern QonsistentSaberColors modManager;