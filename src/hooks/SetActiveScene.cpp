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

    modManager._postfix_SetSceneActive(scene);
 
    return ret;
}

void QonsistentSaberColors::_Hook_SceneManager_SetActiveScene(){
    
    INSTALL_HOOK(getLogger(), SceneManager_SetActiveScene);
}