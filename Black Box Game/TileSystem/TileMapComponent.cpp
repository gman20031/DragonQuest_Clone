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
}

//////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////

BlackBoxEngine::FVector2 TileMapComponent::GetGameCoordsFromTilePos(uint32_t x, uint32_t y)
{
    FVector2 offset = m_pTransform->m_position;
    offset.x += x * m_tileSize;
    offset.y += y * m_tileSize;

    offset.x -= m_cachedXOffset;
    offset.y -= m_cachedYOffset;

    return offset;
}

uint32_t TileMapComponent::GetIndex(uint32_t x, uint32_t y) const
{
    return x + (m_width * y);
}

void TileMapComponent::RenderTileAt(uint32_t x, uint32_t y)
{
    if (m_tileSize == 0)
        BB_LOG(LogType::kWarning, "Tile Size is zero, nothing is being drawn");

    Actor::Id id = m_rawMap[GetIndex(x, y)];
    const auto& pActor = m_pTileActorManager->GetActor(id);
    auto* pInfo = pActor->GetComponent<TileInfoComponent>();
    std::shared_ptr<BB_Texture> pTexture = pInfo->GetTexture();
    BB_FRectangle source = pInfo->GetSourceRectangle();
    if (!pTexture)
        return;

    FVector2 pos = GetGameCoordsFromTilePos(x, y);
    BB_FRectangle dest(pos.x, pos.y, static_cast<float>(m_tileSize), static_cast<float>(m_tileSize) );

    m_pRenderer->DrawTextureGame(
        pTexture.get() , &source, &dest
    );
}

void TileMapComponent::FreeTileMap()
{
    m_rawMap.clear();
}

void TileMapComponent::SaveMap([[maybe_unused]]BlackBoxEngine::XMLElementParser parser)
{
    const char rowName[] = "row";
    char* pSavedRowString = new char[m_width];
    for (size_t i = 0; i < m_height; ++i)
    {
        std::memcpy(pSavedRowString, m_rawMap.c_str() + (i * m_width), m_width);
        parser.NewChildVariable(rowName, pSavedRowString);
    }
    delete[] pSavedRowString;
}

void TileMapComponent::LoadMap([[maybe_unused]]BlackBoxEngine::XMLElementParser parser)
{
    auto element = parser.GetChildElement("row");
    const char* pNextRow = nullptr;
    size_t height = 0;
    size_t width = 0;

    while (element)
    {
        ++height;
        element.GetText(&pNextRow);
        m_rawMap.append(pNextRow);
        if (width == 0)
            width = m_rawMap.length();
        element = element.GetSibling();
    }

    m_width = static_cast<uint32_t>(width);
    m_height = static_cast<uint32_t>(height);
}

//////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////

const TileMapComponent::ActorPtr& TileMapComponent::GetTileAt(uint32_t tileX, uint32_t tileY)
{
    char id = 0;
    auto index = GetIndex(tileX, tileY);
    if (index < 0 || index > m_rawMap.size())
        BB_LOG(LogType::kError, "Attempted to get tile out of bounds");
    else
        id = m_rawMap[index];
    return BlackBoxGame::Get()->GetTileActorManager()->GetActor(static_cast<uint32_t>(id));
}

const TileMapComponent::ActorPtr& TileMapComponent::GetTileAtGamePosition(uint32_t gameX, uint32_t gameY)
{
    uint32_t x = gameX / m_tileSize;
    uint32_t y = gameY / m_tileSize;
    return GetTileAt(x, y);
}

const TileMapComponent::ActorPtr& TileMapComponent::GetTileAtGamePosition(const FVector2 pos)
{
    uint32_t x = static_cast<uint32_t>(pos.x);
    uint32_t y = static_cast<uint32_t>(pos.y);
    return GetTileAtGamePosition(x, y);
}

const TileMapComponent::ActorPtr& TileMapComponent::GetEncounterAtGame( [[maybe_unused]]BlackBoxEngine::FVector2 pos )
{
    uint32_t x = static_cast<uint32_t>(pos.x) / m_tileSize;
    uint32_t y = static_cast<uint32_t>(pos.y) / m_tileSize;
    BB_LOG( LogType::kMessage, "Encounter should occur" );
    return m_encounterHandler.GetEncounterActorAtTile( x, y );
}

void TileMapComponent::SetAnchorPoint(BlackBoxEngine::BB_AnchorPoint anchor)
{
    m_anchorPoint = anchor;

    float totalWidth = static_cast<float>(m_width * m_tileSize);
    float totalHeight = static_cast<float>(m_height * m_tileSize);

    m_cachedXOffset = 0;
    m_cachedYOffset = 0;

    using enum BB_AnchorPoint;
    // offset this position to be different based on the anchor point
    switch (m_anchorPoint)
    {
    case kTopLeft:                                                                          break;
    case kTopMiddle:   m_cachedXOffset = totalWidth / 2;                                    break;
    case kTopRight:    m_cachedXOffset = totalWidth;                                        break;
    case kCenterLeft:                                    m_cachedYOffset = totalHeight / 2; break;
    case kCenterTrue:  m_cachedXOffset = totalWidth / 2; m_cachedYOffset = totalHeight / 2; break;
    case kCenterRight: m_cachedXOffset = totalWidth;     m_cachedYOffset = totalHeight / 2; break;
    case kBotLeft:                                       m_cachedYOffset = totalHeight;     break;
    case kBotmiddle:   m_cachedXOffset = totalWidth / 2; m_cachedYOffset = totalHeight;     break;
    case kBotRight:    m_cachedXOffset = totalWidth;     m_cachedYOffset = totalHeight;     break;
    }
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
    std::string newMap;
    newMap.resize(newSize);
    if (newWidth < m_width)
    {
        for (uint32_t x = 0 ; x < newWidth; ++x)
        {
            for(uint32_t y = 0; y < m_height; ++y)
            {
                uint32_t newIndex = (x + newWidth * y);
                uint32_t oldIndex = GetIndex(x, y);
                newMap[newIndex] = m_rawMap[oldIndex];
            }
        }
    }
    else
    {
        newMap = m_rawMap;
        for (uint32_t x = m_width; x < newWidth; ++x)
        {
            for (uint32_t y = 0; y < m_height; ++y)
            {
                uint32_t newIndex = (x + newWidth * y);
                newMap[newIndex] = padTile;
            }
        }
    }
    m_width = newWidth;
    m_rawMap = newMap;
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
    std::string newMap;
    newMap.resize(newSize);

    if (newHeight < m_height)
    {
        for (uint32_t x = 0; x < m_width; ++x)
        {
            for (uint32_t y = 0; y < newHeight; ++y)
            {
                uint32_t newIndex = (x + m_width * y);
                uint32_t oldIndex = GetIndex(x, y);
                newMap[newIndex] = m_rawMap[oldIndex];
            }
        }
    }
    else
    {
        newMap = m_rawMap;
        for (uint32_t x = 0; x < m_width; ++x)
        {
            for (uint32_t y = m_height; y < newHeight; ++y)
            {
                uint32_t index = (x + m_width * y);
                newMap[index] = padTile;
            }
        }
    }
    m_height = newHeight;
    m_rawMap = newMap;
}

void TileMapComponent::SetNewTileMap(uint32_t height, uint32_t width, uint32_t tileSize, char* pTileMap)
{
    FreeTileMap();
    m_height = height;
    m_width = width;
    m_tileSize = tileSize;
    m_mapSize = width * height;
    m_rawMap = pTileMap;
}

void TileMapComponent::SetNewTileMap(uint32_t height, uint32_t width, uint32_t tileSize, char tile)
{
    FreeTileMap();
    m_height = height;
    m_width = width;
    m_tileSize = tileSize;
    m_mapSize = width * height;
    m_rawMap.resize(m_mapSize, tile);
}   

void TileMapComponent::SetTileAt(uint32_t x, uint32_t y, char tile)
{
    m_rawMap[GetIndex(x, y)] = tile;
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
    parser.NewChildVariable("AnchorPoint", m_anchorPoint );
    parser.NewChildVariable("HasEncounters", m_hasEncounters);
    auto element = parser.InsertNewChild("TileMap");
    SaveMap(element);
}

void TileMapComponent::Load(const BlackBoxEngine::XMLElementParser parser)
{
    parser.GetChildVariable("TileSize",   &m_tileSize);
    parser.GetChildVariable("AnchorPoint",&m_anchorPoint);
    parser.GetChildVariable("HasEncounters", &m_hasEncounters );
    auto element = parser.GetChildElement("TileMap");
    SetAnchorPoint(m_anchorPoint);
    LoadMap(element);
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
