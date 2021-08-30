#include "main.hpp"

#include "QonsistentSaberColors.hpp"
#include "Config.hpp"

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "HMUI/Touchable.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getModConfig().Init(modInfo);
    
    getLogger().info("Completed setup!");
}

void QuestUI_DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if(firstActivation) {
        getLogger().info("QuestUI First Activation !");
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();

        UnityEngine::GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
        UnityEngine::Transform*  parent = container->get_transform();

        auto toggle = QuestUI::BeatSaberUI::CreateToggle( parent, "Colored Laser Pointer",  getModConfig().laserPointerColors.GetValue(), [](bool value) -> void { 
            getModConfig().laserPointerColors.SetValue(value, true);
            modManager.UpdateColors();
        });

        QuestUI::BeatSaberUI::AddHoverHint(toggle->get_gameObject(), "Laser go pew-pew-pew.");

    }
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    
    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, QuestUI_DidActivate);

    getLogger().info("Installing hooks...");
    modManager.InstallHooks();
    getLogger().info("Installed all hooks!");
}