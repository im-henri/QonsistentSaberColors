#include "main.hpp"
#include "QonsistentSaberColors.hpp"

#include "VRUIControls/VRPointer.hpp"

MAKE_HOOK_MATCH(
    VRPointer_CreateLaserPointerAndLaserHit,
    &VRUIControls::VRPointer::CreateLaserPointerAndLaserHit,
    void,
    VRUIControls::VRPointer* self
) {
    VRPointer_CreateLaserPointerAndLaserHit(self);

    modManager._postfix_VRPointer_CreateLaserPointerAndLaserHit();
}

void QonsistentSaberColors::_Hook_VRPointer_CreateLaserPointerAndLaserHit(){
    INSTALL_HOOK(getLogger(), VRPointer_CreateLaserPointerAndLaserHit);
}

