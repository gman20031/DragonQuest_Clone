#pragma once

#include "Window.h"
#include "Renderer.h"
#include "../Actors/ActorManager.h"
#include "../Actors/Collision/CollisionManager.h"

namespace BlackBoxEngine
{
    // Scenes are A BADLY NAMED CONCEPT
    // 
    // 
    // Actor manager of the actors in the scene
    // the window the scene is projected onto
    // the renderer attached to the window
    //struct BB_Scene
    //{
    //    BB_Scene(std::unique_ptr<BB_Window> pWindow);
    //    ~BB_Scene();
    //    BB_Scene(const BB_Scene&) = delete;
    //    BB_Scene(BB_Scene&& other) = default;
    //    void operator=(const BB_Scene&) = delete;
    //    BB_Scene& operator=(BB_Scene&& other) = default;

    //    ActorManager* m_pSceneActorManager = nullptr;
    //    CollisionManager* m_pCollisionManager = nullptr;
    //    std::unique_ptr<BB_Window> m_pWindow = nullptr;
    //    BB_Renderer* Renderer() const { return m_pWindow->GetRenderer(); }
    //};

}