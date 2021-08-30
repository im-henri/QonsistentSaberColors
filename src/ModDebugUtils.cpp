#include "ModDebugUtils.hpp"

#include "main.hpp"

#include "UnityEngine/Component.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/HideFlags.hpp"
#include "UnityEngine/Resources.hpp"

namespace ModDebugUtils{
    void LogAllFields(Il2CppObject* il2CppObj, std::string prependText){
        auto x = getLogger().WithContext(prependText);
        il2cpp_utils::LogFields(x, il2cpp_utils::ExtractClass(il2CppObj));
    }
    void LogAllProperties(Il2CppObject* il2CppObj, std::string prependText){
        auto x = getLogger().WithContext(prependText);
        il2cpp_utils::LogProperties(x, il2cpp_utils::ExtractClass(il2CppObj));
    }
    void LogAllMethods(Il2CppObject* il2CppObj, std::string prependText){
        auto x = getLogger().WithContext(prependText);
        il2cpp_utils::LogMethods(x, il2cpp_utils::ExtractClass(il2CppObj));
    }
    template <typename T>
    void Log_GetComponents(UnityEngine::GameObject* gameobj){
        auto componentArr = gameobj->GetComponents<T>();
        std::stringstream buff;
        for (int i=0; i<componentArr->Length(); i++){
            auto comp = (*componentArr)[i];
            auto name = comp->ToString();
            if(name) buff << to_utf8(csstrtostr(name)).c_str() << "\n";
        }
        getLogger().info("%s",buff.str().c_str());
    }
    template void Log_GetComponents<UnityEngine::Component*>(UnityEngine::GameObject* gameobj);
    
    template <typename T>
    void Log_GetComponentsInChildren(UnityEngine::GameObject* gameobj){
        auto componentArr = gameobj->GetComponentsInChildren<T>();
        std::stringstream buff;
        for (int i=0; i<componentArr->Length(); i++){
            auto comp = (*componentArr)[i];
            auto name = comp->ToString();
            if(name) buff << to_utf8(csstrtostr(name)).c_str() << "\n";
        }
        getLogger().info("%s",buff.str().c_str());
    }
    template void Log_GetComponentsInChildren<UnityEngine::Component*>(UnityEngine::GameObject* gameobj);
    
    void WriteToLog(std::stringstream& logStream){
        bool success = writefile("sdcard/Android/data/com.beatgames.beatsaber/files/logs/QonsistentSaberColors.log", logStream.str());
        if (success)   getLogger().info("Logging to \"QonsistentSaberColors.log\" successful.");
        else           getLogger().info("Creating log file failed.");
    }
    void WriteToLog_AllGameObjectsInScene(){
        auto allObject = UnityEngine::Resources::FindObjectsOfTypeAll(csTypeOf(UnityEngine::GameObject*));
        std::stringstream buff;
        for(int i=0; i<allObject->Length(); i++){
            auto go_asObj = (*allObject)[i];
            //getLogger().info("Found items: %i", i);
            if (
                !(go_asObj->get_hideFlags() == UnityEngine::HideFlags::NotEditable || go_asObj->get_hideFlags() == UnityEngine::HideFlags::HideAndDontSave)
                )
            {
                //getLogger().info("Go name: %s", to_utf8(csstrtostr(name)).c_str() );
                // TRYING CASTING 
                UnityEngine::GameObject* go = il2cpp_utils::cast<UnityEngine::GameObject, UnityEngine::Object>(go_asObj);
                
                Il2CppString* name = go->get_name(); // C# strings are pointers to UTF-16 strings
                Il2CppString* rootName = go->get_transform()->GetRoot()->get_name();

                UnityEngine::Transform* parentTf = go->get_transform();

                while(name->CompareTo(rootName) != 0){
                    std::stringstream namestream;
                    namestream << to_utf8(csstrtostr(name)).c_str() << "(" << parentTf->GetInstanceID() << ")";
                    buff << namestream.str() << " -> ";
                    
                    parentTf = parentTf->get_parent();
                    name = parentTf->get_gameObject()->get_name();
                }
                //Name: SaberA, RootName: MenuCore
                //buff << i << "\t" << "Name: " << to_utf8(csstrtostr(name)).c_str();
                buff << to_utf8(csstrtostr(rootName)).c_str() << "(" << go->get_transform()->GetRoot()->GetInstanceID() << ")";
                buff << '\n' << '\n';
            }
        }

        WriteToLog(buff);
    }
}