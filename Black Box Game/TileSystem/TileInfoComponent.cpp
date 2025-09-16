#include "TileInfoComponent.h"

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
    auto element = parser.InsertNewChild("ImageSource");
    element.NewChildVariable("X", m_imageSource.x);
    element.NewChildVariable("Y", m_imageSource.y);
    element.NewChildVariable("W", m_imageSource.w);
    element.NewChildVariable("H", m_imageSource.h);

}

void TileInfoComponent::Load(const XMLElementParser parser)
{
    parser.GetChildVariable("TextureFilePath", &m_pTexturePath);
    auto element = parser.GetChildElement("ImageSource");
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
