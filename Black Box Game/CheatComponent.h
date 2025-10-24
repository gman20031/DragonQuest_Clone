#pragma once

#include <Actors/Component.h>

#include "PlayerMovementComponent.h"

class PlayerCheatComponent : public BlackBoxEngine::Component
{
    GENERATE_ID( "PlayerCheatComponent" );

    std::vector<uint64_t> m_inputIds;

public:
    PlayerCheatComponent( BlackBoxEngine::Actor* pOwner ) : BlackBoxEngine::Component( pOwner ) {};
    virtual ~PlayerCheatComponent();

    virtual void Start() override;

};
