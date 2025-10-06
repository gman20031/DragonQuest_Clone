#include "TileInfoComponent.h"

#include <Math/Random.h>
#include <Resources/ResourceManager.h>
#include <BlackBoxManager.h>

using namespace BlackBoxEngine;

TileInfoComponent::TileInfoComponent(Actor* pActor)
    : Component(pActor)
    , m_imageSource(0,0,0,0)
{
}

void TileInfoComponent::SetTexture(const char* pFilePath)
{
    m_pTexturePath = pFilePath;
    auto* pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
    m_pTexture = ResourceManager::GetTexture(pRenderer, pFilePath);
}

void TileInfoComponent::Save(XMLElementParser parser)
{
    parser.NewChildVariable("TextureFilePath", m_pTexturePath);
    parser.NewChildVariable("IsWalkable", m_isWalkable);

    parser.NewChildVariable( "EncounterChance", m_encounterChance );

    auto element = parser.InsertNewChild("ImageSource");
    element.NewChildVariable("X", m_imageSource.x);
    element.NewChildVariable("Y", m_imageSource.y);
    element.NewChildVariable("W", m_imageSource.w);
    element.NewChildVariable("H", m_imageSource.h);

}

void TileInfoComponent::Load(const XMLElementParser parser)
{
    parser.GetChildVariable("TextureFilePath", &m_pTexturePath);
    parser.GetChildVariable("IsWalkable", &m_isWalkable);

    auto element = parser.GetChildElement( "EncounterChance" );
    if ( element )
        parser.GetChildVariable( "EncounterChance", &m_encounterChance );

    element = parser.GetChildElement("ImageSource");
    if (element)
    {
        element.GetChildVariable("X", &m_imageSource.x);
        element.GetChildVariable("Y", &m_imageSource.y);
        element.GetChildVariable("W", &m_imageSource.w);
        element.GetChildVariable("H", &m_imageSource.h);
    }
}

void TileInfoComponent::Start()
{
    SetTexture(m_pTexturePath);
}

bool TileInfoComponent::CheckEncounter() const
{
    if ( m_encounterChance <= 0 )
        return false;
    using namespace BlackBoxEngine::Random::Global;
    uint64_t chance = BB_Xoshiro256().GetRandomInRange<uint64_t>( 1, m_encounterChance);
    if ( chance <= 1 )
        return true;
    return false;
}
