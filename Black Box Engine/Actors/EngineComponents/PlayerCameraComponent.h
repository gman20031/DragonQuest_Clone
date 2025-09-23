#pragma once

#include "../Component.h"


namespace BlackBoxEngine
{
    class TransformComponent;
    class BB_Camera;

    class PlayerCameraComponent : public Component
    {
    public:
        GENERATE_ID("PlayerCameraComponent");
    private:
        TransformComponent* m_pTransform = nullptr;
        BB_Camera* m_pGameCamera = nullptr;
        
    public:
        PlayerCameraComponent(Actor* pActor);
        virtual ~PlayerCameraComponent() = default;

        virtual void Update() override;
        virtual void Start() override;
    };
}