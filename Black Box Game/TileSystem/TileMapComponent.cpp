#include "TileMapComponent.h"

#include "../BlackBoxGame.h"
#include "TileInfoComponent.h"

using namespace BlackBoxEngine;

//////////////////////////////////////////////////////////////////////
// Ctor
//////////////////////////////////////////////////////////////////////

TileMapComponent::TileMapComponent(BlackBoxEngine::Actor* pActor)
    : Component(pActor)
{
    m_pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
    m_pTileActorManager = BlackBoxGame::Get()->GetTileActorManager();
}

TileMapComponent::~TileMapComponent()
{
    if (m_pRawMap)
        delete m_pRawMap;
}

//////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////

BlackBoxEngine::FVector2 TileMapComponent::GetGameCoordsFromTilePos(uint32_t x, uint32_t y)
{
    FVector2 offset = m_pTransform->m_position;
    offset.x += x * m_tileSize;
    offset.y += y * m_tileSize;
    return offset;
}

uint32_t TileMapComponent::GetIndex(uint32_t x, uint32_t y) const
{
    return (x + m_width * y);
}

void TileMapComponent::RenderTileAt(uint32_t x, uint32_t y)
{
    if (m_tileSize == 0)
        BB_LOG(LogType::kWarning, "Tile Size is zero, nothing is being drawn");

    Actor::Id id = m_pRawMap[GetIndex(x, y)];
    const auto& pActor = m_pTileActorManager->GetActor(id);
    auto* pInfo = pActor->GetComponent<TileInfoComponent>();
    std::shared_ptr<BB_Texture> pTexture = pInfo->GetTexture();
    BB_Rectangle source = pInfo->GetSourceRectangle();
    if (!pTexture)
        return;

    FVector2 pos = GetGameCoordsFromTilePos(x, y);
    BB_Rectangle dest(pos.x, pos.y, (float)m_tileSize, (float)m_tileSize);

    m_pRenderer->DrawTextureGame(
        pTexture.get() , &source, &dest
    );
}

void TileMapComponent::FreeTileMap()
{
    if (m_pRawMap)
        delete[] m_pRawMap;
}

//////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////

const TileMapComponent::ActorPtr& TileMapComponent::GetTileAt(uint32_t x, uint32_t y)
{
    char id = m_pRawMap[GetIndex(x, y)];
    return BlackBoxGame::Get()->GetTileActorManager()->GetActor(static_cast<uint32_t>(id));
}

void TileMapComponent::SetNewWidth(uint32_t newWidth, char padTile)
{
    if (newWidth == 0)
    {
        BB_LOG(LogType::kWarning, "Tilemap width just set to zero");
        FreeTileMap();
        return;
    }
    uint32_t newSize = m_height * newWidth;
    char* pNewMap = new char[newSize];
    if (newWidth < m_width)
    {
        for (uint32_t x = 0; x < newWidth; ++x)
        {
            for (uint32_t y = 0; y < m_height; ++y)
            {
                uint32_t newIndex = (x + newWidth * y);
                uint32_t oldIndex = GetIndex(x, y);
                pNewMap[newIndex] = m_pRawMap[oldIndex];
            }
        }
    }
    else
    {
        std::memcpy(pNewMap, m_pRawMap, m_mapSize);
        for (uint32_t x = m_width; m_width < newWidth; ++x)
        {
            for (uint32_t y = 0; y < m_height; ++y)
            {
                uint32_t index = (x + newWidth * y);
                pNewMap[index] = padTile;
            }
        }
    }
    m_width = newWidth;
    FreeTileMap();
    m_pRawMap = pNewMap;
}

void TileMapComponent::SetNewHeight(uint32_t newHeight, char padTile)
{
    if (newHeight == 0)
    {
        BB_LOG(LogType::kWarning, "Tilemap height just set to zero");
        FreeTileMap();
        return;
    }

    uint32_t newSize = newHeight * m_width;
    char* pNewMap = new char[newSize];

    if (newHeight < m_height)
    {
        for (uint32_t x = 0; x < m_width; ++x)
        {
            for (uint32_t y = 0; y < newHeight; ++y)
            {
                uint32_t newIndex = (x + m_width * y);
                uint32_t oldIndex = GetIndex(x, y);
                pNewMap[newIndex] = m_pRawMap[oldIndex];
            }
        }
    }
    else
    {
        std::memcpy(pNewMap, m_pRawMap, m_mapSize);
        for (uint32_t x = 0; x < m_width; ++x)
        {
            for (uint32_t y = m_height; y < newHeight; ++y)
            {
                uint32_t index = (x + m_width * y);
                pNewMap[index] = padTile;
            }
        }
    }
    m_height = newHeight;
    FreeTileMap();
    m_pRawMap = pNewMap;
}

void TileMapComponent::SetNewTileMap(uint32_t height, uint32_t width, uint32_t tileSize, char* pTileMap)
{
    FreeTileMap();
    m_height = height;
    m_width = width;
    m_tileSize = tileSize;
    m_mapSize = width * height;
    m_pRawMap = pTileMap;
}

void TileMapComponent::SetNewTileMap(uint32_t height, uint32_t width, uint32_t tileSize, char tile)
{
    FreeTileMap();
    m_height = height;
    m_width = width;
    m_tileSize = tileSize;
    m_mapSize = width * height;
    m_pRawMap = new char[m_mapSize];
    for (size_t i = 0; i < m_mapSize; ++i)
        m_pRawMap[i] = tile;
}   

void TileMapComponent::SetTileAt(uint32_t x, uint32_t y, char tile)
{
    m_pRawMap[GetIndex(x, y)] = tile;
}

void TileMapComponent::Render()
{
    for (uint32_t y = 0; y < m_height; ++y)
    {
        for (uint32_t x = 0; x < m_width; ++x)
        {
            RenderTileAt(x, y);
        }
    }
}

void TileMapComponent::Save(BlackBoxEngine::XMLElementParser parser)
{
    parser.NewChildVariable("TileSize", m_tileSize);
    parser.NewChildVariable("Width", m_width);
    parser.NewChildVariable("Height", m_height);
    parser.NewChildVariable("RawMap", m_pRawMap);
}

void TileMapComponent::Load(const BlackBoxEngine::XMLElementParser parser)
{
    parser.GetChildVariable("TileSize", &m_tileSize);
    parser.GetChildVariable("Width",   &m_width);
    parser.GetChildVariable("Height",  &m_height);

    m_mapSize = m_width * m_height;
    m_pRawMap = new char[m_mapSize];
    const char* temp = nullptr;
    parser.GetChildVariable("RawMap", &temp);
    std::memcpy(m_pRawMap, temp, m_mapSize);
}

void TileMapComponent::Start()
{
    if (!m_pTransform)
        m_pTransform = m_pOwner->GetComponent<TransformComponent>();
    if (!m_pTransform)
    {
        BB_LOG(LogType::kWarning, "TileMap has no translation component, this component requires a position, Perhaps forgot to save transform?");
        m_pTransform = m_pOwner->AddComponent<TransformComponent>();
    }
}
