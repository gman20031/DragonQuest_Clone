#pragma once

#include "../Component.h"
#include "CollisionManager.h"

namespace BlackBoxEngine
{
    class ColliderBorder : public Component
    {
        GENERATE_ID("ColliderBorder");

    public:
        ColliderBorder(Actor* pActor);
        
        virtual void Start() override;
    };

}

