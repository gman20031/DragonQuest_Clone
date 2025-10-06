#include "EncounterTestComponent.h"

#include <Resources/ResourceManager.h>
#include <BlackBoxManager.h>

using namespace BlackBoxEngine;

void EncounterTest::Start()
{
    m_examplePicture.SetTexture( "../Assets/Sprites/BlueSlime.jpg" );
    m_exampleActions.SetTexture( "../Assets/UI/Combat_CommandBox.png" );

}

void EncounterTest::Render()
{
    BB_FRectangle frect = {};
    frect.x = 100;
    frect.y = 90;
    frect.h = 80;
    frect.w = 112;
    m_examplePicture.Render( frect , true);

    frect.x = 120;
    frect.y = 8;
    frect.h = 80;
    frect.w = 112;
    m_exampleActions.Render( {150, 16, 64, 32}, true);
}
