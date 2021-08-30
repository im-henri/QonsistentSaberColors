#pragma once

#include "UnityEngine/GameObject.hpp"

#include <sstream>
#include <string>


namespace ModDebugUtils{
    void LogAllFields(Il2CppObject* il2CppObj, std::string prependText = "");
    
    void LogAllProperties(Il2CppObject* il2CppObj, std::string prependText = "");

    void LogAllMethods(Il2CppObject* il2CppObj, std::string prependText = "");

    template <typename T>
    extern void Log_GetComponents(UnityEngine::GameObject* gameobj);
    
    template <typename T>
    extern void Log_GetComponentsInChildren(UnityEngine::GameObject* gameobj);

    void WriteToLog(std::stringstream& logStream);
    void WriteToLog_AllGameObjectsInScene();
}