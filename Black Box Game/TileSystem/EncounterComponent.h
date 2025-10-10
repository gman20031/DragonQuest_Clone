#pragma once

#include <Actors/Component.h>


class EncounterComponent : public BlackBoxEngine::Component
{
    GENERATE_ID( "EncounterComponent" );

    //bool m_render;

public:
    EncounterComponent( BlackBoxEngine::Actor* pOwner ) : Component( pOwner ) {};


    void StartEncounter();
    //void  BuildCombatUI();
};