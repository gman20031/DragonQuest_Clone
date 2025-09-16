#pragma once

#include <vector>
#include <memory>

#include <Actors/Component.h>
#include <BlackBoxManager.h>

#include "TileActorManager.h"

class TileMapComponent : public BlackBoxEngine::Component
{
    using ActorPtr = std::unique_ptr<BlackBoxEngine::Actor> ;
public:
    GENERATE_ID("TileMapComponent");
    inline static const char* kTileLookupPath = "../Assets/Tiles/TileLookup.xml";
private:
    BlackBoxEngine::TransformComponent* m_pTransform = nullptr;
    BlackBoxEngine::BB_Renderer* m_pRenderer = nullptr;
    TileActorManager* m_pTileActorManager = nullptr;
    char* m_pRawMap = nullptr; 
    uint32_t m_mapSize  = 0;
    uint32_t m_height   = 0;
    uint32_t m_width    = 0;
    uint32_t m_tileSize = 0;

private:
    BlackBoxEngine::FVector2 GetGameCoordsFromTilePos(uint32_t x, uint32_t y);
    uint32_t GetIndex(uint32_t x, uint32_t y) const;

    void RenderTileAt(uint32_t x, uint32_t y);
    void FreeTileMap();
public:
    TileMapComponent(BlackBoxEngine::Actor* pActor);
    ~TileMapComponent();
    TileMapComponent(const TileMapComponent&) = delete;
    TileMapComponent(TileMapComponent&&) = delete;
    TileMapComponent& operator=(const TileMapComponent&) = delete;
    TileMapComponent& operator=(TileMapComponent&&) = delete;

    uint32_t GetTileSize() const { return m_tileSize; }
    uint32_t GetMapWidth() const { return m_width; }
    uint32_t GetMapHeight() const { return m_height; }
    const ActorPtr& GetTileAt(uint32_t x, uint32_t y);
    
    /**
     * @brief Simply changes the size of each tile in the tileMap
     */
    void SetTileSize(uint32_t tileSize) { m_tileSize = tileSize; }

    /**
     * @brief Changes the width of the tilemap, and makes any newly created tiles with padTile
     */
    void SetNewWidth(uint32_t newWidth, char padTile = 0);

    /**
     * @brief Changes the height of the tilemap, and makes any newly created tiles with padTile
     */
    void SetNewHeight(uint32_t newHeight, char padTile = 0);

    /**
     * @brief Completely replaces the current map with a map of set height, width, and tile size.
     * @brief The tile will be set to value of the null termniated string pTileMap. If pTileMap is not
     * @brief the right length it is undefined.
     * @param height of the new map
     * @param width of the new map
     * @param tileSize : the size of each individual tile
     * @param pTileMap : the string of characters used to populate the map
     */
    void SetNewTileMap(uint32_t height, uint32_t width, uint32_t tileSize, char* pTileMap);
    /**
     * @brief Completely replaces the current map with a map of set height, width, and tile size.
     * @brief Every tile in the tilemap will be set to the tile represented by the single char 'tile'
     * @param height of the new map
     * @param width of the new map
     * @param tileSize : the size of each individual tile
     * @param tile : the entire map will be set to the tile represented to by this char
     */
    void SetNewTileMap(uint32_t height, uint32_t width, uint32_t tileSize, char tile);

    /**
     * @brief replaces the tile at pos x,y with the specified tile.
     */
    void SetTileAt(uint32_t x, uint32_t y, char tile);

    virtual void Render() override;
    virtual void Save(BlackBoxEngine::XMLElementParser parser) override;
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override;
    virtual void Start() override;
};

