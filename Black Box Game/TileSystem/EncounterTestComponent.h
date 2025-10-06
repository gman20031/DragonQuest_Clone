#pragma once

#include<Actors/Component.h>
#include <Graphics/Sprite.h>

class EncounterTest : public BlackBoxEngine::Component
{
    GENERATE_ID( "EncounterTest" );
    BlackBoxEngine::Sprite m_examplePicture;
    BlackBoxEngine::Sprite m_exampleActions;

public:
    EncounterTest( BlackBoxEngine::Actor* pActor ) : Component( pActor ) {};


    virtual void Start() override;
    virtual void Render() override;
};
