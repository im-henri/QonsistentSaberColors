#include "main.hpp"
#include "QonsistentSaberColors.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

//MainMenuViewController::DidActivate

MAKE_HOOK_MATCH(
    MainMenuViewController_DidActivate,
    &GlobalNamespace::MainMenuViewController::DidActivate, 
    void,
    GlobalNamespace::MainMenuViewController* self,
    bool firstActivation, 
    bool addedToHierarchy, 
    bool screenSystemEnabling
) {
    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    modManager._postfix_MainMenuViewController_DidActivate();
}

void QonsistentSaberColors::_Hook_MainMenuViewController_DidActivate(){
    INSTALL_HOOK(getLogger(), MainMenuViewController_DidActivate);
}