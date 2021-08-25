#include "main.hpp"
#include "QonsistentSaberColors.hpp"

#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"



MAKE_HOOK_MATCH(
    SceneManager_SetActiveScene, 
    &UnityEngine::SceneManagement::SceneManager::SetActiveScene, 
    bool,
    UnityEngine::SceneManagement::Scene scene
) {
    bool ret = SceneManager_SetActiveScene(scene);
    
    auto sceneName = scene.get_name();
    bool is_MainMenu = (to_utf8(csstrtostr(  sceneName )).compare("MainMenu") == 0);
    bool is_ShaderWarmup = (to_utf8(csstrtostr(  sceneName )).compare("ShaderWarmup") == 0);
    bool is_HealthWarning = (to_utf8(csstrtostr(  sceneName )).compare("HealthWarning") == 0);

    if ( !modManager.initialColorsSet ){
        if((is_MainMenu == true) || (is_ShaderWarmup == true) || (is_HealthWarning == true)){
            getLogger().info("Initial color update");
            modManager.UpdateColors();
            modManager.initialColorsSet = true;
        }
    }

    return ret;
}

void QonsistentSaberColors::_Hook_SceneManager_SetActiveScene(){
    
    INSTALL_HOOK(getLogger(), SceneManager_SetActiveScene);
}