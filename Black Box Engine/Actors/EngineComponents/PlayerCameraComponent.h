#pragma once

#include "../Component.h"


namespace BlackBoxEngine
{
    class TransformComponent;
    class Camera;

    class PlayerCameraComponent : public Component
    {
    public:
        GENERATE_ID("PlayerCameraComponent");
    private:
        TransformComponent* m_pTransform = nullptr;
        Camera* m_pGameCamera = nullptr;
        
    public:
        PlayerCameraComponent(Actor* pActor);

        virtual void Update() override;
        virtual void Start() override;
    };
}